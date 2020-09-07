#!/bin/bash

PROCESSES=$(nproc --all)

# Create a build folder in discord-rpc
mkdir -p discord-rpc/build
# Enter that folder
cd discord-rpc/build
# Run CMake
cmake ..
# Build library
make -j$PROCESSES
# Return to root
cd ../..

# Enter the Godot CPP folder
cd godot-cpp
# Run SConstruct with generate_bindings=yes
scons p=linux a=64 generate_bindings=yes -j$PROCESSES
# Return to root
cd ..

# Build library
scons p=linux a=64 -j$PROCESSES
