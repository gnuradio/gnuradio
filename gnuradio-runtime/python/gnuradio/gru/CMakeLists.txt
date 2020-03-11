# Copyright 2010-2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

GR_PYTHON_INSTALL(FILES
    __init__.py
    freqz.py
    gnuplot_freqz.py
    hexint.py
    listmisc.py
    mathmisc.py
    msgq_runner.py
    os_read_exactly.py
    seq_with_cursor.py
    socket_stuff.py
    daemon.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/gru
)
