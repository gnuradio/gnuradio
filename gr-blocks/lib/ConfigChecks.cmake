# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
INCLUDE(GrMiscUtils)
INCLUDE(CheckCXXSourceCompiles)

IF(MSVC) #add this directory for our provided headers
LIST(APPEND CMAKE_REQUIRED_INCLUDES ${CMAKE_SOURCE_DIR}/msvc)
ENDIF(MSVC)

GR_CHECK_HDR_N_DEF(io.h HAVE_IO_H)

########################################################################
CHECK_INCLUDE_FILE_CXX(windows.h HAVE_WINDOWS_H)
IF(HAVE_WINDOWS_H)
    ADD_DEFINITIONS(-DHAVE_WINDOWS_H -DUSING_WINSOCK)
    MESSAGE(STATUS "Adding windows libs to gr blocks libs...")
    LIST(APPEND blocks_libs ws2_32 wsock32)
ENDIF(HAVE_WINDOWS_H)
