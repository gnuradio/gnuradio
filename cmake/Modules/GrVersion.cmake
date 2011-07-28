# Copyright 2011 Free Software Foundation, Inc.
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

IF(DEFINED __INCLUDED_GR_VERSION_CMAKE)
    RETURN()
ENDIF()
SET(__INCLUDED_GR_VERSION_CMAKE TRUE)

########################################################################
# Setup version variables.
# Parse the output of git describe
# sets VERSION and LIBVER
########################################################################

UNSET(VERSION)
UNSET(LIBVER)

########################################################################
# Extract the version string from git describe.
########################################################################
FIND_PACKAGE(Git)
IF(GIT_FOUND)
    MESSAGE(STATUS "Extracting version information from git...")
    EXECUTE_PROCESS(COMMAND ${GIT_EXECUTABLE} describe
        OUTPUT_VARIABLE VERSION OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    IF(NOT VERSION)
        MESSAGE(WARNING "Tried to extract $VERSION from git describe but failed... using default")
    ENDIF()
ENDIF(GIT_FOUND)

########################################################################
# Extract the library version from the version string.
########################################################################
IF(VERSION)
    INCLUDE(GrPython)
    EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -c "import re; print re.match('^v(\\d+\\.\\d+\\.\\d+)', '${VERSION}').groups()[0]"
        OUTPUT_VARIABLE LIBVER OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    IF(NOT LIBVER)
        MESSAGE(WARNING "Tried to extract $LIBVER from $VERSION but failed... using default")
    ENDIF()
ENDIF()

########################################################################
# Ensure that the version strings are set no matter what.
########################################################################
IF(NOT VERSION)
    SET(VERSION "v3.x.x-unknown")
ENDIF()

IF(NOT LIBVER)
    SET(LIBVER "3.x.x")
ENDIF()

MESSAGE(STATUS "VERSION: ${VERSION}, LIBVER: ${LIBVER}")
