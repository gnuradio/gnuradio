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

IF(NOT DEFINED INCLUDED_GR_BOOST_CMAKE)
SET(INCLUDED_GR_BOOST_CMAKE TRUE)

########################################################################
# Setup Boost and handle some system specific things
########################################################################

SET(BOOST_REQUIRED_COMPONENTS
    date_time
    program_options
    filesystem
    system
    thread
)

IF(UNIX AND EXISTS "/usr/lib64")
    LIST(APPEND BOOST_LIBRARYDIR "/usr/lib64") #fedora 64-bit fix
ENDIF(UNIX AND EXISTS "/usr/lib64")

IF(MSVC)
    IF (NOT DEFINED BOOST_ALL_DYN_LINK)
        SET(BOOST_ALL_DYN_LINK TRUE)
    ENDIF()
    SET(BOOST_ALL_DYN_LINK "${BOOST_ALL_DYN_LINK}" CACHE BOOL "boost enable dynamic linking")
    IF(BOOST_ALL_DYN_LINK)
        ADD_DEFINITIONS(-DBOOST_ALL_DYN_LINK) #setup boost auto-linking in msvc
    ELSE(BOOST_ALL_DYN_LINK)
        UNSET(BOOST_REQUIRED_COMPONENTS) #empty components list for static link
    ENDIF(BOOST_ALL_DYN_LINK)
ENDIF(MSVC)

SET(Boost_ADDITIONAL_VERSIONS "1.42.0" "1.42" "1.43.0" "1.43" "1.44.0" "1.44" "1.45.0" "1.45" "1.46.0" "1.46" "1.47.0" "1.47")
FIND_PACKAGE(Boost "1.35" COMPONENTS ${BOOST_REQUIRED_COMPONENTS})

ENDIF(NOT DEFINED INCLUDED_GR_BOOST_CMAKE)
