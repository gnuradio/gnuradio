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

IF(NOT DEFINED INCLUDED_GR_TEST_CMAKE)
SET(INCLUDED_GR_TEST_CMAKE TRUE)

########################################################################
# Add a unit test and setup the environment for a unit test.
# Takes the same arguments as the ADD_TEST function.
#
# Before calling set the following variables:
# GR_TEST_TARGET_DEPS  - built targets for the library path
# GR_TEST_LIBRARY_DIRS - directories for the library path
# GR_TEST_PYTHON_DIRS  - directories for the python path
########################################################################
FUNCTION(GR_ADD_TEST test_name)

    IF(WIN32)
        #Ensure that the build exe also appears in the PATH.
        LIST(APPEND GR_TEST_TARGET_DEPS ${ARGN})

        #In the land of windows, all libraries must be in the PATH.
        #Since the dependent libraries are not yet installed,
        #we must manually set them in the PATH to run tests.
        #The following appends the path of a target dependency.
        FOREACH(target ${GR_TEST_TARGET_DEPS})
            GET_TARGET_PROPERTY(location ${target} LOCATION)
            IF(location)
                GET_FILENAME_COMPONENT(path ${location} PATH)
                STRING(REGEX REPLACE "\\$\\(.*\\)" ${CMAKE_BUILD_TYPE} path ${path})
                LIST(APPEND GR_TEST_LIBRARY_DIRS ${path})
            ENDIF(location)
        ENDFOREACH(target)

        #SWIG generates the python library files into a subdirectory.
        #Therefore, we must append this subdirectory into PYTHONPATH.
        #Only do this for the python directories matching the following:
        FOREACH(pydir ${GR_TEST_PYTHON_DIRS})
            GET_FILENAME_COMPONENT(name ${pydir} NAME)
            IF(name MATCHES "^(swig|lib|src)$")
                LIST(APPEND GR_TEST_PYTHON_DIRS ${pydir}/${CMAKE_BUILD_TYPE})
            ENDIF()
        ENDFOREACH(pydir)
    ENDIF(WIN32)

    FILE(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR} srcdir)
    FILE(TO_NATIVE_PATH "${GR_TEST_LIBRARY_DIRS}" libpath) #ok to use on dir list?
    FILE(TO_NATIVE_PATH "${GR_TEST_PYTHON_DIRS}" pypath) #ok to use on dir list?

    SET(environs "GR_DONT_LOAD_PREFS=1" "srcdir=${srcdir}")

    #http://www.cmake.org/pipermail/cmake/2009-May/029464.html
    #Replaced this add test + set environs code with the shell script generation.
    #Its nicer to be able to manually run the shell script to diagnose problems.
    #ADD_TEST(${ARGV})
    #SET_TESTS_PROPERTIES(${test_name} PROPERTIES ENVIRONMENT "${environs}")

    IF(UNIX)
        SET(binpath "${CMAKE_CURRENT_BINARY_DIR}:$PATH")
        #set both LD and DYLD paths to cover multiple UNIX OS library paths
        LIST(APPEND libpath "$LD_LIBRARY_PATH" "$DYLD_LIBRARY_PATH")
        LIST(APPEND pypath "$PYTHONPATH")

        #replace list separator with the path separator
        STRING(REPLACE ";" ":" libpath "${libpath}")
        STRING(REPLACE ";" ":" pypath "${pypath}")
        LIST(APPEND environs "PATH=${binpath}" "LD_LIBRARY_PATH=${libpath}" "DYLD_LIBRARY_PATH=${libpath}" "PYTHONPATH=${pypath}")

        #generate a bat file that sets the environment and runs the test
        FIND_PROGRAM(SHELL sh)
        SET(sh_file ${CMAKE_CURRENT_BINARY_DIR}/${test_name}_test.sh)
        FILE(WRITE ${sh_file} "#!${SHELL}\n")
        #each line sets an environment variable
        FOREACH(environ ${environs})
            FILE(APPEND ${sh_file} "export ${environ}\n")
        ENDFOREACH(environ)
        #load the command to run with its arguments
        FOREACH(arg ${ARGN})
            FILE(APPEND ${sh_file} "${arg} ")
        ENDFOREACH(arg)
        FILE(APPEND ${sh_file} "\n")

        ADD_TEST(${test_name} ${SHELL} ${sh_file})

    ENDIF(UNIX)

    IF(WIN32)
        LIST(APPEND libpath ${DLL_PATHS} "%PATH%")
        LIST(APPEND pypath "%PYTHONPATH%")

        #replace list separator with the path separator (escaped)
        STRING(REPLACE ";" "\\;" libpath "${libpath}")
        STRING(REPLACE ";" "\\;" pypath "${pypath}")
        LIST(APPEND environs "PATH=${libpath}" "PYTHONPATH=${pypath}")

        #generate a bat file that sets the environment and runs the test
        SET(bat_file ${CMAKE_CURRENT_BINARY_DIR}/${test_name}_test.bat)
        FILE(WRITE ${bat_file} "@echo off\n")
        #each line sets an environment variable
        FOREACH(environ ${environs})
            FILE(APPEND ${bat_file} "SET ${environ}\n")
        ENDFOREACH(environ)
        #load the command to run with its arguments
        FOREACH(arg ${ARGN})
            FILE(APPEND ${bat_file} "${arg} ")
        ENDFOREACH(arg)
        FILE(APPEND ${bat_file} "\n")

        ADD_TEST(${test_name} ${bat_file})
    ENDIF(WIN32)

ENDFUNCTION(GR_ADD_TEST)

ENDIF(NOT DEFINED INCLUDED_GR_TEST_CMAKE)
