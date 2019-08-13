# Copyright 2021 Horden Engineering
# Author: Adam Horden <adam.horden@horden.engineering>
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

# - Find libad9361
# Find the native libad9361 includes and library
#
# LIBAD9361_FOUND       - True if libad9361 is found.
# LIBAD9361_INCLUDE_DIR - Where to find ad9361.h.
# LIBAD9361_LIBRARIES   - List of libraries when using libad9361.

set(LIBAD9361_NAMES ad9361)
find_library(LIBAD9361_LIBRARY
  NAMES ${LIBAD9361_NAMES}
  PATHS /usr/lib
        /usr/lib64
        /usr/local/lib
        /usr/local/lib64
        /opt/local/lib
        /opt/local/lib64
)

find_path(LIBAD9361_INCLUDE_DIR ad9361.h
  /usr/include
  /usr/local/include
  /opt/local/include
)

if (LIBAD9361_INCLUDE_DIR AND LIBAD9361_LIBRARY)
  set(LIBAD9361_FOUND TRUE)
  set(LIBAD9361_INCLUDE_DIRS ${LIBAD9361_INCLUDE_DIR})
  set(LIBAD9361_LIBRARIES ${LIBAD9361_LIBRARY})
else ()
  set(LIBAD9361_FOUND FALSE)
  set(LIBAD9361_INCLUDE_DIR "")
  set(LIBAD9361_INCLUDE_DIRS "")
  set(LIBAD9361_LIBRARY "")
  set(LIBAD9361_LIBRARIES "")
endif ()

if (LIBAD9361_FOUND)
  message(STATUS "Found libad9361 library: ${LIBAD9361_LIBRARIES}")
endif ()

mark_as_advanced(
  LIBAD9361_INCLUDE_DIRS
  LIBAD9361_LIBRARIES
)
