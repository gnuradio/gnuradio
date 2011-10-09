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

if(DEFINED __INCLUDED_GR_PACKAGE_CMAKE)
    return()
endif()
set(__INCLUDED_GR_PACKAGE_CMAKE TRUE)

include(GrVersion) #sets version information
include(GrPlatform) #sets platform information

#set the cpack generator based on the platform type
if(CPACK_GENERATOR)
    #already set by user
elseif(APPLE)
    set(CPACK_GENERATOR PackageMaker)
elseif(WIN32)
    set(CPACK_GENERATOR NSIS)
elseif(DEBIAN)
    set(CPACK_GENERATOR DEB)
elseif(REDHAT)
    set(CPACK_GENERATOR RPM)
else()
    set(CPACK_GENERATOR TGZ)
endif()

########################################################################
# CPACK_SET - set a global variable and record the variable name
########################################################################
function(CPACK_SET var)
    set(${var} ${ARGN} CACHE INTERNAL "")
    list(APPEND _cpack_vars ${var})
    list(REMOVE_DUPLICATES _cpack_vars)
    set(_cpack_vars ${_cpack_vars} CACHE INTERNAL "")
endfunction(CPACK_SET)

########################################################################
# CPACK_FINALIZE - include cpack and the unset all the cpack variables
########################################################################
function(CPACK_FINALIZE)
    include(CPack) #finalize the cpack settings configured throughout the build system
    foreach(var ${_cpack_vars})
        unset(${var} CACHE)
    endforeach(var)
    unset(_cpack_vars CACHE)
endfunction(CPACK_FINALIZE)

########################################################################
# CPACK_COMPONENT - convenience function to create a cpack component
#
# Usage: CPACK_COMPONENT(
#   name
#   [GROUP group]
#   [DISPLAY_NAME display_name]
#   [DESCRIPTION description]
#   [DEPENDS depends]
# )
########################################################################
function(CPACK_COMPONENT name)
    include(CMakeParseArgumentsCopy)
    set(_options GROUP DISPLAY_NAME DESCRIPTION DEPENDS)
    CMAKE_PARSE_ARGUMENTS(CPACK_COMPONENT "" "${_options}" "" ${ARGN})

    string(TOUPPER "${name}" name_upper)
    foreach(_option ${_options})
        if(CPACK_COMPONENT_${_option})
            CPACK_SET(CPACK_COMPONENT_${name_upper}_${_option} "${CPACK_COMPONENT_${_option}}")
        endif()
    endforeach(_option)

    CPACK_SET(CPACK_COMPONENTS_ALL "${CPACK_COMPONENTS_ALL};${name}")

endfunction(CPACK_COMPONENT)

########################################################################
# Setup CPack
########################################################################
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GNU Radio")
set(CPACK_PACKAGE_VENDOR              "Free Software Foundation, Inc.")
set(CPACK_PACKAGE_CONTACT             "Discuss-gnuradio@gnu.org")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/README)
set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README)
set(CPACK_RESOURCE_FILE_WELCOME ${CMAKE_SOURCE_DIR}/README)
if(${CPACK_GENERATOR} STREQUAL NSIS)
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CMAKE_PROJECT_NAME}")
endif()

########################################################################
# DEB package specific
########################################################################
set(CPACK_DEBIAN_PACKAGE_DEPENDS
    "libboost-all-dev"
    "libfftw3-3"
    "python"
    "python-numpy"
    "libqt4-core"
    "libqwt5-qt4"
    "libqwtplot3d-qt4-0"
    "python-qt4"
    "python-gtk2"
    "python-lxml"
    "python-Cheetah"
)
string(REPLACE ";" ", " CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA ${CMAKE_SOURCE_DIR}/debian/postinst ${CMAKE_SOURCE_DIR}/debian/prerm)

########################################################################
# RPM package specific
########################################################################
set(CPACK_RPM_PACKAGE_REQUIRES "boost-devel") #TODO other packages

########################################################################
# NSIS package specific
########################################################################
set(CPACK_NSIS_MODIFY_PATH ON)

set(HLKM_ENV "\\\"SYSTEM\\\\CurrentControlSet\\\\Control\\\\Session Manager\\\\Environment\\\"")
