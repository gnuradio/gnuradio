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

if(DEFINED __INCLUDED_GR_PLATFORM_CMAKE)
    return()
endif()
set(__INCLUDED_GR_PLATFORM_CMAKE TRUE)

########################################################################
# Setup additional defines for OS types
########################################################################
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LINUX TRUE)
endif()

if(NOT CMAKE_CROSSCOMPILING AND LINUX AND EXISTS "/etc/debian_version")
    set(DEBIAN TRUE)
endif()

if(NOT CMAKE_CROSSCOMPILING AND LINUX AND EXISTS "/etc/redhat-release")
    set(REDHAT TRUE)
endif()

if(NOT CMAKE_CROSSCOMPILING AND LINUX AND EXISTS "/etc/slackware-version")
    set(SLACKWARE TRUE)
endif()

########################################################################
# when the library suffix should be 64 (applies to redhat linux family)
########################################################################
if (REDHAT OR SLACKWARE)
    set(LIB64_CONVENTION TRUE)
endif()

if(NOT DEFINED LIB_SUFFIX AND LIB64_CONVENTION AND CMAKE_SYSTEM_PROCESSOR MATCHES "64$")
    set(LIB_SUFFIX 64)
endif()

########################################################################
# Detect /lib versus /lib64
########################################################################
if (CMAKE_INSTALL_LIBDIR MATCHES lib64)
    set(LIB_SUFFIX 64)
endif()

set(LIB_SUFFIX ${LIB_SUFFIX} CACHE STRING "lib directory suffix")
