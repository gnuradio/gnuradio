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

IF(DEFINED __INCLUDED_GR_COMPONENT_CMAKE)
    RETURN()
ENDIF()
SET(__INCLUDED_GR_COMPONENT_CMAKE TRUE)

SET(_gr_enabled_components "" CACHE INTERNAL "" FORCE)
SET(_gr_disabled_components "" CACHE INTERNAL "" FORCE)

########################################################################
# Register a component into the system
# - name: canonical component name
# - var: variable for enabled status
# - argn: list of dependencies
########################################################################
FUNCTION(GR_REGISTER_COMPONENT name var)
    INCLUDE(CMakeDependentOption)
    MESSAGE(STATUS "")
    MESSAGE(STATUS "Configuring ${name} support...")
    FOREACH(dep ${ARGN})
        MESSAGE(STATUS "  Dependency ${dep} = ${${dep}}")
    ENDFOREACH(dep)

    #setup the dependent option for this component
    CMAKE_DEPENDENT_OPTION(${var} "enable ${name} support" ON "${ARGN}" OFF)
    SET(${var}_ "${${var}}" CACHE INTERNAL "" FORCE)

    #append the component into one of the lists
    IF(${var})
        MESSAGE(STATUS "  Enabling ${name} support.")
        LIST(APPEND _gr_enabled_components ${name})
    ELSE(${var})
        MESSAGE(STATUS "  Disabling ${name} support.")
        LIST(APPEND _gr_disabled_components ${name})
    ENDIF(${var})
    MESSAGE(STATUS "  Override with -D${var}=ON/OFF")

    #make components lists into global variables
    SET(_gr_enabled_components ${_gr_enabled_components} CACHE INTERNAL "" FORCE)
    SET(_gr_disabled_components ${_gr_disabled_components} CACHE INTERNAL "" FORCE)
ENDFUNCTION(GR_REGISTER_COMPONENT)

########################################################################
# Print the registered component summary
########################################################################
FUNCTION(GR_PRINT_COMPONENT_SUMMARY)
    MESSAGE(STATUS "")
    MESSAGE(STATUS "######################################################")
    MESSAGE(STATUS "# Gnuradio enabled components                         ")
    MESSAGE(STATUS "######################################################")
    FOREACH(comp ${_gr_enabled_components})
        MESSAGE(STATUS "  * ${comp}")
    ENDFOREACH(comp)

    MESSAGE(STATUS "")
    MESSAGE(STATUS "######################################################")
    MESSAGE(STATUS "# Gnuradio disabled components                        ")
    MESSAGE(STATUS "######################################################")
    FOREACH(comp ${_gr_disabled_components})
        MESSAGE(STATUS "  * ${comp}")
    ENDFOREACH(comp)

    MESSAGE(STATUS "")
ENDFUNCTION(GR_PRINT_COMPONENT_SUMMARY)
