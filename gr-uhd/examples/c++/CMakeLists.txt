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
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GR_UHD_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${UHD_INCLUDE_DIRS}
)

link_directories(${UHD_LIBRARY_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

########################################################################
# Build executable
########################################################################
add_executable(tags_demo tags_demo.cc)
target_link_libraries(tags_demo gnuradio-uhd ${CMAKE_THREAD_LIBS_INIT})

INSTALL(TARGETS
    tags_demo
    DESTINATION ${GR_PKG_UHD_EXAMPLES_DIR}
    COMPONENT "uhd_examples"
)
