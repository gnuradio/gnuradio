# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Install public header files
########################################################################
install(FILES
  api.h
  device_source.h device_sink.h
  fmcomms2_source.h fmcomms2_sink.h
  fmcomms5_source.h fmcomms5_sink.h
  pluto_source.h pluto_sink.h
  attr_source.h attr_sink.h
  attr_updater.h
  dds_control.h
  DESTINATION ${GR_INCLUDE_DIR}/gnuradio/iio
)
