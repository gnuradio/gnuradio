# Copyright 2021 Horden Engineering
# Author: Adam Horden <adam.horden@horden.engineering>
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

#[=======================================================================[.rst:
Findlibad9361
-------------

Find the libad9361-iio includes and library.

Imported Targets
^^^^^^^^^^^^^^^^

This module provides the following imported targets, if found:

``libad9361::ad9361``
  The ad9361 library

Result Variables
^^^^^^^^^^^^^^^^

This will define the following variables:

``libad9361_FOUND``
  True if the system has libad9361.
``libad9361_VERSION``
  The version of libad9361 that was found.
``libad9361_INCLUDE_DIRS``
  Include directories needed to use libad9361.
``libad9361_LIBRARIES``
  Libraries needed to link to libad9361.

Cache Variables
^^^^^^^^^^^^^^^

The following cache variables may also be set:

``libad9361_INCLUDE_DIR``
  The directory containing ``ad9361.h``.
``libad9361_LIBRARY``
  The path to the ad9361 library.

#]=======================================================================]

find_package(PkgConfig)
pkg_check_modules(PC_libad9361 QUIET libad9361)

find_path(
    libad9361_INCLUDE_DIR
    NAMES ad9361.h
    HINTS ${PC_libad9361_INCLUDE_DIRS}
    PATHS /usr/include /usr/local/include /opt/local/include)

find_library(
    libad9361_LIBRARY
    NAMES ad9361 libad9361
    HINTS ${PC_libad9361_LIBRARY_DIRS}
    PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64 /opt/local/lib
          /opt/local/lib64)

# only way we have to get version is to rely on pkg-config
set(libad9361_VERSION ${PC_libad9361_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    libad9361
    REQUIRED_VARS libad9361_LIBRARY libad9361_INCLUDE_DIR
    VERSION_VAR libad9361_VERSION)

if(libad9361_FOUND)
    set(libad9361_INCLUDE_DIRS ${libad9361_INCLUDE_DIR})
    set(libad9361_LIBRARIES ${libad9361_LIBRARY})
endif()

if(libad9361_FOUND AND NOT TARGET libad9361::ad9361)
    add_library(libad9361::ad9361 INTERFACE IMPORTED)
    set_target_properties(
        libad9361::ad9361
        PROPERTIES INTERFACE_LINK_LIBRARIES "${libad9361_LIBRARIES}"
                   INTERFACE_INCLUDE_DIRECTORIES "${libad9361_INCLUDE_DIRS}")
endif()

mark_as_advanced(libad9361_INCLUDE_DIR libad9361_LIBRARY)
