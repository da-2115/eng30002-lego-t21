#!/bin/bash

echo "Building Lego sorter executable..."

cmake -S . -B build
cmake --build build

echo "Done."