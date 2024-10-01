#!/bin/bash 

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
status=$?
if [[ $status != 0 ]]
then
    echo "PBE: conan install failed";
    exit $status;
fi

#
# qmake
#

qmake ../PBE.pro
status=$?
if [[ $status != 0 ]]
then
    echo "PBE: qmake failed";
    exit $status;
fi

#
# make
#

make
status=$?;
if [[ $status != 0 ]]
then
    echo "PBE: make failed";
    exit $status;
fi

# copy examples

if [[ "$OSTYPE" == "darwin"* ]]
then
    cp -r ../Examples ./PBE.app/contents/MacOS
else
    #
    echo "No Examples Copied"
fi

