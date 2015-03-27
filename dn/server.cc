#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "proto/hdfs.pb.h"
#include "datanode.h"
#include "namenode.h"

using namespace std;
using namespace HDFS;

const string NN_HOST = "172.17.0.8";
int DN_ID;
vector<int> BlockList;

char **
readblock_1_svc(char **argp, struct svc_req *rqstp)
{
  static char * result;
  return &result;
}

char **
writeblock_1_svc(char **argp, struct svc_req *rqstp)
{
  WriteBlockRequest req;
  req.ParseFromString (*argp);

  string filename;          // string which will contain the result
  ostringstream convert;   // stream used for the conversion
  convert << req.blockinfo().blocknumber();      // insert the textual representation of 'Number' in the characters in the stream
  filename = convert.str(); // set 'Result' to the contents of the stream
  
  // writing file 
  ofstream ofs(filename.c_str(), ios::binary);
  ofs << req.data(0);

  BlockList.push_back(req.blockinfo().blocknumber());

  WriteBlockResponse rsp;
  rsp.set_status(0); //no error
  
  int size = rsp.ByteSize();
  static char *cstr = new char[size];
  rsp.SerializeToArray(cstr, size);

  return &cstr;
}

char **
sendblockreport_1_arg() 
{
  BlockReportRequest req;

  req.set_id(DN_ID);
  for (unsigned int i=0; i < BlockList.size(); i++) 
    req.add_blocknumbers(BlockList[i]);

  // serialize this bitch!
  string str;
  req.SerializeToString(&str);
  static char *cstr = new char[str.length() + 1];
  strcpy(cstr, str.c_str());
  return &cstr;
}

int * 
sendblockreportmsg_1_svc(void *argp, struct svc_req *rqstp)
{
  static int result_dn = 0;

  CLIENT *clnt;
  clnt = clnt_create (NN_HOST.c_str(), NAMENODE, NN, "tcp");
  if (clnt == NULL) {
    clnt_pcreateerror (NN_HOST.c_str());
    result_dn = 1;
  }

  char **result_nn;
  result_nn = sendblockreport_1(sendblockreport_1_arg(), clnt);
  if (result_nn == (char **) NULL) {
    clnt_perror (clnt, "call failed");
    result_dn = 1;
  }

  string s (*result_nn);
  BlockReportResponse rsp;
  rsp.ParseFromString(s);
  fprintf (stderr, "BLOCK REPORT RECEIVED AT SERVER\n");

  clnt_destroy (clnt);

  return &result_dn;
}

char **
sendheartbeat_1_arg() 
{
  HeartBeatRequest req;

  req.set_id(1);

  // serialize this bitch!
  string str;
  req.SerializeToString(&str);
  static char *cstr = new char[str.length() + 1];
  strcpy(cstr, str.c_str());
  return &cstr;
}

int * 
sendheartbeatmsg_1_svc(void *argp, struct svc_req *rqstp)
{
  static int result_dn = 0;

  CLIENT *clnt;
  clnt = clnt_create (NN_HOST.c_str(), NAMENODE, NN, "tcp");
  if (clnt == NULL) {
    clnt_pcreateerror (NN_HOST.c_str());
    result_dn = 1;
  }

  char **result_nn;
  result_nn = sendheartbeat_1(sendheartbeat_1_arg(), clnt);
  if (result_nn == (char **) NULL) {
    clnt_perror (clnt, "call failed");
    result_dn = 1;
  }

  string s (*result_nn);
  HeartBeatResponse rsp;
  rsp.ParseFromString(s);
  // mutex required here
  DN_ID = rsp.status();
  fprintf (stderr, "NEW ID : %d\n", DN_ID);

  clnt_destroy (clnt);

  return &result_dn;
}
