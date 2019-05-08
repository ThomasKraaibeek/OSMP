#!/bin/bash

dirname=$(dirname $0)
cd $dirname
cd OSMPLib
make clean
make
cd ../OSMPRun
make clean
make
cd ../OSMPExecutable
make clean
make
