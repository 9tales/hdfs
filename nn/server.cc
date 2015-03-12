#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <arpa/inet.h>
#include "proto/hdfs.pb.h"
#include "namenode.h"

using namespace std;
using namespace HDFS;

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

vector<FILEBLOCKS> FileBlocksList;
vector<DATANODE> DataNodeList;
//vector<BLOCKLOCATIONS> BlockLocationList;
set<BLOCKLOCATIONS> BlockLocationList;

char **
openfile_1_svc(char **argp, struct svc_req *rqstp)
{
  static char * result;
  return &result;
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
  static char * result;
  return &result;
}

char **
closefile_1_svc(char **argp, struct svc_req *rqstp)
{
  static char * result;
  return &result;
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
