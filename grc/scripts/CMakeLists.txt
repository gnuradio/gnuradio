# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
gr_python_install(PROGRAMS gnuradio-companion grcc DESTINATION ${GR_RUNTIME_DIR})

add_subdirectory(freedesktop)
