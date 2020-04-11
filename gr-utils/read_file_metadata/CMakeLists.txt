# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install python files and apps
########################################################################
include(GrPython)

GR_PYTHON_INSTALL(
    PROGRAMS
    gr_read_file_metadata
    DESTINATION ${GR_RUNTIME_DIR}
)
