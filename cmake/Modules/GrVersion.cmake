# Copyright 2011,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

if(DEFINED __INCLUDED_GR_VERSION_CMAKE)
    return()
endif()
set(__INCLUDED_GR_VERSION_CMAKE TRUE)

# always have a value for VERSION_PATCH, even if unset (e.g. some OOTs)
if(NOT DEFINED VERSION_PATCH)
    set(VERSION_PATCH "0")
endif()

#eventually, replace version.sh and fill in the variables below
set(MAJOR_VERSION "${VERSION_MAJOR}")
set(API_COMPAT "${VERSION_API}")
set(MINOR_VERSION "${VERSION_ABI}")
set(MAINT_VERSION "${VERSION_PATCH}")

########################################################################
# Extract the version string from git describe.
########################################################################
find_package(Git)

macro(create_manual_git_describe)
    if(NOT GR_GIT_COUNT)
        set(GR_GIT_COUNT "compat-xxx")
    endif()
    if(NOT GR_GIT_HASH)
        set(GR_GIT_HASH "xunknown")
    endif()
    set(GIT_DESCRIBE "v${MAJOR_VERSION}.${API_COMPAT}-${GR_GIT_COUNT}-${GR_GIT_HASH}")
endmacro()

if(GIT_FOUND AND EXISTS ${PROJECT_SOURCE_DIR}/.git)
    message(STATUS "Extracting version information from git describe...")
    # try to get long description with tag followed by hash
    execute_process(
        COMMAND ${GIT_EXECUTABLE} describe --abbrev=8 --long
        OUTPUT_VARIABLE GIT_DESCRIBE
        OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    # long description failed, so try to just get commit hash
    # (prefixed by "g" so that a hash that is just a number is not misinterpreted)
    if(GIT_DESCRIBE STREQUAL "")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} log --format=g%h -n 1
            OUTPUT_VARIABLE GIT_DESCRIBE
            OUTPUT_STRIP_TRAILING_WHITESPACE
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    endif()
    # git is failing, fallback
    if(GIT_DESCRIBE STREQUAL "")
        create_manual_git_describe()
    endif()
else()
    create_manual_git_describe()
endif()

########################################################################
# Use the logic below to set the version constants
########################################################################
## HEADER VARIABLES -- see include/gnuradio/version.h.in
set(GR_VERSION_MAJOR "${VERSION_MAJOR}")
set(GR_VERSION_API "${VERSION_API}")
set(GR_VERSION_MINOR "${VERSION_ABI}")
set(GR_VERSION_MAINT "${VERSION_PATCH}")
## Main Variables
if("${MINOR_VERSION}" STREQUAL "git")
    # VERSION: 3.3git-xxx-gxxxxxxxx
    # DOCVER:  3.3git
    # LIBVER:  3.3git
    set(VERSION "${GIT_DESCRIBE}")
    set(DOCVER "${MAJOR_VERSION}.${API_COMPAT}${MINOR_VERSION}")
    set(LIBVER "${MAJOR_VERSION}.${API_COMPAT}${MINOR_VERSION}")
    set(RC_MINOR_VERSION "0")
    set(RC_MAINT_VERSION "0")
    set(GR_VERSION_MINOR "1023")
elseif("${MAINT_VERSION}" STREQUAL "git")
    # VERSION: 3.3.1git-xxx-gxxxxxxxx
    # DOCVER:  3.3.1git
    # LIBVER:  3.3.1git
    set(VERSION "${GIT_DESCRIBE}")
    set(DOCVER "${MAJOR_VERSION}.${API_COMPAT}.${MINOR_VERSION}${MAINT_VERSION}")
    set(LIBVER "${MAJOR_VERSION}.${API_COMPAT}.${MINOR_VERSION}${MAINT_VERSION}")
    math(EXPR RC_MINOR_VERSION "${MINOR_VERSION} - 1")
    set(RC_MAINT_VERSION "0")
    set(GR_VERSION_MAINT "1023")
else()
    # This is a numbered release.
    # VERSION: 3.3.1{.x}
    # DOCVER:  3.3.1{.x}
    # LIBVER:  3.3.1{.x}
    set(VERSION "${MAJOR_VERSION}.${API_COMPAT}.${MINOR_VERSION}.${MAINT_VERSION}")
    set(DOCVER "${VERSION}")
    set(LIBVER "${MAJOR_VERSION}.${API_COMPAT}.${MINOR_VERSION}")
    # ensure only numeric values are set for RC_ version parts
    string(REGEX REPLACE "^([0-9]+).*$" "\\1" RC_MINOR_VERSION "${MINOR_VERSION}")
    string(REGEX REPLACE "^([0-9]+).*$" "\\1" RC_MAINT_VERSION "${MAINT_VERSION}")
endif()
