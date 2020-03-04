#Copyright 2020 Free Software Foundation, Inc.
#
#This file is a part of gnuradio
#
#SPDX - License - Identifier : GPL - 3.0 - or -later
#

########################################################################
#Setup library
########################################################################
include(GrPlatform) 

#define LIB_SUFFIX

list(APPEND network_sources 
    tcp_sink_impl.cc
    udp_sink_impl.cc
    udp_source_impl.cc
)

set(network_sources "${network_sources}" PARENT_SCOPE) 
if (NOT network_sources) 
    MESSAGE(STATUS "No C++ sources... skipping lib/")
    return ()
endif(NOT network_sources)

add_library(gnuradio-network SHARED ${network_sources})
target_link_libraries(gnuradio-network PUBLIC gnuradio-runtime)
target_include_directories(gnuradio-network
    PUBLIC $<INSTALL_INTERFACE:include>
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include> 
) 

set_target_properties(gnuradio-network PROPERTIES DEFINE_SYMBOL "gnuradio_network_EXPORTS")

if (APPLE) 
    set_target_properties(gnuradio-network PROPERTIES 
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    ) 
endif(APPLE)

########################################################################
#Install built library files
########################################################################
include(GrMiscUtils)
GR_LIBRARY_FOO(gnuradio-network)

########################################################################
#Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")

########################################################################
#Build and register unit test
########################################################################
include(GrTest)

#If your unit tests require special include paths, add them here
#include_directories()
#List all files that contain Boost.UTF unit tests here
list(APPEND test_network_sources)
#Anything we need to link to for the unit tests go here
list(APPEND GR_TEST_TARGET_DEPS gnuradio-network)

if (NOT test_network_sources)
    MESSAGE(STATUS "No C++ unit tests... skipping") 
    return ()
endif(NOT test_network_sources)

foreach (qa_file ${test_network_sources})
    GR_ADD_CPP_TEST("network_${qa_file}" 
        ${CMAKE_CURRENT_SOURCE_DIR }/${ qa_file }
    ) 
endforeach(qa_file)
