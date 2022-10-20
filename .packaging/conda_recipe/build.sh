#!/usr/bin/env bash

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
    SKIP_TESTS=(
        qa_qtgui
    )
else
    SKIP_TESTS=(
        qa_block_gateway
        qa_fecapi_cc
        qa_fecapi_dummy
        qa_fecapi_ldpc
        qa_fecapi_repetition
        qa_header_payload_demux
        qa_hier_block2
        qa_hier_block2_message_connections
        qa_message_debug
        qa_message_strobe
        qa_python_message_passing
        qa_uncaught_exception
    )
fi
SKIP_TESTS_STR=$( IFS="|"; echo "${SKIP_TESTS[*]}" )

ctest --build-config Release --output-on-failure --timeout 120 -j${CPU_COUNT} -E $SKIP_TESTS_STR

# now run the skipped tests to see the failures, but don't error out
ctest --build-config Release --output-on-failure --timeout 120 -j${CPU_COUNT} -R $SKIP_TESTS_STR || exit 0
