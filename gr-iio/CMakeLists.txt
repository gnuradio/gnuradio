# Copyright 2021 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
find_package(libiio)
find_package(libad9361)

set(GR_IIO_DEPENDENCIES ENABLE_GNURADIO_RUNTIME ENABLE_GR_BLOCKS libiio_FOUND)

########################################################################
# Register component
########################################################################
include(GrComponent)
gr_register_component("gr-iio" ENABLE_GR_IIO ${GR_IIO_DEPENDENCIES})

if(libad9361_FOUND)
    # Sadly, libad9361 is not reliably versioned (yet?).
    # Especially Fedora ships an incompatible early version of the ad9361.h header
    # Let's check whether things compile.
    set(OLD_CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES})
    set(OLD_CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
    set(CMAKE_REQUIRED_INCLUDES ${libad9361_INCLUDE_DIRS})
    set(CMAKE_REQUIRED_LIBRARIES ${libad9361_LIBRARIES})
    check_cxx_source_compiles(
        "
    #include <ad9361.h>
    namespace testing
    {
      auto test_ad9361_multichip_sync = &::ad9361_multichip_sync;
      auto test_ad9361_set_bb_rate_custom_filter_manual = &::ad9361_set_bb_rate_custom_filter_manual;
    };
    int main() {}"
        libad9361_SUFFICIENT)
    if(libad9361_SUFFICIENT)
        gr_append_subcomponent("libad9361")
    endif()
    set(CMAKE_REQUIRED_INCLUDES ${OLD_CMAKE_REQUIRED_INCLUDES})
    set(CMAKE_REQUIRED_LIBRARIES ${OLD_CMAKE_REQUIRED_LIBRARIES})
endif()

set(GR_PKG_IIO_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/iio)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_IIO)

    ########################################################################
    # Add subdirectories
    ########################################################################
    add_subdirectory(include/gnuradio/iio)
    add_subdirectory(lib)
    if(ENABLE_PYTHON)
        add_subdirectory(python/iio)
        add_subdirectory(examples)
    endif(ENABLE_PYTHON)
    if(ENABLE_GRC)
        add_subdirectory(grc)
    endif(ENABLE_GRC)

    ########################################################################
    # Create Pkg Config File
    ########################################################################
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-iio.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-iio.pc @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-iio.pc
            DESTINATION ${GR_LIBRARY_DIR}/pkgconfig)

endif(ENABLE_GR_IIO)
