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

###########################
# Check for system libgsm #
###########################

INCLUDE(FindPkgConfig)
INCLUDE(FindPackageHandleStandardArgs)

pkg_check_modules(LIBGSM_PKG QUIET gsm)

find_path(LIBGSM_INCLUDE_DIR NAMES gsm.h
  PATHS
  ${LIBGSM_PKG_INCLUDE_DIRS}
  /usr/local/include/gsm
  /usr/local/include
  /usr/include/gsm
  /usr/include
  )

find_library(LIBGSM_LIBRARIES NAMES gsm
  PATHS
  ${LIBGSM_PKG_LIBRARY_DIRS}
  /usr/local/lib
  /usr/lib
  )

if(LIBGSM_INCLUDE_DIR AND LIBGSM_LIBRARIES)
  set(LIBGSM_FOUND TRUE)
  set(LIBGSM_INCLUDE_DIRS ${LIBGSM_INCLUDE_DIR})
  set(LIBGSM_LIBRARIES ${LIBGSM_LIBRARIES} ${LIBGSM_LIBRARY})
endif(LIBGSM_INCLUDE_DIR AND LIBGSM_LIBRARIES)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBGSM DEFAULT_MSG LIBGSM_LIBRARIES LIBGSM_INCLUDE_DIRS)
mark_as_advanced(LIBGSM_INCLUDE_DIR LIBGSM_LIBRARIES)
