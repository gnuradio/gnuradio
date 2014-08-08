# Copyright 2013 Free Software Foundation, Inc.
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

if(ENABLE_GR_CTRLPORT)

# Add definition so we can compile in ControlPort to the blocks.
ADD_DEFINITIONS(-DGR_CTRLPORT)

list(APPEND gnuradio_ctrlport_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/rpcmanager.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/rpcserver_aggregator.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/rpcserver_booter_aggregator.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/rpcserver_selector.cc
)

########################################################################
# Add controlport stuff to gnuradio-runtime
########################################################################

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Add any required libraries here
#list(APPEND gnuradio_runtime_libs
#)

endif(ENABLE_GR_CTRLPORT)
