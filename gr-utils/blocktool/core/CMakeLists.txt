# Copyright 2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

GR_PYTHON_INSTALL(FILES
    __init__.py
    base.py
    comments.py
    parseheader.py
    parseheader_generic.py
    iosignature.py
    outputschema.py
    Constants.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/blocktool/core
)
