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

gr_find_package(MPLIB)

########################################################################
# Setup compatibility checks and defines
########################################################################
include(${CMAKE_CURRENT_SOURCE_DIR}/ConfigChecks.cmake)
gr_check_linux_sched_avail()

########################################################################
# Find spdlog
########################################################################
gr_find_package(spdlog)

########################################################################
# Register component
########################################################################
include(GrComponent)
gr_register_component(
    NAME "gnuradio-runtime"
    VAR ENABLE_GNURADIO_RUNTIME
    REQUIRED_TPLS Boost_FOUND
                  PYTHONINTERP_FOUND
                  MPLIB_FOUND
                  spdlog_FOUND
                  Volk_FOUND
)

get_filename_component(GNURADIO_RUNTIME_PYTHONPATH ${PROJECT_SOURCE_DIR}/python ABSOLUTE)

########################################################################
# Register controlport component
########################################################################

gr_register_component(
    NAME "gr-ctrlport"
    NO_INSTALL
    VAR ENABLE_GR_CTRLPORT
    REQUIRED_TPLS Boost_FOUND
    REQUIRED_COMPONENTS gnuradio-runtime
)

# find thrift quietly to determine whether it should be enabled by default
gr_find_package(THRIFT ${GR_THRIFT_MIN_VERSION} QUIET)

option(ENABLE_CTRLPORT_THRIFT "Enable ControlPort Thrift support" ${THRIFT_FOUND})

if(ENABLE_CTRLPORT_THRIFT)
    # ensure we have thrift, loudly, since it is enabled
    gr_find_package(THRIFT ${GR_THRIFT_MIN_VERSION} REQUIRED)
    set(THRIFT_BIN ${Thrift_INCLUDE_DIR}/../bin/thrift)
    set(THRIFT_HAS_THREADFACTORY_H TRUE)
    set_target_properties(Thrift::thrift
        PROPERTIES
        INTERFACE_COMPILE_DEFINITIONS THRIFT_HAS_THREADFACTORY_H
)
endif()

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GNURADIO_RUNTIME)

    get_filename_component(GR_RUNTIME_PYTHONPATH ${CMAKE_CURRENT_SOURCE_DIR}/python
                           ABSOLUTE)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-runtime.conf.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.conf @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.conf
            DESTINATION ${GR_PREFSDIR_relative_to_prefix})

    set(PC_ADD_CFLAGS ${MPLIB_PC_ADD_CFLAGS})

    ########################################################################
    # Add subdirectories
    ########################################################################
    add_subdirectory(include)
    add_subdirectory(lib)
    add_subdirectory(apps)
    if(ENABLE_PYTHON)
        add_subdirectory(python)
        if(ENABLE_EXAMPLES)
            add_subdirectory(examples)
        endif(ENABLE_EXAMPLES)
    endif(ENABLE_PYTHON)

    ########################################################################
    # Create Pkg Config File
    ########################################################################
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-runtime.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.pc @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.pc
            DESTINATION ${GR_LIBRARY_DIR}/pkgconfig)

endif(ENABLE_GNURADIO_RUNTIME)
