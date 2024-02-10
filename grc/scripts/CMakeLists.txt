# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
configure_file(
    gnuradio-companion
    ${CMAKE_BINARY_DIR}/gnuradio-companion
    @ONLY
)
configure_file(
    grcc
    ${CMAKE_BINARY_DIR}/grcc
    @ONLY
)

GR_PYTHON_INSTALL(
    PROGRAMS ${CMAKE_BINARY_DIR}/gnuradio-companion ${CMAKE_BINARY_DIR}/grcc
    DESTINATION ${GR_RUNTIME_DIR}
)
if(WIN32)
    gen_py_launcher(
        MODULES gnuradio-companion grcc
        PYTHON ${GR_BUNDLE_PYTHON}
    )
elseif(APPLE)
    gen_py_program_launcher(
        PROGRAMS gnuradio-companion grcc
    )
endif()
add_subdirectory(freedesktop)
