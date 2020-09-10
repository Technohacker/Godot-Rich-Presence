rem Create a build folder in discord-rpc
mkdir discord-rpc\build

rem Enter that folder
cd discord-rpc\build

rem Run CMake
cmake ..

rem Build library
msbuild DiscordRPC.sln

rem Return to root
cd ..\..

rem Enter the Godot CPP folder
cd godot-cpp

rem Run SConstruct with generate_bindings=yes
scons platform=windows bits=64 generate_bindings=yes

rem Return to root
cd ..

rem Build library
scons platform=windows arch=64
