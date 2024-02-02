@echo off

cmake --preset Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1
cmake --build .\Build\Debug
copy Build\Debug\compile_commands.json .\
