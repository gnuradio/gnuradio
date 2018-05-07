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

if(DEFINED __INCLUDED_GR_BOOST_CMAKE)
    return()
endif()
set(__INCLUDED_GR_BOOST_CMAKE TRUE)

########################################################################
# Setup Boost and handle some system specific things
########################################################################

set(BOOST_REQUIRED_COMPONENTS
    date_time
    program_options
    filesystem
    system
    regex
    thread
    unit_test_framework
)

if(UNIX AND NOT BOOST_ROOT AND EXISTS "/usr/lib64")
    list(APPEND BOOST_LIBRARYDIR "/usr/lib64") #fedora 64-bit fix
endif(UNIX AND NOT BOOST_ROOT AND EXISTS "/usr/lib64")

if(WIN32)
    #The following libraries are either used indirectly,
    #or conditionally within the various core components.
    #We explicitly list the libraries here because they
    #are either required in environments MSVC and MINGW
    #or linked-in automatically via header inclusion.

    #However, this is not robust; and its recommended that
    #these libraries should be listed in the main components
    #list once the minimum version of boost had been bumped
    #to a version which always contains these components.
    list(APPEND BOOST_REQUIRED_COMPONENTS
        atomic
        chrono
    )
endif(WIN32)

if(MSVC)
    if (NOT DEFINED BOOST_ALL_DYN_LINK)
        set(BOOST_ALL_DYN_LINK TRUE)
    endif()
    set(BOOST_ALL_DYN_LINK "${BOOST_ALL_DYN_LINK}" CACHE BOOL "boost enable dynamic linking")
    if(BOOST_ALL_DYN_LINK)
        add_definitions(-DBOOST_ALL_DYN_LINK) #setup boost auto-linking in msvc
    else(BOOST_ALL_DYN_LINK)
        unset(BOOST_REQUIRED_COMPONENTS) #empty components list for static link
    endif(BOOST_ALL_DYN_LINK)
endif(MSVC)

find_package(Boost ${GR_BOOST_MIN_VERSION} COMPONENTS ${BOOST_REQUIRED_COMPONENTS})

# This does not allow us to disable specific versions. It is used
# internally by cmake to know the formation newer versions. As newer
# Boost version beyond what is shown here are produced, we must extend
# this list. To disable Boost versions, see below.
set(Boost_ADDITIONAL_VERSIONS
    "1.45.0" "1.45" "1.46.0" "1.46" "1.47.0" "1.47" "1.48.0" "1.48" "1.49.0" "1.49"
    "1.50.0" "1.50" "1.51.0" "1.51" "1.52.0" "1.52" "1.53.0" "1.53" "1.54.0" "1.54"
    "1.55.0" "1.55" "1.56.0" "1.56" "1.57.0" "1.57" "1.58.0" "1.58" "1.59.0" "1.59"
    "1.60.0" "1.60" "1.61.0" "1.61" "1.62.0" "1.62" "1.63.0" "1.63" "1.64.0" "1.64"
    "1.65.0" "1.65" "1.66.0" "1.66" "1.67.0" "1.67" "1.68.0" "1.68" "1.69.0" "1.69"
)

# Boost 1.52 disabled, see https://svn.boost.org/trac/boost/ticket/7669
# Similar problems with Boost 1.46 and 1.47.

OPTION(ENABLE_BAD_BOOST "Enable known bad versions of Boost" OFF)
if(ENABLE_BAD_BOOST)
  MESSAGE(STATUS "Enabling use of known bad versions of Boost.")
endif(ENABLE_BAD_BOOST)

# For any unsuitable Boost version, add the version number below in
# the following format: XXYYZZ
# Where:
#     XX is the major version ('10' for version 1)
#     YY is the minor version number ('46' for 1.46)
#     ZZ is the patcher version number (typically just '00')
set(Boost_NOGO_VERSIONS
  104600 104601 104700 105200
  )

foreach(ver ${Boost_NOGO_VERSIONS})
  if("${Boost_VERSION}" STREQUAL "${ver}")
    if(NOT ENABLE_BAD_BOOST)
      MESSAGE(STATUS "WARNING: Found a known bad version of Boost (v${Boost_VERSION}). Disabling.")
      set(Boost_FOUND FALSE)
    else(NOT ENABLE_BAD_BOOST)
      MESSAGE(STATUS "WARNING: Found a known bad version of Boost (v${Boost_VERSION}). Continuing anyway.")
      set(Boost_FOUND TRUE)
    endif(NOT ENABLE_BAD_BOOST)
  endif("${Boost_VERSION}" STREQUAL "${ver}")
endforeach(ver)
