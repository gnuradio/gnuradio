# Copyright 2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

########################################################################
# Install some dtv apps
########################################################################
GR_PYTHON_INSTALL(
    PROGRAMS
    DESTINATION ${GR_RUNTIME_DIR}
)
