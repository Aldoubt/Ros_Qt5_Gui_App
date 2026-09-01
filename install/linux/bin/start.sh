#!/bin/bash
cd "$(dirname "$0")"
export LD_LIBRARY_PATH="$(dirname "$0")/lib:$LD_LIBRARY_PATH"
./agt_robot_hmi
