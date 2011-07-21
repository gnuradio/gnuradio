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

IF(NOT DEFINED INCLUDED_GR_PACKAGE_CMAKE)
SET(INCLUDED_GR_PACKAGE_CMAKE TRUE)

INCLUDE(GrVersion) #sets version information
INCLUDE(GrPlatform) #sets platform information

#set the cpack generator based on the platform type
IF(CPACK_GENERATOR)
    #already set by user
ELSEIF(APPLE)
    SET(CPACK_GENERATOR PackageMaker)
ELSEIF(WIN32)
    SET(CPACK_GENERATOR NSIS)
ELSEIF(DEBIAN)
    SET(CPACK_GENERATOR DEB)
ELSEIF(REDHAT)
    SET(CPACK_GENERATOR RPM)
ELSE()
    SET(CPACK_GENERATOR TGZ)
ENDIF()

########################################################################
# CPACK_SET - set a global variable and record the variable name
########################################################################
FUNCTION(CPACK_SET var)
    SET(${var} ${ARGN} CACHE INTERNAL "")
    LIST(APPEND _cpack_vars ${var})
    SET(_cpack_vars ${_cpack_vars} CACHE INTERNAL "")
ENDFUNCTION(CPACK_SET)

########################################################################
# CPACK_FINALIZE - include cpack and the unset all the cpack variables
########################################################################
FUNCTION(CPACK_FINALIZE)
    INCLUDE(CPack) #finalize the cpack settings configured throughout the build system
    FOREACH(var ${_cpack_vars})
        UNSET(${var} CACHE)
    ENDFOREACH(var)
    UNSET(_cpack_vars CACHE)
ENDFUNCTION(CPACK_FINALIZE)

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
FUNCTION(CPACK_COMPONENT name)
    INCLUDE(CMakeParseArgumentsCopy)
    SET(_options GROUP DISPLAY_NAME DESCRIPTION DEPENDS)
    CMAKE_PARSE_ARGUMENTS(CPACK_COMPONENT "" "${_options}" "" ${ARGN})

    FOREACH(_option ${_options})
        IF(CPACK_COMPONENT_${_option})
            CPACK_SET(CPACK_COMPONENT_${name}_${_option} "${CPACK_COMPONENT_${_option}}")
        ENDIF()
    ENDFOREACH(_option)

    CPACK_SET(CPACK_COMPONENTS_ALL "${CPACK_COMPONENTS_ALL};${name}")

ENDFUNCTION(CPACK_COMPONENT)

########################################################################
# Setup CPack
########################################################################
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GNU Radio")
SET(CPACK_PACKAGE_VENDOR              "Free Software Foundation, Inc.")
SET(CPACK_PACKAGE_CONTACT             "Discuss-gnuradio@gnu.org")
SET(CPACK_PACKAGE_VERSION ${VERSION})
SET(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/README)
SET(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README)
SET(CPACK_RESOURCE_FILE_WELCOME ${CMAKE_SOURCE_DIR}/README)
IF(${CPACK_GENERATOR} STREQUAL NSIS)
    SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${CMAKE_PROJECT_NAME}")
ENDIF()

########################################################################
# DEB package specific
########################################################################
SET(CPACK_DEBIAN_PACKAGE_DEPENDS
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
STRING(REPLACE ";" ", " CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}")
SET(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA ${CMAKE_SOURCE_DIR}/debian/postinst ${CMAKE_SOURCE_DIR}/debian/prerm)

########################################################################
# RPM package specific
########################################################################
SET(CPACK_RPM_PACKAGE_REQUIRES "boost-devel") #TODO other packages

########################################################################
# NSIS package specific
########################################################################
SET(CPACK_NSIS_MODIFY_PATH ON)

SET(HLKM_ENV "\\\"SYSTEM\\\\CurrentControlSet\\\\Control\\\\Session Manager\\\\Environment\\\"")

ENDIF(NOT DEFINED INCLUDED_GR_PACKAGE_CMAKE)
