#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <arpa/inet.h>
#include "proto/hdfs.pb.h"
#include "namenode.h"

using namespace std;
using namespace HDFS;

struct FILEHANDLE {
  string fname;
  int fhandle;
  vector<int> blocks;
};

struct FILEBLOCKS {
  string fname;
  vector<int> block;
};

struct DATANODE {
  int id;
  string host;
  int port;
};

struct BLOCKLOCATIONS {
  int block_id;
  set<int> dn_id_list;
  
  bool operator<(const BLOCKLOCATIONS& rhs) const
  {
    return block_id < rhs.block_id;
  }

  bool operator==(const BLOCKLOCATIONS& rhs) const
  {
    return block_id == rhs.block_id;
  }

  BLOCKLOCATIONS(const int id = 0, const set<int> b = set<int>())
  {
    block_id = id;
    dn_id_list = b;
  }
};


/* GLOBAL DATASTRUCTURES */

int BLOCK_COUNT = 1;

vector<FILEBLOCKS> FileBlocksList;
vector<DATANODE> DataNodeList;
set<BLOCKLOCATIONS> BlockLocationList;
vector<FILEHANDLE> OpenHandleWriteList;
vector<FILEHANDLE> OpenHandleReadList;


char **
openfile_1_svc(char **argp, struct svc_req *rqstp)
{
  OpenFileRequest req;
  OpenFileResponse rsp;
  
  string str_arg(*argp);
  req.ParseFromString(str_arg);

  int fstatuserror = 0;
  for (unsigned int i=0; i< FileBlocksList.size(); i++) {
    if (req.filename() == FileBlocksList[i].fname) {
      fstatuserror = 1;
    }
  }

  for (unsigned int i=0; i < OpenHandleWriteList.size(); i++) {
    if (req.filename() == OpenHandleWriteList[i].fname) {
      fstatuserror = 1;
    }
  }

  if (!fstatuserror) {

    int newhandle;
    if (!OpenHandleWriteList.empty()) { //not empty
      newhandle = OpenHandleWriteList.back().fhandle + 1;
    }
    else {
      newhandle = 1;
    }

    FILEHANDLE fh;
    fh.fname = req.filename();
    fh.fhandle = newhandle;

    OpenHandleWriteList.push_back(fh);

    rsp.set_status(1);
    rsp.set_handle(newhandle);
  }
  else {
    rsp.set_status(0);
  }

  // serialize this bitch!
  string str;
  rsp.SerializeToString(&str);
  static char *cstr = new char[str.length() + 1];
  strcpy(cstr, str.c_str());
  return &cstr;
}

char **
getblocklocations_1_svc(char **argp, struct svc_req *rqstp)
{
  static char * result;
  return &result;
}

char **
assignblock_1_svc(char **argp, struct svc_req *rqstp)
{
  AssignBlockRequest req;
  string str_arg (*argp);
  req.ParseFromString(str_arg);

  AssignBlockResponse rsp;

  if ( !DataNodeList.empty() ) { // DN available
    int MAX_DN = 2; //maybe make this global?
    int NUM_DN = DataNodeList.size();
    int BL_NUM = BLOCK_COUNT++;
    vector<int> shuf;

    rsp.set_status(20);

    BlockLocations *bl = rsp.mutable_newblock();// = new BlockLocations;
    bl->set_blocknumber(BL_NUM);

    for (int i=0; i< NUM_DN; i++) {
      shuf.push_back(i);
    }
    while (NUM_DN && MAX_DN) {
      srand ( time (NULL) );
      int j = rand() % NUM_DN;
      int t = shuf[NUM_DN-1];
      shuf[NUM_DN-1] = shuf[j];
      shuf[j] = t;
      NUM_DN--;
      MAX_DN--;
      DataNodeLocation *dnl = bl->add_locations();
      dnl->set_ip( DataNodeList[shuf[t]].host );
      dnl->set_port( DataNodeList[shuf[t]].port );
    }
    for (int i=0; i< DataNodeList.size(); i++) {
      cout << shuf[i] << " ";
    }
    cout << endl;
    int h = req.handle();
    for (unsigned int i=0; i< OpenHandleWriteList.size(); i++) {
      if (OpenHandleWriteList[i].fhandle == h) {
        OpenHandleWriteList[i].blocks.push_back(BL_NUM);
      }
    }
  }

  int size = rsp.ByteSize();
  rsp.set_status(size);
  static char *cstr4 = new char[size];
  rsp.SerializeToArray(cstr4, size);

  return &cstr4;
}

char **
closefile_1_svc(char **argp, struct svc_req *rqstp)
{
  CloseFileRequest req;
  req.ParseFromString(*argp);
  for (unsigned int i=0; i < OpenHandleWriteList.size(); i++) {
    if ( OpenHandleWriteList[i].fhandle == req.handle() ) {
      FILEBLOCKS fb;
      fb.fname = OpenHandleWriteList[i].fname;
      fb.block = OpenHandleWriteList[i].blocks;
    }
    OpenHandleWriteList.erase(OpenHandleWriteList.begin() +i);
  }

  CloseFileResponse rsp;  
  int size = rsp.ByteSize();
  static char *cstr = new char[size];
  rsp.SerializeToArray(cstr, size);

  rsp.set_status(0); // no errors

  return &cstr;
}

char **
list_1_svc(char **argp, struct svc_req *rqstp)
{
  static char * result;
  return &result;
}

char **
sendblockreport_1_svc(char **argp, struct svc_req *rqstp)
{
  string str_arg(*argp);
  BlockReportRequest req;
  req.ParseFromString(str_arg);

  int dn_id = req.id();
  vector<int> bl;
  BLOCKLOCATIONS tb;
  set<BLOCKLOCATIONS>::iterator it;
  for (int i = 0; i < req.blocknumbers_size(); i++) 
  {
    int b_id = req.blocknumbers(i);
    bl.push_back(b_id);

    tb.block_id = b_id;
    tb.dn_id_list = set<int>();
    tb.dn_id_list.insert(dn_id);

    it = BlockLocationList.find(b_id);
    if (it == BlockLocationList.end())
    {
      BlockLocationList.insert(tb);
    }
    else 
    { // update set
      BLOCKLOCATIONS nb = *it;
      nb.dn_id_list.insert(dn_id);
      BlockLocationList.erase(it);
      BlockLocationList.insert(nb);
    }
  }

  fprintf(stderr, "BLOCKLOCATIONLIST FOLLOWS :\n");
  for (it = BlockLocationList.begin(); it!= BlockLocationList.end(); it++) {
    fprintf(stderr, "%d : ", it->block_id);
    set<int>::iterator jt = it->dn_id_list.begin();
    for (jt = it->dn_id_list.begin(); jt != it->dn_id_list.end(); jt++)
      fprintf(stderr, "%d ", *jt);
    fprintf(stderr, "\n");
  }

  BlockReportResponse rsp;
  rsp.add_status(1);

  // serialize this bitch!
  string str;
  rsp.SerializeToString(&str);
  static char *cstr = new char[str.length() + 1];
  strcpy(cstr, str.c_str());
  return &cstr;

}

char **
sendheartbeat_1_svc(char **argp, struct svc_req *rqstp)
{
  //static char * result;
  string str_arg(*argp);
  HeartBeatRequest req;
  HeartBeatResponse rsp;
  req.ParseFromString(str_arg);

  //dn sent request with id=1, nn replies with id of dn
  if (req.id()) 
  { 
    // set (char *)ip and (int)port
    char *ip = new char[INET_ADDRSTRLEN+1]; 
    inet_ntop(AF_INET, &(rqstp->rq_xprt->xp_raddr.sin_addr), ip, INET_ADDRSTRLEN);
    int port = rqstp->rq_xprt->xp_raddr.sin_port;

    // find in DataNodeList, append if not found, set req.status -> id;
    int f = 0;
    for (unsigned int i = 0; i < DataNodeList.size(); i++) 
    {
      //if (DataNodeList[i].host == string(ip) && DataNodeList[i].port == port) 
      if (DataNodeList[i].host == string(ip)) 
      {
        rsp.set_status(DataNodeList[i].id);
        f = 1;
        break;
      }
      else 
      {
        fprintf(stderr, "%s:%d %s:%d\n", DataNodeList[i].host.c_str(), DataNodeList[i].port, string(ip).c_str(), port);
      }
    }
    if (!f) 
    {
      DATANODE dn;
      dn.host = string(ip);
      dn.port = port;
      if (!DataNodeList.empty()) 
        dn.id = DataNodeList.back().id + 1;
      else 
        dn.id = 1;

      DataNodeList.push_back(dn);
      rsp.set_status(dn.id);
    }

  }
  // serialize this bitch!
  string str;
  rsp.SerializeToString(&str);
  static char *cstr = new char[str.length() + 1];
  strcpy(cstr, str.c_str());
  return &cstr;
  //return &result;
}
