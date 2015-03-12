SERVER = ns

SOURCES.x = namenode.x
SOURCES_SVC.c = namenode_svc.c server.cc
SOURCES_PROTO.c = proto/hdfs.pb.cc

TARGETS = namenode.h namenode_svc.c namenode_clnt.c

CC = g++
CXXFLAGS += -g -Wall
LDLIBS += -lnsl -pthread -lprotobuf -lpthread
RPCGENFLAGS =

# Targets
all :
	rpcgen $(RPCGENFLAGS) $(SOURCES.x)
	$(CC) $(CXXFLAGS) -o $(SERVER) $(SOURCES_PROTO.c) $(SOURCES_SVC.c) $(LDLIBS)

clean:
	$(RM) core $(TARGETS) $(CLIENT) $(SERVER)
