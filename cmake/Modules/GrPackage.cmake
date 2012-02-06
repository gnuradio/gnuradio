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

    #set the package depends for monolithic package
    foreach(comp ${CPACK_COMPONENTS_ALL})
        string(TOUPPER "PACKAGE_DEPENDS_${comp}" package_depends_var)
        list(APPEND PACKAGE_DEPENDS_ALL ${${package_depends_var}})
    endforeach(comp)
    string(REPLACE ";" ", " CPACK_DEBIAN_PACKAGE_DEPENDS "${PACKAGE_DEPENDS_ALL}")
    string(REPLACE ";" ", " CPACK_RPM_PACKAGE_REQUIRES "${PACKAGE_DEPENDS_ALL}")

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
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "GNU Radio - The GNU Software Radio")
set(CPACK_PACKAGE_VENDOR              "Free Software Foundation, Inc.")
set(CPACK_PACKAGE_CONTACT             "Discuss GNURadio <discuss-gnuradio@gnu.org>")
set(CPACK_PACKAGE_VERSION ${VERSION})
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/README)
set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README)
set(CPACK_RESOURCE_FILE_WELCOME ${CMAKE_SOURCE_DIR}/README)

find_program(LSB_RELEASE_EXECUTABLE lsb_release)

if((DEBIAN OR REDHAT) AND LSB_RELEASE_EXECUTABLE)

    #extract system information by executing the commands
    execute_process(
        COMMAND ${LSB_RELEASE_EXECUTABLE} --short --id
        OUTPUT_VARIABLE LSB_ID OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${LSB_RELEASE_EXECUTABLE} --short --release
        OUTPUT_VARIABLE LSB_RELEASE OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    #set a more sensible package name for this system
    SET(CPACK_PACKAGE_FILE_NAME "gnuradio-${CPACK_PACKAGE_VERSION}-${LSB_ID}-${LSB_RELEASE}-${CMAKE_SYSTEM_PROCESSOR}")

    #now try to include the component based dependencies
    set(package_deps_file "${CMAKE_SOURCE_DIR}/cmake/Packaging/${LSB_ID}-${LSB_RELEASE}.cmake")
    if (EXISTS ${package_deps_file})
        include(${package_deps_file})
    endif()

endif()

if(${CPACK_GENERATOR} STREQUAL NSIS)
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "${CMAKE_PROJECT_NAME}")
endif()

########################################################################
# DEB package specific
########################################################################
foreach(filename preinst postinst prerm postrm)
    list(APPEND CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA ${CMAKE_BINARY_DIR}/Packaging/${filename})
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Packaging)
    configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/Packaging/${filename}.in
        ${CMAKE_BINARY_DIR}/Packaging/${filename}
    @ONLY)
endforeach(filename)

########################################################################
# RPM package specific
########################################################################
foreach(filename post_install post_uninstall pre_install pre_uninstall)
    string(TOUPPER ${filename} filename_upper)
    list(APPEND CPACK_RPM_${filename_upper}_SCRIPT_FILE ${CMAKE_BINARY_DIR}/Packaging/${filename})
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/Packaging)
    configure_file(
        ${CMAKE_SOURCE_DIR}/cmake/Packaging/${filename}.in
        ${CMAKE_BINARY_DIR}/Packaging/${filename}
    @ONLY)
endforeach(filename)

########################################################################
# NSIS package specific
########################################################################
set(CPACK_NSIS_MODIFY_PATH ON)

set(HLKM_ENV "\\\"SYSTEM\\\\CurrentControlSet\\\\Control\\\\Session Manager\\\\Environment\\\"")
