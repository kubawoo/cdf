#!/bin/bash -e

cmake --build build
ctest --test-dir build


cmake --install build --prefix ~/.local/

