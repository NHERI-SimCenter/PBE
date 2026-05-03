#!/bin/bash
set -euo pipefail

QT=/Users/fmckenna/Qt/6.10.1/macos
PREFIX=$(pwd)/DEPS
ARCH=""

while [[ $# -gt 0 ]]; do
    case "$1" in
        --arch|-a)
            # 1. Capture the value
            ARCH="$2"
            
            # 2. VALIDATION: Check if it's one of the allowed types

            if [[ "$ARCH" != "x86_64" && "$ARCH" != "arm64" ]]; then
                echo "Error: Invalid architecture '$ARCH'."
                echo "Supported architectures: x86_64, arm64"
                exit 1
            fi


            # 3. If valid, set the variables
            PREFIX="${PREFIX}_$ARCH"
            
            shift 2
            ;;
        *)
            echo "Unknown argument: $1"
            echo "Usage: $0 --arch <arm64|x86_64>"
            exit 1
            ;;
    esac
done


mkdir -p "$PREFIX"

if [ ! -d "quazip" ]; then
    echo "--- Cloning QuaZIP repository ---"
    git clone https://github.com/stachenov/quazip.git
fi

cd quazip

CMAKE_ARCH_FLAG=""
if [ -n "${ARCH}" ]; then
    CMAKE_ARCH_FLAG="-DCMAKE_OSX_ARCHITECTURES=${ARCH}"
fi

cmake . \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DQUAZIP_QT_MAJOR_VERSION=6 \
    -DCMAKE_PREFIX_PATH="$QT" \
    ${CMAKE_ARCH_FLAG} \
    -DBUILD_SHARED_LIBS=ON

cmake --build . --config Release
cmake --install .

cd ..

echo "Build complete!"
