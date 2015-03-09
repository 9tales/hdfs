% Distributed FS
% Abhinandan Panigrahi, Arpit Merchant

# Initial Setup

## HDFS.proto

```bash
protoc -I=$SRC_DIR --cpp_out=$DST_DIR. $SRC_DIR/HDFS.proto
```

## datanode.x

```bash
rpcgen -Ma datanode.x
```

## namenode.x

```bash
rpcgen -Ma namenode.x
```

