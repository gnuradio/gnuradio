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

IF(NOT DEFINED INCLUDED_GR_PLATFORM_CMAKE)
SET(INCLUDED_GR_PLATFORM_CMAKE TRUE)

########################################################################
# Setup additional defines for OS types
########################################################################
IF(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    SET(LINUX TRUE)
ENDIF()

IF(LINUX AND EXISTS "/etc/debian_version")
    SET(DEBIAN TRUE)
ENDIF()

IF(LINUX AND EXISTS "/etc/redhat-release")
    SET(REDHAT TRUE)
ENDIF()

########################################################################
# when the library suffix should be 64 (applies to redhat linux family)
########################################################################
IF(NOT DEFINED LIB_SUFFIX AND REDHAT AND CMAKE_SYSTEM_PROCESSOR MATCHES "64$")
    SET(LIB_SUFFIX 64)
ENDIF()
SET(LIB_SUFFIX ${LIB_SUFFIX} CACHE STRING "lib directory suffix")

ENDIF(NOT DEFINED INCLUDED_GR_PLATFORM_CMAKE)
