#!/bin/bash

QT=/Users/fmckenna/Qt/6.10.1/macos
PREFIX=$(pwd)/DEPS

mkdir -p "$PREFIX"

if [ ! -d "quazip" ]; then
    echo "--- Cloning QuaZIP repository ---"
    git clone https://github.com/stachenov/quazip.git
fi

cd quazip
cmake . \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DQUAZIP_QT_MAJOR_VERSION=6 \
    -DCMAKE_PREFIX_PATH="$QT" \
    -DBUILD_SHARED_LIBS=ON

cmake --build . --config Release
cmake --install .

cd ..

echo "Build complete!"
