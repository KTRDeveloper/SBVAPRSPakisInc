#!/bin/sh

make clean
make -C ./preprocess/sbva clean
make -C ./solvers/kissat-inc-psids clean
rm -rf ./preprocess/archives/eigen-3.4.0
