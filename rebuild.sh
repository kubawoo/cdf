#!/bin/bash -e

rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build

rm -rf ~/.local/lib/libcdf.so ~/.local/lib/cmake/cdf ~/.local/include/cdf

cmake --install build --prefix ~/.local/

