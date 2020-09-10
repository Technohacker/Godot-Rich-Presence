# Edit to match your CPU
$threadcount = 12

# Create a build folder in discord-rpc
mkdir -Force discord-rpc\build

# Enter that folder
cd discord-rpc\build

# Run CMake
cmake ..

# Build library
msbuild DiscordRPC.sln -m:$threadcount

# Return to root
cd ..\..

# Enter the Godot CPP folder
cd godot-cpp

# Run SConstruct with generate_bindings=yes
scons platform=windows bits=64 generate_bindings=yes -j $threadcount

# Return to root
cd ..

# Build library
scons platform=windows arch=64 -j $threadcount
