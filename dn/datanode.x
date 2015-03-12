program DATANODE{
  version DN{
    /* ReadBlockResponse readBlock(ReadBlockRequest)) */
    /* Method to read data from any block given block-number */
    string readBlock(string protobuf<>) = 1;

    /* WriteBlockResponse writeBlock(WriteBlockRequest) */
    /* Method to write data to a specific block */
    string writeBlock(string protobuf<>) = 2;

    /* BlockReportResponse blockReport(BlockReportRequest) */
    /* Get datanode server to send blockReport to namenode server */
    /*string sendBlockReportMsg(string protobuf<>) = 3;*/
    int sendBlockReportMsg(void) = 3;

    /* HeartBeatResponse heartBeat(HeartBeatRequest) */
    /* Get datanode server to send heartBeat to namenode server */
    /*string sendHeartBeatMsg(string protobuf<>) = 4;*/
    int sendHeartBeatMsg(void) = 4;
  } = 1;

} = 0x20150001 ;

