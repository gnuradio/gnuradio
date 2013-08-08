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
# Setup the include and linker paths
########################################################################
include_directories(
    ${GR_COMEDI_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${LOG4CXX_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${COMEDI_INCLUDE_DIRS}
)

link_directories(
  ${LOG4CXX_LIBRARY_DIRS}
    ${Boost_LIBRARY_DIRS}
    ${COMEDI_LIBRARY_DIRS}
)

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
  include_directories(${ICE_INCLUDE_DIR})
endif(ENABLE_GR_CTRLPORT)

########################################################################
# Setup library
########################################################################
list(APPEND comedi_sources
    sink_s_impl.cc
    source_s_impl.cc
)

list(APPEND comedi_libs
    gnuradio-runtime
    ${Boost_LIBRARIES}
    ${COMEDI_LIBRARIES}
)

add_library(gnuradio-comedi SHARED ${comedi_sources})
target_link_libraries(gnuradio-comedi ${comedi_libs})
GR_LIBRARY_FOO(gnuradio-comedi RUNTIME_COMPONENT "comedi_runtime" DEVEL_COMPONENT "comedi_devel")
