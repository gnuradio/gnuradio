# Copyright 2010-2011 Free Software Foundation, Inc.
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

IF(DEFINED __INCLUDED_GR_MISC_UTILS_CMAKE)
    RETURN()
ENDIF()
SET(__INCLUDED_GR_MISC_UTILS_CMAKE TRUE)

########################################################################
# Set global variable macro.
# Used for subdirectories to export settings.
# Example: include and library paths.
########################################################################
FUNCTION(GR_SET_GLOBAL var)
    SET(${var} ${ARGN} CACHE INTERNAL "" FORCE)
ENDFUNCTION(GR_SET_GLOBAL)

########################################################################
# Set the pre-processor definition if the condition is true.
#  - def the pre-processor definition to set and condition name
########################################################################
FUNCTION(GR_ADD_COND_DEF def)
    IF(${def})
        ADD_DEFINITIONS(-D${def})
    ENDIF(${def})
ENDFUNCTION(GR_ADD_COND_DEF)

########################################################################
# Check for a header and conditionally set a compile define.
#  - hdr the relative path to the header file
#  - def the pre-processor definition to set
########################################################################
FUNCTION(GR_CHECK_HDR_N_DEF hdr def)
    INCLUDE(CheckIncludeFileCXX)
    CHECK_INCLUDE_FILE_CXX(${hdr} ${def})
    GR_ADD_COND_DEF(${def})
ENDFUNCTION(GR_CHECK_HDR_N_DEF)

########################################################################
# Include subdirectory macro.
# Sets the CMake directory variables,
# includes the subdirectory CMakeLists.txt,
# resets the CMake directory variables.
#
# This macro includes subdirectories rather than adding them
# so that the subdirectory can affect variables in the level above.
# This provides a work-around for the lack of convenience libraries.
# This way a subdirectory can append to the list of library sources.
########################################################################
MACRO(GR_INCLUDE_SUBDIRECTORY subdir)
    #insert the current directories on the front of the list
    LIST(INSERT _cmake_source_dirs 0 ${CMAKE_CURRENT_SOURCE_DIR})
    LIST(INSERT _cmake_binary_dirs 0 ${CMAKE_CURRENT_BINARY_DIR})

    #set the current directories to the names of the subdirs
    SET(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${subdir})
    SET(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${subdir})

    #include the subdirectory CMakeLists to run it
    FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

    #reset the value of the current directories
    LIST(GET _cmake_source_dirs 0 CMAKE_CURRENT_SOURCE_DIR)
    LIST(GET _cmake_binary_dirs 0 CMAKE_CURRENT_BINARY_DIR)

    #pop the subdir names of the front of the list
    LIST(REMOVE_AT _cmake_source_dirs 0)
    LIST(REMOVE_AT _cmake_binary_dirs 0)
ENDMACRO(GR_INCLUDE_SUBDIRECTORY)
