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
    ${Boost_INCLUDE_DIRS}
    ${COMEDI_INCLUDE_DIRS}
)

link_directories(
    ${Boost_LIBRARY_DIRS}
    ${COMEDI_LIBRARY_DIRS}
)

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
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

if(ENABLE_STATIC_LIBS)
  if(ENABLE_GR_CTRLPORT)
    # Remove GR_CTRLPORT set this target's definitions.
    # Makes sure we don't try to use ControlPort stuff in source files
    GET_DIRECTORY_PROPERTY(STATIC_DEFS COMPILE_DEFINITIONS)
    list(REMOVE_ITEM STATIC_DEFS "GR_CTRLPORT")
    SET_PROPERTY(DIRECTORY PROPERTY COMPILE_DEFINITIONS "${STATIC_DEFS}")

    # readd it to the target since we removed it from the directory-wide list.
    SET_PROPERTY(TARGET gnuradio-comedi APPEND PROPERTY COMPILE_DEFINITIONS "GR_CTRLPORT")
  endif(ENABLE_GR_CTRLPORT)

  add_library(gnuradio-comedi_static STATIC ${comedi_sources})

  if(NOT WIN32)
    set_target_properties(gnuradio-comedi_static
      PROPERTIES OUTPUT_NAME gnuradio-comedi)
  endif(NOT WIN32)

  install(TARGETS gnuradio-comedi_static
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT "comedi_devel"   # .lib file
    )
endif(ENABLE_STATIC_LIBS)
