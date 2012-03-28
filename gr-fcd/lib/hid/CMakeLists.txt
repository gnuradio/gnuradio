# Copyright 2012 Free Software Foundation, Inc.
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
# This file included, use CMake directory variables
########################################################################

include_directories(APPEND ${USB_INCLUDES})

set(hid_sources)

if(WIN32)
  list(APPEND hid_sources ${CMAKE_CURRENT_SOURCE_DIR}/hidwin.c)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  list(APPEND hid_sources ${CMAKE_CURRENT_SOURCE_DIR}/hidmac.c)
else()
  list(APPEND hid_sources ${CMAKE_CURRENT_SOURCE_DIR}/hid-libusb.c)
endif()

########################################################################
# Append gnuradio-fcd library sources
########################################################################
list(APPEND gr_fcd_sources ${hid_sources})
