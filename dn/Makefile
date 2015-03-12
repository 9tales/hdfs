CLIENT = dc
SERVER = ds

SOURCES_DN.x = datanode.x 
SOURCES_NN.x = namenode.x
SOURCES_CLNT.c = datanode_clnt.c client.cc
SOURCES_SVC.c = namenode_clnt.c datanode_svc.c server.cc
SOURCES_PROTO.c = proto/hdfs.pb.cc

TARGETS = datanode.h datanode_clnt.c datanode_svc.c namenode.h namenode_clnt.c namenode_svc.c

CC = g++
CXXFLAGS += -g -Wall
LDLIBS += -lnsl -pthread -lprotobuf -lpthread
RPCGENFLAGS =

# Targets
all :
	rpcgen $(RPCGENFLAGS) $(SOURCES_DN.x)
	rpcgen $(RPCGENFLAGS) $(SOURCES_NN.x)
	$(CC) $(CXXFLAGS) -o $(CLIENT) $(SOURCES_PROTO.c) $(SOURCES_CLNT.c) $(LDLIBS)
	$(CC) $(CXXFLAGS) -o $(SERVER) $(SOURCES_PROTO.c) $(SOURCES_SVC.c) $(LDLIBS)

clean:
	$(RM) core $(TARGETS) $(CLIENT) $(SERVER)
