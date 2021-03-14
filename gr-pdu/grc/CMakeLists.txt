# Copyright 2021 Jacob Gilbert
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
file(GLOB yml_files "*.yml")

install(FILES ${yml_files} DESTINATION ${GRC_BLOCKS_DIR})
