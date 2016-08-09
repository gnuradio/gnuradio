# Copyright 2016 Free Software Foundation, Inc.
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

##############################
# Check for system libcodec2 #
##############################

INCLUDE(FindPkgConfig)
INCLUDE(FindPackageHandleStandardArgs)

pkg_check_modules(LIBCODEC2_PKG QUIET codec2)

find_path(LIBCODEC2_INCLUDE_DIR NAMES codec2.h
  PATHS
  ${LIBCODEC2_PKG_INCLUDE_DIRS}
  /usr/include/codec2
  /usr/include
  /usr/local/include/codec2
  /usr/local/include
  )

find_library(LIBCODEC2_LIBRARIES NAMES codec2
  PATHS
  ${LIBCODEC2_PKG_LIBRARY_DIRS}
  /usr/lib
  /usr/local/lib
  )

if(LIBCODEC2_INCLUDE_DIR AND LIBCODEC2_LIBRARIES)
  set(LIBCODEC2_FOUND TRUE)
  set(LIBCODEC2_INCLUDE_DIRS ${LIBCODEC2_INCLUDE_DIR})
  set(LIBCODEC2_LIBRARIES ${LIBCODEC2_LIBRARIES} ${LIBCODEC2_LIBRARY})
  file(READ "${LIBCODEC2_INCLUDE_DIR}/codec2.h" _CODEC2_H_CONTENTS)
  string(REGEX MATCHALL "CODEC2_MODE[_a-z0-9A-Z]+" CODEC2_MODES "${_CODEC2_H_CONTENTS}")
  foreach(mode ${CODEC2_MODES})
    set(LIBCODEC2_HAS_${mode} True)
  endforeach()
  if(EXISTS "${LIBCODEC2_INCLUDE_DIR}/freedv_api.h")
    set(LIBCODEC2_HAS_FREEDV_API True)
    file(READ "${LIBCODEC2_INCLUDE_DIR}/freedv_api.h" _FREEDV_API_H_CONTENTS)
    string(REGEX MATCHALL "FREEDV_MODE[_a-z0-9A-Z]+" FREEDV_MODES "${_FREEDV_API_H_CONTENTS}")
    foreach(mode ${FREEDV_MODES})
      set(LIBCODEC2_HAS_${mode} True)
    endforeach()
  else()
    set(LIBCODEC2_HAS_FREEDV_API false)
  endif()
endif(LIBCODEC2_INCLUDE_DIR AND LIBCODEC2_LIBRARIES)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBCODEC2 DEFAULT_MSG LIBCODEC2_LIBRARIES LIBCODEC2_INCLUDE_DIRS)
mark_as_advanced(LIBCODEC2_INCLUDE_DIR LIBCODEC2_LIBRARIES)
