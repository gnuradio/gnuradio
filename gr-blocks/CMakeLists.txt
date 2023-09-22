# Copyright 2012,2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
include(GrBoost)

find_package(SNDFILE)

if(NOT SNDFILE_FOUND)
    message(STATUS "")
    message(STATUS "********************* WARNING!!! *********************")
    message(STATUS "libsndfile1-dev is required to compile wavfile blocks.")
    message(STATUS "Wav File Sink and Source blocks will not be available.")
    message(STATUS "******************************************************")
endif()

########################################################################
# Register component
########################################################################
include(GrComponent)
gr_register_component("gr-blocks" ENABLE_GR_BLOCKS Boost_FOUND ENABLE_GNURADIO_RUNTIME)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_BLOCKS)

    ########################################################################
    # Add subdirectories
    ########################################################################
    add_subdirectory(include/gnuradio/blocks)
    add_subdirectory(lib)
    if(ENABLE_TESTING)
     add_subdirectory(tests)
    endif(ENABLE_TESTING)
    if(ENABLE_PYTHON)
        add_subdirectory(python/blocks)
        add_subdirectory(docs)
        if(ENABLE_EXAMPLES)
            add_subdirectory(examples)
        endif(ENABLE_EXAMPLES)
    endif(ENABLE_PYTHON)
    if(ENABLE_GRC)
        add_subdirectory(grc)
    endif(ENABLE_GRC)

    ########################################################################
    # Create Pkg Config File
    ########################################################################
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-blocks.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-blocks.pc @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-blocks.pc
            DESTINATION ${GR_LIBRARY_DIR}/pkgconfig)

endif(ENABLE_GR_BLOCKS)
