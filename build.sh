#!/bin/bash

set -e

mkdir -p build
cd build

cmake .. \
    -DCMAKE_TOOLCHAIN_FILE="${ANDROID_NDK}"/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=armeabi-v7a \
    -DANDROID_STL=gnustl_static \
    -DANDROID_PIE=OFF \
    -DANDROID_PLATFORM=android-24   

make
