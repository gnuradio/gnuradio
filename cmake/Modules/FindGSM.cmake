# Copyright 2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

###########################
# Check for system libgsm #
###########################

find_package(PkgConfig)
include(FindPackageHandleStandardArgs)

pkg_check_modules(LIBGSM_PKG QUIET gsm)

find_path(
    LIBGSM_INCLUDE_DIR
    NAMES gsm.h
    HINTS ${LIBGSM_PKG_INCLUDE_DIRS}
    PATHS /usr/local/include /usr/include
    PATH_SUFFIXES gsm)

find_library(
    LIBGSM_LIBRARIES
    NAMES gsm
    HINTS ${LIBGSM_PKG_LIBRARY_DIRS}
    PATHS /usr/local/lib /usr/lib)

if(LIBGSM_INCLUDE_DIR AND LIBGSM_LIBRARIES)
    set(LIBGSM_FOUND TRUE)
    set(LIBGSM_INCLUDE_DIRS ${LIBGSM_INCLUDE_DIR})
    set(LIBGSM_LIBRARIES ${LIBGSM_LIBRARIES} ${LIBGSM_LIBRARY})
endif(LIBGSM_INCLUDE_DIR AND LIBGSM_LIBRARIES)

find_package_handle_standard_args(GSM DEFAULT_MSG LIBGSM_LIBRARIES LIBGSM_INCLUDE_DIRS)
mark_as_advanced(LIBGSM_INCLUDE_DIR LIBGSM_LIBRARIES)

if(LIBGSM_FOUND AND NOT TARGET GSM::GSM)
    add_library(GSM::GSM INTERFACE IMPORTED)
    set_target_properties(
        GSM::GSM PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${LIBGSM_INCLUDE_DIRS}"
                            INTERFACE_LINK_LIBRARIES "${LIBGSM_LIBRARIES}")
endif()
