#!/bin/bash

git clone --depth 1 https://github.com/NHERI-SimCenter/SimCenterBackendApplications.git

cp ./SimCenterBackendApplications/modules/performUQ/SimCenterUQ/nataf_gsa/CMakeLists.txt.UBUNTU ./SimCenterBackendApplications/modules/performUQ/SimCenterUQ/nataf_gsa/CMakeLists.txt

rm -fr ~/.conan

sudo apt-get install -y liblapack-dev libomp-dev libssl-dev apt-transport-https ca-certificates \
 
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test

sudo apt-get update

sudo apt-get install -y gcc-11 g++-11 gfortran-11

export CC=gcc-11

export CXX=g++-11

export FC=gfortran-11

cd SimCenterBackendApplications

mkdir build

cd build

conan install .. --build missing

cmake ..

cmake --build . --config Release

cmake --install .

cd ../..
