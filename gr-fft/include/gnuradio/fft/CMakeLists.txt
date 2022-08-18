# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install header files
########################################################################
install(
    FILES api.h
          fft.h
          fft_shift.h
          fft_v.h
          goertzel.h
          goertzel_fc.h
          window.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/fft)

if(ENABLE_GR_CTRLPORT)
    install(FILES ctrlport_probe_psd.h DESTINATION ${GR_INCLUDE_DIR}/gnuradio/fft)
endif(ENABLE_GR_CTRLPORT)
