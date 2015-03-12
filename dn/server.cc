#include <iostream>
#include <string>
#include "proto/hdfs.pb.h"
#include "datanode.h"
#include "namenode.h"

using namespace std;
using namespace HDFS;

const string NN_HOST = "192.168.1.100";
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
  static char * result;
  return &result;
}

char **
sendblockreport_1_arg() 
{
  BlockReportRequest req;

  req.set_id(DN_ID);
  for (unsigned int i=0; i < BlockList.size(); i++) 
    req.add_blocknumbers(BlockList[i]);

  /*
   *req.add_blocknumbers(7);
   *req.add_blocknumbers(3);
   *req.add_blocknumbers(4);
   */

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
