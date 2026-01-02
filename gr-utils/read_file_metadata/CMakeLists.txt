# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install python files and apps
########################################################################
include(GrPython)

configure_file(gr_read_file_metadata ${CMAKE_BINARY_DIR}/gr_read_file_metadata @ONLY)
if(WIN32)
    gen_py_launcher(
        MODULES gr_read_file_metadata
        PYTHON ${GR_BUNDLE_PYTHON}
    )
elseif(APPLE)
gen_py_program_launcher(
    PROGRAMS gr_read_file_metadata
)

endif()
GR_PYTHON_INSTALL(
    PROGRAMS
    ${CMAKE_BINARY_DIR}/gr_read_file_metadata
    DESTINATION ${GR_RUNTIME_DIR}
)
