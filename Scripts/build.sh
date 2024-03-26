#!/bin/bash

cmake --preset Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1
cmake --build Build

cp Build/compile_commands.json .
