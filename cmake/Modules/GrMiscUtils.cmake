# Copyright 2010-2011,2014,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

if(DEFINED __INCLUDED_GR_MISC_UTILS_CMAKE)
    return()
endif()
set(__INCLUDED_GR_MISC_UTILS_CMAKE TRUE)

########################################################################
# Set the pre-processor definition if the condition is true.
#  - def the pre-processor definition to set and condition name
########################################################################
function(GR_ADD_COND_DEF def)
    if(${def})
        add_definitions(-D${def})
    endif(${def})
endfunction(GR_ADD_COND_DEF)

########################################################################
# Check for a header and conditionally set a compile define.
#  - hdr the relative path to the header file
#  - def the pre-processor definition to set
########################################################################
function(GR_CHECK_HDR_N_DEF hdr def)
    include(CheckIncludeFileCXX)
    check_include_file_cxx(${hdr} ${def})
    gr_add_cond_def(${def})
endfunction(GR_CHECK_HDR_N_DEF)

########################################################################
# Check if a compiler flag works and conditionally set a compile define.
#  - flag the compiler flag to check for
#  - have the variable to set with result
########################################################################
macro(GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE flag have)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(${flag} ${have})
    if(${have})
        string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_dup)
        if(${flag_dup} EQUAL -1)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}")
        endif(${flag_dup} EQUAL -1)
    endif(${have})
endmacro(GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE)

########################################################################
# Do standard things to the library target
# - set target properties
# - make install rules
# Also handle gnuradio custom naming conventions w/ extras mode.
########################################################################
function(GR_LIBRARY_FOO target)
    #set additional target properties
    set_target_properties(${target} PROPERTIES SOVERSION ${LIBVER})
    set_target_properties(${target} PROPERTIES VERSION ${VERSION})

    #install the generated files like so...
    install(
        TARGETS ${target}
        EXPORT ${target}-export
        LIBRARY DESTINATION ${GR_LIBRARY_DIR} # .so/.dylib file
        ARCHIVE DESTINATION ${GR_LIBRARY_DIR} # .lib file
        RUNTIME DESTINATION ${GR_RUNTIME_DIR} # .dll file
    )

    #install the exported target files
    install(
        EXPORT ${target}-export
        FILE ${target}Targets.cmake
        NAMESPACE gnuradio::
        DESTINATION ${GR_CMAKE_DIR})

    include(CMakePackageConfigHelpers)
    set(TARGET ${target})
    set(TARGET_DEPENDENCIES ${ARGN})

    configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/Modules/targetConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/Modules/${target}Config.cmake
        INSTALL_DESTINATION ${GR_CMAKE_DIR})
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/cmake/Modules/${target}Config.cmake
            DESTINATION ${GR_CMAKE_DIR})

endfunction(GR_LIBRARY_FOO)

########################################################################
# Create a dummy custom command that depends on other targets.
# Usage:
#   GR_GEN_TARGET_DEPS(unique_name target_deps <target1> <target2> ...)
#   ADD_CUSTOM_COMMAND(<the usual args> ${target_deps})
#
# Custom command can't depend on targets, but can depend on executables,
# and executables can depend on targets. So this is the process:
########################################################################
function(GR_GEN_TARGET_DEPS name var)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp.in "int main(void){return 0;}\n")
    execute_process(
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp.in
            ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp)
    add_executable(${name} ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp)
    if(ARGN)
        add_dependencies(${name} ${ARGN})
    endif(ARGN)

    if(CMAKE_CROSSCOMPILING)
        set(${var}
            "DEPENDS;${name}"
            PARENT_SCOPE) #can't call command when cross
    else()
        set(${var}
            "DEPENDS;${name};COMMAND;${name}"
            PARENT_SCOPE)
    endif()
endfunction(GR_GEN_TARGET_DEPS)

########################################################################
# Run GRCC to compile .grc files into .py files.
#
# Usage: GRCC(filename, directory)
#    - filenames: List of file name of .grc file
#    - directory: directory of built .py file - usually in
#                 ${CMAKE_CURRENT_BINARY_DIR}
#    - Sets PYFILES: output converted GRC file names to Python files.
########################################################################
function(GRCC)
    # Extract directory from list of args, remove it for the list of filenames.
    list(GET ARGV -1 directory)
    list(REMOVE_AT ARGV -1)
    set(filenames ${ARGV})
    file(MAKE_DIRECTORY ${directory})

    set(GRCC_COMMAND ${PROJECT_SOURCE_DIR}/gr-utils/python/grcc)

    # GRCC uses some stuff in grc and gnuradio-runtime, so we force
    # the known paths here
    list(APPEND PYTHONPATHS ${PROJECT_SOURCE_DIR}
         ${PROJECT_SOURCE_DIR}/gnuradio-runtime/python)

    if(WIN32)
        #SWIG generates the python library files into a subdirectory.
        #Therefore, we must append this subdirectory into PYTHONPATH.
        #Only do this for the python directories matching the following:
        foreach(pydir ${PYTHONPATHS})
            get_filename_component(NAME ${pydir} NAME)
            if(name MATCHES "^(lib|src)$")
                list(APPEND PYTHONPATHS ${pydir}/${CMAKE_BUILD_TYPE})
            endif()
        endforeach(pydir)
    endif(WIN32)

    file(TO_NATIVE_PATH "${PYTHONPATHS}" pypath)

    if(UNIX)
        list(APPEND pypath "$PYTHONPATH")
        string(REPLACE ";" ":" pypath "${pypath}")
        set(ENV{PYTHONPATH} ${pypath})
    endif(UNIX)

    if(WIN32)
        list(APPEND pypath "%PYTHONPATH%")
        string(REPLACE ";" "\\;" pypath "${pypath}")
        #list(APPEND environs "PYTHONPATH=${pypath}")
        set(ENV{PYTHONPATH} ${pypath})
    endif(WIN32)

    foreach(f ${filenames})
        execute_process(COMMAND ${GRCC_COMMAND} -d ${directory} ${f})
        string(REPLACE ".grc" ".py" pyfile "${f}")
        string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" pyfile
                       "${pyfile}")
        list(APPEND pyfiles ${pyfile})
    endforeach(f)

    set(PYFILES
        ${pyfiles}
        PARENT_SCOPE)
endfunction(GRCC)

########################################################################
# Check if HAVE_PTHREAD_SETSCHEDPARAM and HAVE_SCHED_SETSCHEDULER
#  should be defined
########################################################################
macro(GR_CHECK_LINUX_SCHED_AVAIL)
    set(CMAKE_REQUIRED_LIBRARIES -lpthread)
    check_cxx_source_compiles(
        "
        #include <pthread.h>
        int main(){
            pthread_t pthread;
            pthread_setschedparam(pthread,  0, 0);
            return 0;
        } "
        HAVE_PTHREAD_SETSCHEDPARAM)
    gr_add_cond_def(HAVE_PTHREAD_SETSCHEDPARAM)

    check_cxx_source_compiles(
        "
        #include <sched.h>
        int main(){
            pid_t pid;
            sched_setscheduler(pid, 0, 0);
            return 0;
        } "
        HAVE_SCHED_SETSCHEDULER)
    gr_add_cond_def(HAVE_SCHED_SETSCHEDULER)
    set(CMAKE_REQUIRED_LIBRARIES)
endmacro(GR_CHECK_LINUX_SCHED_AVAIL)
