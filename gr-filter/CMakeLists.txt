# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
include(GrBoost)

########################################################################
# Register component
########################################################################
include(GrComponent)

GR_REGISTER_COMPONENT("gr-filter" ENABLE_GR_FILTER
    Boost_FOUND
    ENABLE_GNURADIO_RUNTIME
    ENABLE_GR_FFT
    ENABLE_GR_BLOCKS
)

SET(GR_PKG_FILTER_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/filter)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_FILTER)
########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/filter)
add_subdirectory(lib)
add_subdirectory(python/filter)
GR_PYTHON_CHECK_MODULE_RAW(
    "pyqtgraph"
    "import pyqtgraph"
    PYQTGRAPH_FOUND
)
GR_PYTHON_CHECK_MODULE_RAW(
    "scipy"
    "import scipy"
    SCIPY_FOUND
)
GR_REGISTER_COMPONENT("gr-filterdesign" ENABLE_GR_FILTER_DESIGN
    ENABLE_PYTHON
    PYQTGRAPH_FOUND
    SCIPY_FOUND)
if(ENABLE_GR_FILTER_DESIGN)
    add_subdirectory(python/filter/design)
    add_subdirectory(python/filter/gui)
    add_subdirectory(apps)
    if(ENABLE_EXAMPLES)
        add_subdirectory(examples)
    endif(ENABLE_EXAMPLES)
endif(ENABLE_GR_FILTER_DESIGN)
if(ENABLE_GRC)
    add_subdirectory(grc)
endif(ENABLE_GRC)
add_subdirectory(docs)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-filter.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-filter.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-filter.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

endif(ENABLE_GR_FILTER)
