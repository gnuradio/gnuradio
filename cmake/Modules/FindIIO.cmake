# Copyright 2021 Horden Engineering
# Author: Adam Horden <adam.horden@horden.engineering>
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

# - Find libiio
# Find the native libiio includes and library
#
# LIBIIO_FOUND       - True if libiio is found.
# LIBIIO_INCLUDE_DIR - Where to find iio.h.
# LIBIIO_LIBRARIES   - List of libraries when using libiio.

set(LIBIIO_NAMES iio)
find_library(LIBIIO_LIBRARY
  NAMES ${LIBIIO_NAMES}
  PATHS /usr/lib
        /usr/lib64
        /usr/local/lib
        /usr/local/lib64
        /opt/local/lib
        /opt/local/lib64
)

find_path(LIBIIO_INCLUDE_DIR iio.h
  /usr/include
  /usr/local/include
  /opt/local/include
)

if (LIBIIO_INCLUDE_DIR AND LIBIIO_LIBRARY)
  set(LIBIIO_FOUND TRUE)
  set(LIBIIO_INCLUDE_DIRS ${LIBIIO_INCLUDE_DIR})
  set(LIBIIO_LIBRARIES ${LIBIIO_LIBRARY})
else ()
  set(LIBIIO_FOUND FALSE)
  set(LIBIIO_INCLUDE_DIR "")
  set(LIBIIO_INCLUDE_DIRS "")
  set(LIBIIO_LIBRARY "")
  set(LIBIIO_LIBRARIES "")
endif ()

if (LIBIIO_FOUND)
  message(STATUS "Found libiio library: ${LIBIIO_LIBRARIES}")
endif ()

mark_as_advanced(
  LIBIIO_INCLUDE_DIRS
  LIBIIO_LIBRARIES
)
