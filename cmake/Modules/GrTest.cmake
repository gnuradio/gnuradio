# Copyright 2010-2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

if(DEFINED __INCLUDED_GR_TEST_CMAKE)
    return()
endif()
set(__INCLUDED_GR_TEST_CMAKE TRUE)

function(GR_CONVERT_QUOTED_STRING path_str quoted_path)
    file(TO_NATIVE_PATH "${path_str}" path_str)
    string(CONCAT path_str "\"" ${path_str} "\"")
    string(REPLACE "\\ " " " path_str ${path_str})
    set(${quoted_path}
        "${path_str}"
        PARENT_SCOPE)
endfunction()

########################################################################
# Add a unit test and setup the environment for a unit test.
# Takes the same arguments as the ADD_TEST function.
#
# Before calling set the following variables:
# GR_TEST_TARGET_DEPS  - built targets for the library path
# GR_TEST_LIBRARY_DIRS - directories for the library path
# GR_TEST_PYTHON_DIRS  - directories for the python path
# GR_TEST_ENVIRONS  - other environment key/value pairs
########################################################################
function(GR_ADD_TEST test_name)

    #Ensure that the build exe also appears in the PATH.
    list(APPEND GR_TEST_TARGET_DEPS ${ARGN})

    #In the land of windows, all libraries must be in the PATH.
    #Since the dependent libraries are not yet installed,
    #we must manually set them in the PATH to run tests.
    #The following appends the path of a target dependency.
    foreach(target ${GR_TEST_TARGET_DEPS})
        get_filename_component(PATH $<TARGET_FILE:$target> PATH)
        string(REGEX REPLACE "\\$\\(.*\\)" "${CMAKE_BUILD_TYPE}" path "${path}")
        list(APPEND GR_TEST_LIBRARY_DIRS ${path})
    endforeach(target)

    if(WIN32)
        #SWIG generates the python library files into a subdirectory.
        #Therefore, we must append this subdirectory into PYTHONPATH.
        #Only do this for the python directories matching the following:
        foreach(pydir ${GR_TEST_PYTHON_DIRS})
            get_filename_component(NAME ${pydir} NAME)
            if(name MATCHES "^(lib|src)$")
                list(APPEND GR_TEST_PYTHON_DIRS ${pydir}/${CMAKE_BUILD_TYPE})
            endif()
        endforeach(pydir)
    endif(WIN32)

    gr_convert_quoted_string("${CMAKE_CURRENT_BINARY_DIR}" bindir)
    file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" srcdir)
    gr_convert_quoted_string("${GR_TEST_LIBRARY_DIRS}" libpath)
    #GR_CONVERT_QUOTED_STRING("${GR_TEST_PYTHON_DIRS}" pypath)
    # Keep the original path conversion for pypath - the above commented line breaks CI tests
    file(TO_NATIVE_PATH "${GR_TEST_PYTHON_DIRS}" pypath) #ok to use on dir list?

    # add test module directory to PYTHONPATH to allow CTest to find QA test modules.
    # We add it to the beginning of the list to use locally-built modules before installed ones.
    list(INSERT pypath 0 "${PROJECT_BINARY_DIR}/test_modules")

    set(environs "VOLK_GENERIC=1" "GR_DONT_LOAD_PREFS=1" "srcdir=${srcdir}"
                 "GR_CONF_CONTROLPORT_ON=False")
    list(APPEND environs ${GR_TEST_ENVIRONS})

    #http://www.cmake.org/pipermail/cmake/2009-May/029464.html
    #Replaced this add test + set environs code with the shell script generation.
    #Its nicer to be able to manually run the shell script to diagnose problems.
    #ADD_TEST(${ARGV})
    #SET_TESTS_PROPERTIES(${test_name} PROPERTIES ENVIRONMENT "${environs}")

    if(UNIX)
        set(LD_PATH_VAR "LD_LIBRARY_PATH")
        if(APPLE)
            set(LD_PATH_VAR "DYLD_LIBRARY_PATH")
        endif()

        set(binpath "${bindir}:\"$PATH\"")
        list(APPEND libpath "$${LD_PATH_VAR}")
        list(APPEND pypath "$PYTHONPATH")

        #replace list separator with the path separator
        string(REPLACE ";" ":" libpath "${libpath}")
        string(REPLACE ";" ":" pypath "${pypath}")
        list(APPEND environs "PATH=${binpath}" "${LD_PATH_VAR}=${libpath}"
             "PYTHONPATH=${pypath}")

        #generate a bat file that sets the environment and runs the test
        if(CMAKE_CROSSCOMPILING)
            set(SHELL "/bin/sh")
        else(CMAKE_CROSSCOMPILING)
            find_program(SHELL sh)
        endif(CMAKE_CROSSCOMPILING)
        set(sh_file ${CMAKE_CURRENT_BINARY_DIR}/${test_name}_test.sh)
        file(WRITE ${sh_file} "#!${SHELL}\n")
        if(NOT CMAKE_CROSSCOMPILING)
            #each line sets an environment variable
            foreach(environ ${environs})
                file(APPEND ${sh_file} "export ${environ}\n")
            endforeach(environ)
            #load the command to run with its arguments
        endif(NOT CMAKE_CROSSCOMPILING)
        foreach(arg ${ARGN})
            file(APPEND ${sh_file} "${arg} ")
        endforeach(arg)
        file(APPEND ${sh_file} "\n")

        #make the shell file executable
        execute_process(COMMAND chmod +x ${sh_file})

        add_test(${test_name} ${SHELL} ${test_name}_test.sh)
    endif(UNIX)

    if(WIN32)
        list(APPEND libpath ${DLL_PATHS} "%PATH%")
        list(APPEND pypath "%PYTHONPATH%")

        #replace list separator with the path separator (escaped)
        string(REPLACE ";" "\\;" libpath "${libpath}")
        string(REPLACE ";" "\\;" pypath "${pypath}")
        list(APPEND environs "PATH=${libpath}" "PYTHONPATH=${pypath}")

        #generate a bat file that sets the environment and runs the test
        set(bat_file ${CMAKE_CURRENT_BINARY_DIR}/${test_name}_test.bat)
        file(WRITE ${bat_file} "@echo off\n")
        #each line sets an environment variable
        foreach(environ ${environs})
            file(APPEND ${bat_file} "SET ${environ}\n")
        endforeach(environ)
        #load the command to run with its arguments
        foreach(arg ${ARGN})
            file(APPEND ${bat_file} "${arg} ")
        endforeach(arg)
        file(APPEND ${bat_file} "\n")

        add_test(${test_name} ${bat_file})
    endif(WIN32)

endfunction(GR_ADD_TEST)

########################################################################
# Add a C++ unit test and setup the environment for a unit test.
# Takes the same arguments as the ADD_TEST function.
#
# test_name -- An identifier for your test, for usage with ctest -R
# test_source -- Path to the .cc file
#
# Before calling set the following variables:
# GR_TEST_TARGET_DEPS  - built targets for the library path
########################################################################
function(GR_ADD_CPP_TEST test_name test_source)
    add_executable(${test_name} ${test_source})
    target_link_libraries(${test_name} ${GR_TEST_TARGET_DEPS} Boost::unit_test_framework)
    set_target_properties(${test_name} PROPERTIES COMPILE_DEFINITIONS
                                                  "BOOST_TEST_DYN_LINK;BOOST_TEST_MAIN")
    if(NOT CMAKE_CROSSCOMPILING)
        gr_add_test(${test_name} ${test_name})
    endif(NOT CMAKE_CROSSCOMPILING)
endfunction(GR_ADD_CPP_TEST)
