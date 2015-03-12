#!/bin/bash

sudo systemctl stop rpcbind.service
sudo systemctl stop nfs-server.service 
sudo systemctl start rpcbind.service
sudo systemctl start nfs-server.service 
