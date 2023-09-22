# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

GR_PYTHON_INSTALL(FILES
    __init__.py
    cmakefile_editor.py
    cppfile_editor.py
    code_generator.py
    grc_yaml_generator.py
    parser_cc_block.py
    scm.py
    util_functions.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/modtool/tools
)
