@echo off

mkdir build
cd build
cmake .. -GNinja -DCMAKE_PREFIX_PATH=C:\dev\gnu-radio\cmake\conan\build\generators -DCMAKE_INSTALL_PREFIX=C:\dev\gnu-radio\deps
