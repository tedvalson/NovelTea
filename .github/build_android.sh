#!/bin/sh
set -e

ABI_LIST="armeabi-v7a x86"

for ABI in ${ABI_LIST}
do
    mkdir -p SFML/build/${ABI}
    cd SFML/build/${ABI}
    cmake -DCMAKE_SYSTEM_NAME=Android -DCMAKE_ANDROID_NDK=$1 -DCMAKE_ANDROID_ARCH_ABI=$ABI -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang -DCMAKE_ANDROID_STL_TYPE=c++_static -DCMAKE_ANDROID_API=16 ../..
    cmake --build . --config Release --target install
    cd -

    mkdir -p build/${ABI}
    cd build/${ABI}
    cmake -DBUILD_EDITOR=OFF -DBUILD_TEST=OFF -DBUILD_DOCS=OFF -DCMAKE_SYSTEM_NAME=Android -DANDROID_ABI=$ABI -DCMAKE_ANDROID_NDK=$1 -DCMAKE_ANDROID_ARCH_ABI=$ABI -DCMAKE_ANDROID_NDK_TOOLCHAIN_VERSION=clang -DCMAKE_ANDROID_STL_TYPE=c++_static -DCMAKE_ANDROID_API=16 ../..
    cmake --build . --config Release --target install
    cd -
done
