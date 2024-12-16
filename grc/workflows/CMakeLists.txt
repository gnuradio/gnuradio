# Copyright 2024 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
include(GrPython)

file(GLOB_RECURSE yml_files "*.workflow.yml")

install(
    FILES ${yml_files}
    DESTINATION ${GRC_BLOCKS_DIR}
)
