#include <iostream>
#include <string>
#include "proto/hdfs.pb.h"
#include "datanode.h"

using namespace std;
using namespace HDFS;

char **
writeblock_1_arg() {
  WriteBlockRequest wbr;
  string contents("This is sparta");
  wbr.add_data(contents);
  string str;
  wbr.SerializeToString(&str);
  static char *cstr = new char[str.length() + 1];
  strcpy(cstr, str.c_str());
  return &cstr;
}

void
datanode_1(char *host, int opt)
{
  CLIENT *clnt;
  char **result_2;
  int *result_3;
  int *result_4;

#ifndef	DEBUG
  clnt = clnt_create (host, DATANODE, DN, "tcp");
  if (clnt == NULL) {
    clnt_pcreateerror (host);
    exit (1);
  }
#endif	/* DEBUG */

  switch(opt) 
  {
    case 2 :
      result_2 = writeblock_1(writeblock_1_arg(), clnt);
      if (result_2 == (char **) NULL) {
        clnt_perror (clnt, "call failed");
      }
      break;
    case 3 :
      result_3 = sendblockreportmsg_1((void *)NULL, clnt);
      if (result_3 == (int *) NULL) {
        clnt_perror (clnt, "call failed");
      }
      break;

    case 4 :
      result_4 = sendheartbeatmsg_1((void *)NULL, clnt);
      if (result_4 == (int *) NULL) {
        clnt_perror (clnt, "call failed");
      }
      printf("returned with : %d\n", *result_4);
      break;

    default :
      printf("Invalid option\n");
  };

#ifndef	DEBUG
  google::protobuf::ShutdownProtobufLibrary();
  clnt_destroy (clnt);
#endif	 /* DEBUG */
}

int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 3) {
		printf ("usage: %s server_host option\n", argv[0]);
		exit (1);
	}
	host = argv[1];
    int opt = atoi(argv[2]);
	datanode_1 (host, opt);
exit (0);
}
