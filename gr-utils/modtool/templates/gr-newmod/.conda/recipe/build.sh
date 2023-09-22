#!/usr/bin/env bash

set -ex

cmake -E make_directory buildconda
cd buildconda

cmake_config_args=(
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_INSTALL_PREFIX=$PREFIX
    -DLIB_SUFFIX=""
    -DENABLE_DOXYGEN=OFF
    -DENABLE_TESTING=ON
)

cmake ${CMAKE_ARGS} -G "Ninja" .. "${cmake_config_args[@]}"
cmake --build . --config Release -- -j${CPU_COUNT}
cmake --build . --config Release --target install

if [[ "${CONDA_BUILD_CROSS_COMPILATION:-}" != "1" || "${CROSSCOMPILING_EMULATOR}" != "" ]]; then
    ctest --build-config Release --output-on-failure --timeout 120 -j${CPU_COUNT}
fi
