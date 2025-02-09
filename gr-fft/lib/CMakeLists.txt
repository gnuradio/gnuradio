# Copyright 2012-2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup library
########################################################################
add_library(gnuradio-fft fft.cc fft_v_fftw.cc goertzel_fc_impl.cc goertzel.cc window.cc)

target_link_libraries(
    gnuradio-fft
    PUBLIC gnuradio-runtime Volk::volk
    PRIVATE fftw3f::fftw3f)

# Address linker issues with std::filesystem on Centos 8 and Debian
target_link_libraries(
    gnuradio-fft
    PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>>:stdc++fs>
)

target_include_directories(
    gnuradio-fft PUBLIC $<INSTALL_INTERFACE:include>
                        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)

if(ENABLE_GR_CTRLPORT)
    target_sources(gnuradio-fft PRIVATE ctrlport_probe_psd_impl.cc)
endif(ENABLE_GR_CTRLPORT)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-fft.rc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fft.rc @ONLY)

    target_sources(gnuradio-fft PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fft.rc)
endif(MSVC)

if(BUILD_SHARED_LIBS)
    gr_library_foo(gnuradio-fft)
endif()

if(ENABLE_TESTING)
    include(GrTest)

    list(APPEND test_gr_fft_sources qa_fft_shift.cc)
    list(APPEND GR_TEST_TARGET_DEPS gnuradio-fft)

    foreach(qa_file ${test_gr_fft_sources})
        gr_add_cpp_test("fft_${qa_file}" ${CMAKE_CURRENT_SOURCE_DIR}/${qa_file})
    endforeach(qa_file)
endif(ENABLE_TESTING)
