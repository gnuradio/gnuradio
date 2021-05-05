# Copyright 2021 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install public header files
########################################################################

install(FILES
  api.h
  attr_sink.h
  attr_source.h
  attr_updater.h
  device_sink.h
  device_source.h
  dds_control.h
  fmcomms2_sink.h
  fmcomms2_source.h
  # fmcomms5_sink.h
  # fmcomms5_source.h
  pluto_sink.h
  pluto_source.h
  DESTINATION ${GR_INCLUDE_DIR}/gnuradio/iio
)
