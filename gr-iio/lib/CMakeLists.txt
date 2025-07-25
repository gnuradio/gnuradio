# Copyright 2021 Free Software Foundation, Inc.
# Copyright 2021 GNU Radio Contributors
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup library
########################################################################
add_library(
    gnuradio-iio
    attr_sink_impl.cc
    attr_source_impl.cc
    attr_updater_impl.cc
    dds_control_impl.cc
    device_sink_impl.cc
    device_source_impl.cc
    iio_types.cc)

target_include_directories(
    gnuradio-iio PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
                        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>)
target_link_libraries(
    gnuradio-iio
    PUBLIC gnuradio-runtime
    PRIVATE gnuradio-blocks libiio::iio Volk::volk)

if(libad9361_SUFFICIENT)
    target_sources(gnuradio-iio PRIVATE fmcomms2_sink_impl.cc fmcomms2_source_impl.cc
                                        pluto_utils.cc)
    target_link_libraries(gnuradio-iio PRIVATE libad9361::ad9361)
    target_compile_definitions(gnuradio-iio PUBLIC -DGR_IIO_LIBAD9361)
endif(libad9361_SUFFICIENT)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-iio.rc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-iio.rc @ONLY)

    target_sources(gnuradio-iio PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-iio.rc)
endif(MSVC)

if(BUILD_SHARED_LIBS)
    gr_library_foo(gnuradio-iio)
endif()
