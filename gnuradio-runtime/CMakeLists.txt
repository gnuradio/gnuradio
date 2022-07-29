# Copyright 2013,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
include(GrBoost)
include(GrPython)

find_package(MPLIB)

########################################################################
# Setup compatibility checks and defines
########################################################################
include(${CMAKE_CURRENT_SOURCE_DIR}/ConfigChecks.cmake)
GR_CHECK_LINUX_SCHED_AVAIL()

########################################################################
# Find spdlog
########################################################################
find_package(spdlog CONFIG)

########################################################################
# Register component
########################################################################
include(GrComponent)
if (ENABLE_PYTHON)
GR_REGISTER_COMPONENT("gnuradio-runtime" ENABLE_GNURADIO_RUNTIME
    Boost_FOUND
    PYTHONINTERP_FOUND
    MPLIB_FOUND
    spdlog_FOUND
    pybind11_FOUND
    Volk_FOUND
)
else()
GR_REGISTER_COMPONENT("gnuradio-runtime" ENABLE_GNURADIO_RUNTIME
    Boost_FOUND
    PYTHONINTERP_FOUND
    MPLIB_FOUND
    spdlog_FOUND
    Volk_FOUND
)
endif()

get_filename_component(GNURADIO_RUNTIME_PYTHONPATH
    ${PROJECT_SOURCE_DIR}/python ABSOLUTE
)
########################################################################
# Setup precompiled header pseudo-component
########################################################################
if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.16.0")
  set(GR_CMAKE_HAS_PCH ${CMAKE_VERSION})
endif()

GR_REGISTER_COMPONENT("common-precompiled-headers" ENABLE_COMMON_PCH
  ENABLE_GNURADIO_RUNTIME
  GR_CMAKE_HAS_PCH
  )

if(ENABLE_COMMON_PCH)
  add_library(common-precompiled-headers
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/dummy.cc
    )
  target_include_directories(common-precompiled-headers
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    )

  if(MSVC)
    target_compile_definitions(common-precompiled-headers PUBLIC
      # disables stupidity and enables std::min and std::max
      -DNOMINMAX
    )
  endif(MSVC)

  target_precompile_headers(
    common-precompiled-headers
    PUBLIC
    $<$<COMPILE_LANGUAGE:CXX>:$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/gnuradio/top_block.h>>
    )
  target_link_libraries(common-precompiled-headers PUBLIC spdlog::spdlog)
endif()

########################################################################
# Register controlport component
########################################################################

GR_REGISTER_COMPONENT("gr-ctrlport" ENABLE_GR_CTRLPORT
  Boost_FOUND
  ENABLE_GNURADIO_RUNTIME
)

# find thrift quietly to determine whether it should be enabled by default
find_package(THRIFT ${GR_THRIFT_MIN_VERSION} QUIET)
option(ENABLE_CTRLPORT_THRIFT "Enable ControlPort Thrift support" ${THRIFT_FOUND})

if(ENABLE_CTRLPORT_THRIFT)
  # ensure we have thrift, loudly, since it is enabled
  find_package(THRIFT ${GR_THRIFT_MIN_VERSION} REQUIRED)
endif()

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GNURADIO_RUNTIME)

get_filename_component(GR_RUNTIME_PYTHONPATH
    ${CMAKE_CURRENT_SOURCE_DIR}/python ABSOLUTE
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-runtime.conf.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.conf
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.conf
    DESTINATION ${GR_PREFSDIR}
)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_log_default.conf
    DESTINATION ${GR_PREFSDIR}
)

set(PC_ADD_CFLAGS ${MPLIB_PC_ADD_CFLAGS})

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(apps)
if(ENABLE_PYTHON)
     add_subdirectory(python)
    if (ENABLE_EXAMPLES)
        add_subdirectory(examples)
    endif (ENABLE_EXAMPLES)
endif(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-runtime.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

endif(ENABLE_GNURADIO_RUNTIME)
