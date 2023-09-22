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

gr_python_install(PROGRAMS gr_filter_design DESTINATION ${GR_RUNTIME_DIR})
