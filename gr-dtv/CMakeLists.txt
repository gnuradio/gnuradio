# Copyright 2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################

########################################################################
# Register component
########################################################################
include(GrComponent)

gr_register_component("gr-dtv" ENABLE_GR_DTV ENABLE_GNURADIO_RUNTIME ENABLE_GR_ANALOG
                      ENABLE_GR_FILTER ENABLE_GR_FEC)

set(GR_PKG_DTV_EXAMPLES_DIR ${GR_PKG_DATA_DIR}/examples/dtv)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GR_DTV)

    ########################################################################
    # Add subdirectories
    ########################################################################
    add_subdirectory(include/gnuradio/dtv)
    add_subdirectory(lib)
    if(ENABLE_PYTHON)
        add_subdirectory(python/dtv)
        if(ENABLE_EXAMPLES)
            add_subdirectory(examples)
        endif(ENABLE_EXAMPLES)
        add_subdirectory(apps)
    endif(ENABLE_PYTHON)
    if(ENABLE_GRC)
        add_subdirectory(grc)
    endif(ENABLE_GRC)
    add_subdirectory(docs)

    ########################################################################
    # Create Pkg Config File
    ########################################################################
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-dtv.pc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-dtv.pc @ONLY)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-dtv.pc
            DESTINATION ${GR_LIBRARY_DIR}/pkgconfig)

endif(ENABLE_GR_DTV)
