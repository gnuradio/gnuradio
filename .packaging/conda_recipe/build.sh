#!/usr/bin/env bash

if [[ $target_platform == osx* ]] ; then
    CXXFLAGS="${CXXFLAGS} -D_LIBCPP_DISABLE_AVAILABILITY"
fi

# Workaround for no std::aligned_alloc with osx-64
# https://github.com/chriskohlhoff/asio/issues/1090
# Maybe remove when boost is updated to 1.80.0?
if [[ "${target_platform}" == "osx-64" ]]; then
  export CXXFLAGS="-DBOOST_ASIO_DISABLE_STD_ALIGNED_ALLOC ${CXXFLAGS}"
fi

cmake -E make_directory build
cd build

cmake_config_args=(
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_PREFIX_PATH=$PREFIX
    -DCMAKE_INSTALL_PREFIX=$PREFIX
    -DLIB_SUFFIX=""
    -DPYTHON_EXECUTABLE=$PYTHON
    -DGR_PYTHON_DIR=$SP_DIR
)

cmake ${CMAKE_ARGS} -G "Ninja" .. "${cmake_config_args[@]}"
cmake --build . --config Release -- -j${CPU_COUNT}

cmake --build . --config Release --target install

if [[ $target_platform == linux* ]] ; then
    export QT_QPA_PLATFORM=offscreen
    SKIP_TESTS=(
    )
else
    SKIP_TESTS=(
    )
fi
SKIP_TESTS_STR=$( IFS="|"; echo "^(${SKIP_TESTS[*]})$" )

ctest --build-config Release --output-on-failure --timeout 120 -j${CPU_COUNT} -E "$SKIP_TESTS_STR"

# now run the skipped tests to see the failures, but don't error out
ctest --build-config Release --output-on-failure --timeout 120 -j${CPU_COUNT} -R "$SKIP_TESTS_STR" || exit 0
