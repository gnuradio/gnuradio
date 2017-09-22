# Copyright 2017 Free Software Foundation, Inc.
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

########################################################################
# This file contains functions that override those provided by CMake.
# We do this to allow for more generic use of these functions than as
# provided by CMake.
########################################################################

if(DEFINED __INCLUDED_CMAKE_OVERLOADS)
  return()
endif()
set(__INCLUDED_CMAKE_OVERLOADS TRUE)

# overload INCLUDE_DIRECTORIES to be a little smarter
#
# NOTE: moving all include directories to either BEFORE (internal to
# source or build) or AFTER (external to source or build) will work in
# general. The primary time it could fail is when include ordering is
# required to find a specific version of a header when multiple of the
# same name are available in the various include directories. This
# situation seems like it's unlikely, so we ignore it here.

macro(INCLUDE_DIRECTORIES)
  # for each provided include directory ...
  foreach(inc_dir ${ARGN})

    # is this dir the literal string "BEFORE" or "AFTER"?
    string(FIND ${inc_dir} BEFORE IS_BEFORE)
    string(FIND ${inc_dir} AFTER IS_AFTER)
    if(NOT ${IS_BEFORE} EQUAL 0 AND NOT ${IS_AFTER} EQUAL 0)

      # not "BEFORE" or "AFTER"; a real directory.
      # get absolute path of this include directory
      get_filename_component(inc_dir_abs ${inc_dir} ABSOLUTE)

      # is this include directory located within the SOURCE or BUILD?
      string(FIND ${inc_dir_abs} ${CMAKE_SOURCE_DIR} IS_IN_SOURCE)
      string(FIND ${inc_dir_abs} ${CMAKE_BINARY_DIR} IS_IN_BINARY)
      if(${IS_IN_SOURCE} EQUAL 0 OR ${IS_IN_BINARY} EQUAL 0)
        # yes: local SOURCE or BINARY; internal.
        # call the overloaded INCLUDE_DIRECTORIES,
        # prepending this internal directory.
        _include_directories(BEFORE ${inc_dir_abs})
      else()
        # no: not SOURCE or BUILD; must be external.
        # call the overloaded INCLUDE_DIRECTORIES,
        # appending this external directory.
        _include_directories(AFTER ${inc_dir_abs})
      endif()
    endif()
  endforeach()
endmacro(INCLUDE_DIRECTORIES)
