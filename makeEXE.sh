#!/bin/bash 

release=${1:-"NO_RELEASE"}

#
# create build dir if does not exist, cd to build, conan install and then qmake
# 

mkdir -p build
cd build
rm -fr *.app *dmg

#
# conan install
#

conan install .. --build missing
cmd_status=$?
if [[ $cmd_status != 0 ]]
then
    echo "PBE: conan install failed";
    exit $cmd_status;
fi

#
# qmake
#

if [ -n "$release" ] && [ "$release" = "release" ]; then
    echo "******** RELEASE BUILD *************"    
    qmake QMAKE_CXXFLAGS+=-D_SC_RELEASE QMAKE_CXXFLAGS+=-D_INCLUDE_USER_PASS ../PBE.pro
else
    echo "********* NON RELEASE BUILD ********"
    qmake ../PBE.pro QMAKE_CXXFLAGS+=-D_INCLUDE_USER_PASS
fi

cmd_status=$?
if [[ $cmd_status != 0 ]]
then
    echo "PBE: qmake failed";
    exit $cmd_status;
fi

#
# make
#

touch ../main.cpp
make -j 4
cmd_status=$?;
if [[ $cmd_status != 0 ]]
then
    echo "PBE: make failed";
    exit $cmd_status;
fi

# copy examples

if [[ "$OSTYPE" == "darwin"* ]]
then
    cp -r ../Examples ./PBE.app/contents/MacOS
else
    #
    echo "No Examples Copied"
fi

