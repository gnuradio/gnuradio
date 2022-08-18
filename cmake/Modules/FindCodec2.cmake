# Copyright 2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

##############################
# Check for system libcodec2 #
##############################

find_package(PkgConfig)
include(FindPackageHandleStandardArgs)

pkg_check_modules(LIBCODEC2_PKG QUIET codec2)

find_path(
    LIBCODEC2_INCLUDE_DIR
    NAMES codec2.h
    HINTS ${LIBCODEC2_PKG_INCLUDE_DIRS}
    PATHS /usr/include /usr/local/include
    PATH_SUFFIXES codec2)

find_library(
    LIBCODEC2_LIBRARIES
    NAMES codec2 libcodec2
    HINTS ${LIBCODEC2_PKG_LIBRARY_DIRS}
    PATHS /usr/lib /usr/local/lib)

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
        string(REGEX MATCHALL "FREEDV_MODE[_a-z0-9A-Z]+" FREEDV_MODES
                     "${_FREEDV_API_H_CONTENTS}")
        foreach(mode ${FREEDV_MODES})
            set(LIBCODEC2_HAS_${mode} True)
        endforeach()
    else()
        set(LIBCODEC2_HAS_FREEDV_API false)
    endif()

    # we need to discover if codec2 < 0.9
    # moreover, codec2 < 0.8 doesn't provide codec2/version.h
    if(EXISTS "${LIBCODEC2_INCLUDE_DIRS}/version.h")
        file(READ "${LIBCODEC2_INCLUDE_DIRS}/version.h" _CODEC2_VERSION_H_CONTENTS)
        string(REGEX MATCH
                     "CODEC2_VERSION_MAJOR ([0-9]+)(.*)CODEC2_VERSION_MINOR ([0-9]+)"
                     _CODEC2_TMP_MATCH " ${_CODEC2_VERSION_H_CONTENTS}")
        if((CMAKE_MATCH_1 EQUAL 0) AND (CMAKE_MATCH_3 LESS 9))
            add_definitions(-DCODEC2_LEGACY)
        endif()
    else()
        add_definitions(-DCODEC2_LEGACY)
    endif()
endif(LIBCODEC2_INCLUDE_DIR AND LIBCODEC2_LIBRARIES)

find_package_handle_standard_args(Codec2 DEFAULT_MSG LIBCODEC2_LIBRARIES
                                  LIBCODEC2_INCLUDE_DIRS)
mark_as_advanced(LIBCODEC2_INCLUDE_DIR LIBCODEC2_LIBRARIES)

if(LIBCODEC2_FOUND AND NOT TARGET CODEC2::CODEC2)
    add_library(CODEC2::CODEC2 INTERFACE IMPORTED)
    set_target_properties(
        CODEC2::CODEC2
        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${LIBCODEC2_INCLUDE_DIRS}"
                   INTERFACE_LINK_LIBRARIES "${LIBCODEC2_LIBRARIES}")
endif()
