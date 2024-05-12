#!/bin/sh

# Build kissat-inc-psids
cd solvers/kissat-inc-psids
./configure --quiet --compact --no-proofs 
make
cd ../..

# Unpack the archives
cd preprocess/archives
tar xf eigen-3.4.0.tar.gz
cd ../..

# Build SBVA
make -C preprocess/sbva 
rm -f ./bin/bva
cp preprocess/sbva/bva ./bin

# Build PRSPakisInc
make
rm -f  ./bin/PRSPakisInc
cp PRSPakisInc ./bin
