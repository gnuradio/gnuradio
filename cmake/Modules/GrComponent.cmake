# Copyright 2010-2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

if(DEFINED __INCLUDED_GR_COMPONENT_CMAKE)
    return()
endif()
set(__INCLUDED_GR_COMPONENT_CMAKE TRUE)

set(_gr_enabled_components
    ""
    CACHE INTERNAL "" FORCE)
set(_gr_disabled_components
    ""
    CACHE INTERNAL "" FORCE)

if(NOT DEFINED ENABLE_DEFAULT)
    set(ENABLE_DEFAULT ON)
    message(STATUS "")
    message(STATUS "The build system will automatically enable all components.")
    message(STATUS "Use -DENABLE_DEFAULT=OFF to disable components by default.")
endif()

define_property(GLOBAL PROPERTY GR_ACTIVE_COMPONENT BRIEF_DOCS "Current GR Component being processed")


########################################################################
# Register a component into the system
# GR_REGISTER_COMPONENT(NAME <component-name> VAR <enabled-var> [NO_INSTALL]
#                       [REQUIRED_COMPONENTS <list of required compoents>] [REQUIRED_TPLS <required-tpls>])
########################################################################
function(GR_REGISTER_COMPONENT)
    set(OPTIONS NO_INSTALL)
    set(SINGLE_OPT NAME VAR)
    set(MULTI_OPT REQUIRED_COMPONENTS REQUIRED_TPLS)
    cmake_parse_arguments(GR_COMPONENT "${OPTIONS}" "${SINGLE_OPT}" "${MULTI_OPT}" ${ARGN})
    include(CMakeDependentOption)
    define_property(GLOBAL PROPERTY COMPONENT_${GR_COMPONENT_NAME}_VAR)
    set_property(GLOBAL PROPERTY COMPONENT_${GR_COMPONENT_NAME}_VAR ${GR_COMPONENT_VAR})
    foreach(dep ${GR_COMPONENT_REQUIRED_COMPONENTS})
        get_property(dep_var GLOBAL PROPERTY COMPONENT_${dep}_VAR)
        list(APPEND gr_component_dep_vars ${dep_var})
    endforeach(dep)
    message(STATUS "")
    message(STATUS "Configuring ${GR_COMPONENT_NAME} support...")
    foreach(dep ${gr_component_dep_vars})
        message(STATUS "  Component Dependency ${dep} = ${${dep}}")
    endforeach(dep)
    foreach(dep ${GR_COMPONENT_REQUIRED_TPLS})
        message(STATUS "  External Dependency ${dep} = ${${dep}}")
    endforeach()

    # if the user set the var to force on, we note this
    if("${${GR_COMPONENT_VAR}}" STREQUAL "ON")
        set(var_force TRUE)
    else()
        set(var_force FALSE)
    endif()

    # rewrite the dependency list so that deps that are also components use the cached version
    unset(comp_deps)
    foreach(dep ${gr_component_dep_vars})
        list(FIND _gr_enabled_components ${dep} dep_enb_index)
        list(FIND _gr_disabled_components ${dep} dep_dis_index)
        if(${dep_enb_index} EQUAL -1 AND ${dep_dis_index} EQUAL -1)
            list(APPEND comp_deps ${dep})
        else()
            list(APPEND comp_deps ${dep}_cached) #is a component, use cached version
        endif()
    endforeach(dep)
    set(comp_deps ${comp_deps} ${GR_COMPONENT_REQUIRED_TPLS})
    # setup the dependent option for this component
    cmake_dependent_option(${GR_COMPONENT_VAR} "enable ${GR_COMPONENT_NAME} support" ${ENABLE_DEFAULT}
                           "${comp_deps}" OFF)
    set(${GR_COMPONENT_VAR}
        "${${GR_COMPONENT_VAR}}"
        PARENT_SCOPE)
    set(${GR_COMPONENT_VAR}_cached
        "${${GR_COMPONENT_VAR}}"
        CACHE INTERNAL "" FORCE)

    #force was specified, but the dependencies were not met
    if(NOT ${GR_COMPONENT_VAR} AND var_force)
        message(FATAL_ERROR "user force-enabled ${GR_COMPONENT_NAME} but configuration checked failed")
    endif()

    # append the component into one of the lists
    if(${GR_COMPONENT_VAR})
        message(STATUS "  Enabling ${GR_COMPONENT_NAME} support.")
        list(APPEND _gr_enabled_components ${GR_COMPONENT_NAME})
    else(${GR_COMPONENT_VAR})
        message(STATUS "  Disabling ${GR_COMPONENT_NAME} support.")
        list(APPEND _gr_disabled_components ${GR_COMPONENT_NAME})
    endif(${GR_COMPONENT_VAR})
    message(STATUS "  Override with -D${GR_COMPONENT_VAR}=ON/OFF")

    # make components lists into global variables
    set(_gr_enabled_components
        ${_gr_enabled_components}
        CACHE INTERNAL "" FORCE)
    set(_gr_disabled_components
        ${_gr_disabled_components}
        CACHE INTERNAL "" FORCE)

    GR_DEFINE_COMPONENT_INTERFACE(${GR_COMPONENT_NAME} ${GR_COMPONENT_REQUIRED_COMPONENTS})
    if(NOT GR_COMPONENT_NO_INSTALL)
        set_property(GLOBAL PROPERTY GR_ACTIVE_COMPONENT ${GR_COMPONENT_NAME})
    endif()
endfunction(GR_REGISTER_COMPONENT)

function(GR_APPEND_SUBCOMPONENT name)
    list(APPEND _gr_enabled_components "* ${name}")
    set(_gr_enabled_components
        ${_gr_enabled_components}
        CACHE INTERNAL "" FORCE)
endfunction(GR_APPEND_SUBCOMPONENT name)

function(GR_CLEAR_ACTIVE_COMPONENT)
    set_property(GLOBAL PROPERTY GR_ACTIVE_COMPONENT)
endfunction()

function(GR_DEFINE_COMPONENT_INTERFACE name)
    define_property(GLOBAL PROPERTY COMPONENT_${name}_DEPS)
    define_property(GLOBAL PROPERTY COMPONENT_${name}_TARGETS)
    set_property(GLOBAL PROPERTY COMPONENT_${name}_DEPS ${ARGN})
endfunction()

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
