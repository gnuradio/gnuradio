# Copyright 2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

GR_PYTHON_INSTALL(FILES
    __init__.py
    add.py
    base.py
    bind.py
    disable.py
    info.py
    makeyaml.py
    newmod.py
    rm.py
    rename.py
    update.py
    help.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/modtool/cli
)
