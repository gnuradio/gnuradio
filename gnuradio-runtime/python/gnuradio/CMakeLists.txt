# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
include(GrPython)

add_subdirectory(gr)

if(ENABLE_GR_CTRLPORT)
    add_subdirectory(ctrlport)
endif(ENABLE_GR_CTRLPORT)

gr_python_install(FILES __init__.py eng_notation.py eng_option.py eng_arg.py
                        gr_unittest.py DESTINATION ${GR_PYTHON_DIR}/gnuradio)
