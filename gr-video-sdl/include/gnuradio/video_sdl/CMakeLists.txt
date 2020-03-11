# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install header files
########################################################################
install(FILES
    api.h
    sink_s.h
    sink_uc.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/video_sdl
)

