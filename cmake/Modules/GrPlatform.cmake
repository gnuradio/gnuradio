# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include_guard()

message(DEPRECATION
    "GrPlatform usage is deprecated, use GNUInstallDirs variables directly.\n"
    "See https://github.com/gnuradio/gnuradio/pull/7768."
)

########################################################################
# Setup additional defines for OS types
########################################################################
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LINUX TRUE)
endif()

if(NOT CMAKE_CROSSCOMPILING
   AND LINUX
   AND EXISTS "/etc/debian_version")
    set(DEBIAN TRUE)
endif()

if(NOT CMAKE_CROSSCOMPILING
   AND LINUX
   AND EXISTS "/etc/redhat-release")
    set(REDHAT TRUE)
endif()

if(NOT CMAKE_CROSSCOMPILING
   AND LINUX
   AND EXISTS "/etc/slackware-version")
    set(SLACKWARE TRUE)
endif()

########################################################################
# Try to compute LIB_SUFFIX as best as possible
########################################################################
get_property(enabled_languages GLOBAL PROPERTY ENABLED_LANGUAGES)
if(NOT (C IN_LIST enabled_languages OR CXX IN_LIST enabled_languages))
    message(WARNING "GrPlatform should be included after a project is defined")

    # Try best guess of LIB_SUFFIX
    if(REDHAT OR SLACKWARE)
        set(LIB64_CONVENTION TRUE)
    endif()

    if(NOT DEFINED LIB_SUFFIX
        AND LIB64_CONVENTION
        AND CMAKE_SYSTEM_PROCESSOR MATCHES "64$")
        set(LIB_SUFFIX 64)
    endif()
else()
    include(GNUInstallDirs)
    if(CMAKE_INSTALL_LIBDIR MATCHES "lib64$")
        set(LIB_SUFFIX 64)
    endif()
endif()

set(LIB_SUFFIX
    ${LIB_SUFFIX}
    CACHE STRING "lib directory suffix")
