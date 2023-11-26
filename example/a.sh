#!/bin/bash

set -e

cd ..
sudo ./build.sh
cd example/
make clean
make
./testserver
