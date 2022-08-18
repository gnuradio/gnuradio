# Copyright 2021 Horden Engineering
# Author: Adam Horden <adam.horden@horden.engineering>
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

#[=======================================================================[.rst:
Findlibiio
----------

Find the native libiio includes and library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``libbio::iio``
  The iio library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``libiio_FOUND``
  True if the system has libiio.
``libiio_VERSION``
  The version of libiio that was found.
``libiio_INCLUDE_DIRS``
  Include directories needed to use libiio.
``libiio_LIBRARIES``
  Libraries needed to link to libiio.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``libiio_INCLUDE_DIR``
  The directory containing ``iio.h``.
``libiio_LIBRARY``
  The path to the iio library.

#]=======================================================================]

find_package(PkgConfig)
pkg_check_modules(PC_libiio QUIET libiio)

find_path(
    libiio_INCLUDE_DIR
    NAMES iio.h
    HINTS ${PC_libiio_INCLUDE_DIRS}
    PATHS /usr/include /usr/local/include /opt/local/include)

find_library(
    libiio_LIBRARY
    NAMES iio libiio
    HINTS ${PC_libiio_LIBRARY_DIRS}
    PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64 /opt/local/lib
          /opt/local/lib64)

# only way we have to get version is to rely on pkg-config
set(libiio_VERSION ${PC_libiio_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    libiio
    REQUIRED_VARS libiio_LIBRARY libiio_INCLUDE_DIR
    VERSION_VAR libiio_VERSION)

if(libiio_FOUND)
    set(libiio_INCLUDE_DIRS ${libiio_INCLUDE_DIR})
    set(libiio_LIBRARIES ${libiio_LIBRARY})
endif()

if(libiio_FOUND AND NOT TARGET libiio::iio)
    add_library(libiio::iio INTERFACE IMPORTED)
    set_target_properties(
        libiio::iio PROPERTIES INTERFACE_LINK_LIBRARIES "${libiio_LIBRARIES}"
                               INTERFACE_INCLUDE_DIRECTORIES "${libiio_INCLUDE_DIRS}")
endif()

mark_as_advanced(libiio_INCLUDE_DIR libiio_LIBRARY)
