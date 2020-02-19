# Copyright 2012 Free Software Foundation, Inc.
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
    squash_ff.h
    wavelet_ff.h
    wvps_ff.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/wavelet
)

