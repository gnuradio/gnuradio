# Copyright 2010-2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# This file included, use CMake directory variables
########################################################################
#MSVC workaround: we can't have dynamically sized arrays.
#So ifdef a max array bounds that is larger than NN and NROOTS
#Its a bit of a hack, but if you look at the code, its so full of ifdefs,
#and lacks optimization where it should be pre-allocating these arrays.
if(MSVC)
    set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/exercise.c ${CMAKE_CURRENT_SOURCE_DIR}/char.c
        PROPERTIES COMPILE_DEFINITIONS "MAX_ARRAY=256;")
endif(MSVC)

add_library(
    gr_fec_rs OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/ccsds.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ccsds_tab.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ccsds_tal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/char.c
    ${CMAKE_CURRENT_SOURCE_DIR}/decode_rs_ccsds.c
    ${CMAKE_CURRENT_SOURCE_DIR}/encode_rs_ccsds.c
    ${CMAKE_CURRENT_SOURCE_DIR}/init_rs.c)
target_include_directories(
    gr_fec_rs
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
    PRIVATE $<TARGET_PROPERTY:gnuradio-runtime,INCLUDE_DIRECTORIES>)
set_target_properties(gr_fec_rs PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(gnuradio-fec PRIVATE $<TARGET_OBJECTS:gr_fec_rs>)

########################################################################
# Register unit tests
########################################################################
if(ENABLE_TESTING)
    add_executable(
        gr_fec_rstest ${CMAKE_CURRENT_SOURCE_DIR}/exercise.c
                      ${CMAKE_CURRENT_SOURCE_DIR}/rstest.c $<TARGET_OBJECTS:gr_fec_rs>)
    target_link_libraries(gr_fec_rstest gnuradio-runtime)
    target_include_directories(gr_fec_rstest
                               PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
    add_test(test_rs gr_fec_rstest)
endif(ENABLE_TESTING)
