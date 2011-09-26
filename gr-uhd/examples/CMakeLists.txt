# Copyright 2011 Free Software Foundation, Inc.
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
INCLUDE_DIRECTORIES(
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${GR_UHD_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

INCLUDE_DIRECTORIES(${UHD_INCLUDE_DIRS})
LINK_DIRECTORIES(${UHD_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

########################################################################
# Build executable
########################################################################
ADD_EXECUTABLE(tags_demo tags_demo.cc)
TARGET_LINK_LIBRARIES(tags_demo gnuradio-uhd)
