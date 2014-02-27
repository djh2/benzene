#!/bin/bash
#export LDFLAGS="-static"
#export LIBS="-lpthread -ldb"
#env|grep LDFLAGS
./configure  --with-boost=/home/leooo/Workspace/hex/libboost_1_45_0 --with-fuego-root=/home/leooo/Workspace/hex/fuego/ 
#make clean;
make 