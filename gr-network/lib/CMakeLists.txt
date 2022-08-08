#Copyright 2020-2021 Free Software Foundation, Inc.
#
#This file is a part of gnuradio
#
#SPDX - License - Identifier : GPL - 3.0 - or -later
#

########################################################################
# Setup compatibility checks and defines
########################################################################
include(GrMiscUtils)
include(CheckCXXSourceCompiles)
GR_CHECK_HDR_N_DEF(io.h HAVE_IO_H)
CHECK_INCLUDE_FILE_CXX(windows.h HAVE_WINDOWS_H)

########################################################################
#Setup library
########################################################################
add_library(gnuradio-network
    socket_pdu_impl.cc
    stream_pdu_base.cc
    tcp_connection.cc
    tcp_sink_impl.cc
    tuntap_pdu_impl.cc
    udp_sink_impl.cc
    udp_source_impl.cc
)

target_link_libraries(gnuradio-network PUBLIC gnuradio-runtime)

if(ENABLE_COMMON_PCH)
  target_link_libraries(gnuradio-network PRIVATE common-precompiled-headers)
endif()

target_include_directories(gnuradio-network
    PUBLIC $<INSTALL_INTERFACE:include>
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
)

if(HAVE_WINDOWS_H)
    target_compile_definitions(gnuradio-network PRIVATE -DHAVE_WINDOWS_H)
    target_link_libraries(gnuradio-network PRIVATE ws2_32 wsock32)
endif()

#Add Windows DLL resource file if using MSVC
if (MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-network.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-network.rc
    @ONLY)

    target_sources(gnuradio-network PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-network.rc
    )
endif (MSVC)

########################################################################
#Install built library files
########################################################################
if(BUILD_SHARED_LIBS)
    GR_LIBRARY_FOO(gnuradio-network)
endif()

########################################################################
#Build and register unit test
########################################################################
if(ENABLE_TESTING)
    include(GrTest)

    list(APPEND test_network_sources
    )
    list(APPEND GR_TEST_TARGET_DEPS gnuradio-network)

    foreach (qa_file ${test_network_sources})
        GR_ADD_CPP_TEST("network_${qa_file}"
            ${CMAKE_CURRENT_SOURCE_DIR}/${qa_file}
        )
    endforeach(qa_file)

endif(ENABLE_TESTING)
