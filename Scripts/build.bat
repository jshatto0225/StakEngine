@echo off

cmake --preset Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1
cmake --build .\Build\
copy Build\compile_commands.json .\
