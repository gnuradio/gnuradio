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

gr_register_component("gr-filter" ENABLE_GR_FILTER Boost_FOUND ENABLE_GNURADIO_RUNTIME
                      ENABLE_GR_FFT ENABLE_GR_BLOCKS)

set(GR_PKG_FILTER_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/filter)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_FILTER)
    ########################################################################
    # Add subdirectories
    ########################################################################
    add_subdirectory(include/gnuradio/filter)
    add_subdirectory(lib)

    # Check for scipy and pyqtgraph, but don't fail if they don't exist.
    gr_python_check_module_raw("pyqtgraph" "import pyqtgraph" PYQTGRAPH_FOUND)
    gr_python_check_module_raw("scipy" "import scipy" SCIPY_FOUND)

    if(NOT PYQTGRAPH_FOUND OR NOT SCIPY_FOUND)
        message(WARNING "PyQtGraph and Scipy are required to run the filter design tool, "
                        "but are not detected! Please make sure they are installed on "
                        "the target system.")
    endif(NOT PYQTGRAPH_FOUND OR NOT SCIPY_FOUND)

    if(ENABLE_PYTHON)
        add_subdirectory(python/filter)
        add_subdirectory(python/filter/design)
        add_subdirectory(python/filter/gui)
        add_subdirectory(apps)
        if(ENABLE_EXAMPLES)
            add_subdirectory(examples)
        endif(ENABLE_EXAMPLES)
    endif(ENABLE_PYTHON)
    if(ENABLE_GRC)
        add_subdirectory(grc)
    endif(ENABLE_GRC)
    add_subdirectory(docs)

    ########################################################################
    # Create Pkg Config File
    ########################################################################
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-filter.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-filter.pc @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-filter.pc
            DESTINATION ${GR_LIBRARY_DIR}/pkgconfig)

endif(ENABLE_GR_FILTER)
