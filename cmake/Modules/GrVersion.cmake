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

if(DEFINED __INCLUDED_GR_VERSION_CMAKE)
    return()
endif()
set(__INCLUDED_GR_VERSION_CMAKE TRUE)

########################################################################
# Setup version variables.
# Parse the output of git describe
# sets VERSION and LIBVER
########################################################################

unset(VERSION)
unset(LIBVER)

########################################################################
# Extract the version string from git describe.
########################################################################
find_package(Git)
if(GIT_FOUND)
    message(STATUS "Extracting version information from git...")
    execute_process(COMMAND ${GIT_EXECUTABLE} describe
        OUTPUT_VARIABLE VERSION OUTPUT_STRIP_TRAILING_WHITESPACE
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    if(NOT VERSION)
        message(WARNING "Tried to extract $VERSION from git describe but failed... using default")
    endif()
endif(GIT_FOUND)

########################################################################
# Extract the library version from the version string.
########################################################################
if(VERSION)
    include(GrPython)
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import re; print re.match('^v(\\d+\\.\\d+\\.\\d+)', '${VERSION}').groups()[0]"
        OUTPUT_VARIABLE LIBVER OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT LIBVER)
        message(WARNING "Tried to extract $LIBVER from $VERSION but failed... using default")
    endif()
endif()

########################################################################
# Ensure that the version strings are set no matter what.
########################################################################
if(NOT VERSION)
    set(VERSION "v3.x.x-unknown")
endif()

if(NOT LIBVER)
    set(LIBVER "3.x.x")
endif()

message(STATUS "VERSION: ${VERSION}, LIBVER: ${LIBVER}")
