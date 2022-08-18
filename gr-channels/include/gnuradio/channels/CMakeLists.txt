# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install header files
########################################################################
install(FILES api.h channel_model.h channel_model2.h fading_model.h
              selective_fading_model.h selective_fading_model2.h
        DESTINATION ${GR_INCLUDE_DIR}/gnuradio/channels)
