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

if(DEFINED __INCLUDED_GR_COMPONENT_CMAKE)
    return()
endif()
set(__INCLUDED_GR_COMPONENT_CMAKE TRUE)

set(_gr_enabled_components "" CACHE INTERNAL "" FORCE)
set(_gr_disabled_components "" CACHE INTERNAL "" FORCE)

if(NOT DEFINED ENABLE_DEFAULT)
    set(ENABLE_DEFAULT ON)
    message(STATUS "")
    message(STATUS "The build system will automatically enable all components.")
    message(STATUS "Use -DENABLE_DEFAULT=OFF to disable components by default.")
endif()

########################################################################
# Register a component into the system
# - name: canonical component name
# - var: variable for enabled status
# - argn: list of dependencies
########################################################################
function(GR_REGISTER_COMPONENT name var)
    include(CMakeDependentOption)
    message(STATUS "")
    message(STATUS "Configuring ${name} support...")
    foreach(dep ${ARGN})
        message(STATUS "  Dependency ${dep} = ${${dep}}")
    endforeach(dep)

    #if the user set the var to force on, we note this
    if("${${var}}" STREQUAL "ON")
        set(var_force TRUE)
    else()
        set(var_force FALSE)
    endif()

    #rewrite the dependency list so that deps that are also components use the cached version
    unset(comp_deps)
    foreach(dep ${ARGN})
        list(FIND _gr_enabled_components ${dep} dep_enb_index)
        list(FIND _gr_disabled_components ${dep} dep_dis_index)
        if (${dep_enb_index} EQUAL -1 AND ${dep_dis_index} EQUAL -1)
            list(APPEND comp_deps ${dep})
        else()
            list(APPEND comp_deps ${dep}_cached) #is a component, use cached version
        endif()
    endforeach(dep)

    #setup the dependent option for this component
    CMAKE_DEPENDENT_OPTION(${var} "enable ${name} support" ${ENABLE_DEFAULT} "${comp_deps}" OFF)
    set(${var} "${${var}}" PARENT_SCOPE)
    set(${var}_cached "${${var}}" CACHE INTERNAL "" FORCE)

    #force was specified, but the dependencies were not met
    if(NOT ${var} AND var_force)
        message(FATAL_ERROR "user force-enabled ${name} but configuration checked failed")
    endif()

    #append the component into one of the lists
    if(${var})
        message(STATUS "  Enabling ${name} support.")
        list(APPEND _gr_enabled_components ${name})
    else(${var})
        message(STATUS "  Disabling ${name} support.")
        list(APPEND _gr_disabled_components ${name})
    endif(${var})
    message(STATUS "  Override with -D${var}=ON/OFF")

    #make components lists into global variables
    set(_gr_enabled_components ${_gr_enabled_components} CACHE INTERNAL "" FORCE)
    set(_gr_disabled_components ${_gr_disabled_components} CACHE INTERNAL "" FORCE)
endfunction(GR_REGISTER_COMPONENT)


function(GR_APPEND_SUBCOMPONENT name)
  list(APPEND _gr_enabled_components "* ${name}")
  set(_gr_enabled_components ${_gr_enabled_components} CACHE INTERNAL "" FORCE)
endfunction(GR_APPEND_SUBCOMPONENT name)

########################################################################
# Print the registered component summary
########################################################################
function(GR_PRINT_COMPONENT_SUMMARY)
    message(STATUS "")
    message(STATUS "######################################################")
    message(STATUS "# Gnuradio enabled components                         ")
    message(STATUS "######################################################")
    foreach(comp ${_gr_enabled_components})
        message(STATUS "  * ${comp}")
    endforeach(comp)

    message(STATUS "")
    message(STATUS "######################################################")
    message(STATUS "# Gnuradio disabled components                        ")
    message(STATUS "######################################################")
    foreach(comp ${_gr_disabled_components})
        message(STATUS "  * ${comp}")
    endforeach(comp)

    message(STATUS "")
endfunction(GR_PRINT_COMPONENT_SUMMARY)
