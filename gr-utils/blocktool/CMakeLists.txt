# Copyright 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

GR_PYTHON_INSTALL(FILES
    __init__.py
    __main__.py
    cli.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/blocktool
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(core)
