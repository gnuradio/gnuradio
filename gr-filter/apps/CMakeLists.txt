# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install python files and apps
########################################################################
include(GrPython)

configure_file(gr_filter_design ${CMAKE_BINARY_DIR}/gr_filter_design @ONLY)

if(WIN32)
gen_py_launcher(
    MODULES gr_filter_design
    PYTHON ${GR_BUNDLE_PYTHON}
)
elseif(APPLE)
    gen_py_program_launcher(
        PROGRAMS gr_filter_design
    )
endif()
gr_python_install(PROGRAMS ${CMAKE_BINARY_DIR}/gr_filter_design DESTINATION ${GR_RUNTIME_DIR})
