# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

file(GLOB yml_files "*.yml")
install(FILES ${yml_files} DESTINATION ${GRC_BLOCKS_DIR})
