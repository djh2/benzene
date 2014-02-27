#!/bin/bash
export LD_LIBRARY_PATH=/home/leooo/Workspace/hex/libboost_1_45_0/lib:$LD_LIBRARY_PATH

exec /home/leooo/Workspace/hex/benzene/src/wolve/wolve $*

