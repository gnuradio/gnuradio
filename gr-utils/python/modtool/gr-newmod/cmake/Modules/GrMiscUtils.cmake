# Copyright 2010-2011,2014 Free Software Foundation, Inc.
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

if(DEFINED __INCLUDED_GR_MISC_UTILS_CMAKE)
    return()
endif()
set(__INCLUDED_GR_MISC_UTILS_CMAKE TRUE)

########################################################################
# Set global variable macro.
# Used for subdirectories to export settings.
# Example: include and library paths.
########################################################################
function(GR_SET_GLOBAL var)
    set(${var} ${ARGN} CACHE INTERNAL "" FORCE)
endfunction(GR_SET_GLOBAL)

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
    CHECK_INCLUDE_FILE_CXX(${hdr} ${def})
    GR_ADD_COND_DEF(${def})
endfunction(GR_CHECK_HDR_N_DEF)

########################################################################
# Include subdirectory macro.
# Sets the CMake directory variables,
# includes the subdirectory CMakeLists.txt,
# resets the CMake directory variables.
#
# This macro includes subdirectories rather than adding them
# so that the subdirectory can affect variables in the level above.
# This provides a work-around for the lack of convenience libraries.
# This way a subdirectory can append to the list of library sources.
########################################################################
macro(GR_INCLUDE_SUBDIRECTORY subdir)
    #insert the current directories on the front of the list
    list(INSERT _cmake_source_dirs 0 ${CMAKE_CURRENT_SOURCE_DIR})
    list(INSERT _cmake_binary_dirs 0 ${CMAKE_CURRENT_BINARY_DIR})

    #set the current directories to the names of the subdirs
    set(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${subdir})
    set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${subdir})

    #include the subdirectory CMakeLists to run it
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt)

    #reset the value of the current directories
    list(GET _cmake_source_dirs 0 CMAKE_CURRENT_SOURCE_DIR)
    list(GET _cmake_binary_dirs 0 CMAKE_CURRENT_BINARY_DIR)

    #pop the subdir names of the front of the list
    list(REMOVE_AT _cmake_source_dirs 0)
    list(REMOVE_AT _cmake_binary_dirs 0)
endmacro(GR_INCLUDE_SUBDIRECTORY)

########################################################################
# Check if a compiler flag works and conditionally set a compile define.
#  - flag the compiler flag to check for
#  - have the variable to set with result
########################################################################
macro(GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE flag have)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG(${flag} ${have})
    if(${have})
      if(${CMAKE_VERSION} VERSION_GREATER "2.8.4")
        STRING(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_dup)
        if(${flag_dup} EQUAL -1)
          set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
          set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${flag}")
        endif(${flag_dup} EQUAL -1)
      endif(${CMAKE_VERSION} VERSION_GREATER "2.8.4")
    endif(${have})
endmacro(GR_ADD_CXX_COMPILER_FLAG_IF_AVAILABLE)

########################################################################
# Generates the .la libtool file
# This appears to generate libtool files that cannot be used by auto*.
# Usage GR_LIBTOOL(TARGET [target] DESTINATION [dest])
# Notice: there is not COMPONENT option, these will not get distributed.
########################################################################
function(GR_LIBTOOL)
    if(NOT DEFINED GENERATE_LIBTOOL)
        set(GENERATE_LIBTOOL OFF) #disabled by default
    endif()

    if(GENERATE_LIBTOOL)
        include(CMakeParseArgumentsCopy)
        CMAKE_PARSE_ARGUMENTS(GR_LIBTOOL "" "TARGET;DESTINATION" "" ${ARGN})

        find_program(LIBTOOL libtool)
        if(LIBTOOL)
            include(CMakeMacroLibtoolFile)
            CREATE_LIBTOOL_FILE(${GR_LIBTOOL_TARGET} /${GR_LIBTOOL_DESTINATION})
        endif(LIBTOOL)
    endif(GENERATE_LIBTOOL)

endfunction(GR_LIBTOOL)

########################################################################
# Do standard things to the library target
# - set target properties
# - make install rules
# Also handle gnuradio custom naming conventions w/ extras mode.
########################################################################
function(GR_LIBRARY_FOO target)
    #parse the arguments for component names
    include(CMakeParseArgumentsCopy)
    CMAKE_PARSE_ARGUMENTS(GR_LIBRARY "" "RUNTIME_COMPONENT;DEVEL_COMPONENT" "" ${ARGN})

    #set additional target properties
    set_target_properties(${target} PROPERTIES SOVERSION ${LIBVER})

    #install the generated files like so...
    install(TARGETS ${target}
        LIBRARY DESTINATION ${GR_LIBRARY_DIR} COMPONENT ${GR_LIBRARY_RUNTIME_COMPONENT} # .so/.dylib file
        ARCHIVE DESTINATION ${GR_LIBRARY_DIR} COMPONENT ${GR_LIBRARY_DEVEL_COMPONENT}   # .lib file
        RUNTIME DESTINATION ${GR_RUNTIME_DIR} COMPONENT ${GR_LIBRARY_RUNTIME_COMPONENT} # .dll file
    )

    #extras mode enabled automatically on linux
    if(NOT DEFINED LIBRARY_EXTRAS)
        set(LIBRARY_EXTRAS ${LINUX})
    endif()

    #special extras mode to enable alternative naming conventions
    if(LIBRARY_EXTRAS)

        #create .la file before changing props
        GR_LIBTOOL(TARGET ${target} DESTINATION ${GR_LIBRARY_DIR})

        #give the library a special name with ultra-zero soversion
        set_target_properties(${target} PROPERTIES OUTPUT_NAME ${target}-${LIBVER} SOVERSION "0.0.0")
        set(target_name lib${target}-${LIBVER}.so.0.0.0)

        #custom command to generate symlinks
        add_custom_command(
            TARGET ${target}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink ${target_name} ${CMAKE_CURRENT_BINARY_DIR}/lib${target}.so
            COMMAND ${CMAKE_COMMAND} -E create_symlink ${target_name} ${CMAKE_CURRENT_BINARY_DIR}/lib${target}-${LIBVER}.so.0
            COMMAND ${CMAKE_COMMAND} -E touch ${target_name} #so the symlinks point to something valid so cmake 2.6 will install
        )

        #and install the extra symlinks
        install(
            FILES
            ${CMAKE_CURRENT_BINARY_DIR}/lib${target}.so
            ${CMAKE_CURRENT_BINARY_DIR}/lib${target}-${LIBVER}.so.0
            DESTINATION ${GR_LIBRARY_DIR} COMPONENT ${GR_LIBRARY_RUNTIME_COMPONENT}
        )

    endif(LIBRARY_EXTRAS)
endfunction(GR_LIBRARY_FOO)

########################################################################
# Create a dummy custom command that depends on other targets.
# Usage:
#   GR_GEN_TARGET_DEPS(unique_name target_deps <target1> <target2> ...)
#   ADD_CUSTOM_COMMAND(<the usual args> ${target_deps})
#
# Custom command cant depend on targets, but can depend on executables,
# and executables can depend on targets. So this is the process:
########################################################################
function(GR_GEN_TARGET_DEPS name var)
    file(
        WRITE ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp.in
        "int main(void){return 0;}\n"
    )
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp.in
        ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp
    )
    add_executable(${name} ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp)
    if(ARGN)
        add_dependencies(${name} ${ARGN})
    endif(ARGN)

    if(CMAKE_CROSSCOMPILING)
        set(${var} "DEPENDS;${name}" PARENT_SCOPE) #cant call command when cross
    else()
        set(${var} "DEPENDS;${name};COMMAND;${name}" PARENT_SCOPE)
    endif()
endfunction(GR_GEN_TARGET_DEPS)

########################################################################
# Control use of gr_logger
# Usage:
#   GR_LOGGING()
#
# Will set ENABLE_GR_LOG to 1 by default.
# Can manually set with -DENABLE_GR_LOG=0|1
########################################################################
function(GR_LOGGING)
  find_package(Log4cpp)

  OPTION(ENABLE_GR_LOG "Use gr_logger" ON)
  if(ENABLE_GR_LOG)
    # If gr_logger is enabled, make it usable
    add_definitions( -DENABLE_GR_LOG )

    # also test LOG4CPP; if we have it, use this version of the logger
    # otherwise, default to the stdout/stderr model.
    if(LOG4CPP_FOUND)
      SET(HAVE_LOG4CPP True CACHE INTERNAL "" FORCE)
      add_definitions( -DHAVE_LOG4CPP )
    else(not LOG4CPP_FOUND)
      SET(HAVE_LOG4CPP False CACHE INTERNAL "" FORCE)
      SET(LOG4CPP_INCLUDE_DIRS "" CACHE INTERNAL "" FORCE)
      SET(LOG4CPP_LIBRARY_DIRS "" CACHE INTERNAL "" FORCE)
      SET(LOG4CPP_LIBRARIES "" CACHE INTERNAL "" FORCE)
    endif(LOG4CPP_FOUND)

    SET(ENABLE_GR_LOG ${ENABLE_GR_LOG} CACHE INTERNAL "" FORCE)

  else(ENABLE_GR_LOG)
    SET(HAVE_LOG4CPP False CACHE INTERNAL "" FORCE)
    SET(LOG4CPP_INCLUDE_DIRS "" CACHE INTERNAL "" FORCE)
    SET(LOG4CPP_LIBRARY_DIRS "" CACHE INTERNAL "" FORCE)
    SET(LOG4CPP_LIBRARIES "" CACHE INTERNAL "" FORCE)
  endif(ENABLE_GR_LOG)

  message(STATUS "ENABLE_GR_LOG set to ${ENABLE_GR_LOG}.")
  message(STATUS "HAVE_LOG4CPP set to ${HAVE_LOG4CPP}.")
  message(STATUS "LOG4CPP_LIBRARIES set to ${LOG4CPP_LIBRARIES}.")

endfunction(GR_LOGGING)

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

  SET(GRCC_COMMAND ${CMAKE_SOURCE_DIR}/gr-utils/python/grcc)

  # GRCC uses some stuff in grc and gnuradio-runtime, so we force
  # the known paths here
  list(APPEND PYTHONPATHS
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/gnuradio-runtime/python
    ${CMAKE_SOURCE_DIR}/gnuradio-runtime/lib/swig
    ${CMAKE_BINARY_DIR}/gnuradio-runtime/lib/swig
    )

  if(WIN32)
    #SWIG generates the python library files into a subdirectory.
    #Therefore, we must append this subdirectory into PYTHONPATH.
    #Only do this for the python directories matching the following:
    foreach(pydir ${PYTHONPATHS})
      get_filename_component(name ${pydir} NAME)
      if(name MATCHES "^(swig|lib|src)$")
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
    execute_process(
      COMMAND ${GRCC_COMMAND} -d ${directory} ${f}
      )
    string(REPLACE ".grc" ".py" pyfile "${f}")
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" pyfile "${pyfile}")
    list(APPEND pyfiles ${pyfile})
  endforeach(f)

  set(PYFILES ${pyfiles} PARENT_SCOPE)
endfunction(GRCC)

########################################################################
# Check if HAVE_PTHREAD_SETSCHEDPARAM and HAVE_SCHED_SETSCHEDULER
#  should be defined
########################################################################
macro(GR_CHECK_LINUX_SCHED_AVAIL)
set(CMAKE_REQUIRED_LIBRARIES -lpthread)
    CHECK_CXX_SOURCE_COMPILES("
        #include <pthread.h>
        int main(){
            pthread_t pthread;
            pthread_setschedparam(pthread,  0, 0);
            return 0;
        } " HAVE_PTHREAD_SETSCHEDPARAM
    )
    GR_ADD_COND_DEF(HAVE_PTHREAD_SETSCHEDPARAM)

    CHECK_CXX_SOURCE_COMPILES("
        #include <sched.h>
        int main(){
            pid_t pid;
            sched_setscheduler(pid, 0, 0);
            return 0;
        } " HAVE_SCHED_SETSCHEDULER
    )
    GR_ADD_COND_DEF(HAVE_SCHED_SETSCHEDULER)
endmacro(GR_CHECK_LINUX_SCHED_AVAIL)

########################################################################
# Macros to generate source and header files from template
########################################################################
macro(GR_EXPAND_X_H component root)

  include(GrPython)

  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
"#!${PYTHON_EXECUTABLE}

import sys, os, re
sys.path.append('${GR_RUNTIME_PYTHONPATH}')
sys.path.append('${CMAKE_SOURCE_DIR}/python')
os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}'
os.chdir('${CMAKE_CURRENT_BINARY_DIR}')

if __name__ == '__main__':
    import build_utils
    root, inp = sys.argv[1:3]
    for sig in sys.argv[3:]:
        name = re.sub ('X+', sig, root)
        d = build_utils.standard_dict2(name, sig, '${component}')
        build_utils.expand_template(d, inp)
")

  #make a list of all the generated headers
  unset(expanded_files_h)
  foreach(sig ${ARGN})
    string(REGEX REPLACE "X+" ${sig} name ${root})
    list(APPEND expanded_files_h ${CMAKE_CURRENT_BINARY_DIR}/${name}.h)
  endforeach(sig)
  unset(name)

  #create a command to generate the headers
  add_custom_command(
    OUTPUT ${expanded_files_h}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}.h.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
    ${root} ${root}.h.t ${ARGN}
  )

  #install rules for the generated headers
  list(APPEND generated_includes ${expanded_files_h})

endmacro(GR_EXPAND_X_H)

macro(GR_EXPAND_X_CC_H component root)

  include(GrPython)

  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
"#!${PYTHON_EXECUTABLE}

import sys, os, re
sys.path.append('${GR_RUNTIME_PYTHONPATH}')
sys.path.append('${CMAKE_SOURCE_DIR}/python')
os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}'
os.chdir('${CMAKE_CURRENT_BINARY_DIR}')

if __name__ == '__main__':
    import build_utils
    root, inp = sys.argv[1:3]
    for sig in sys.argv[3:]:
        name = re.sub ('X+', sig, root)
        d = build_utils.standard_impl_dict2(name, sig, '${component}')
        build_utils.expand_template(d, inp)
")

  #make a list of all the generated files
  unset(expanded_files_cc)
  unset(expanded_files_h)
  foreach(sig ${ARGN})
    string(REGEX REPLACE "X+" ${sig} name ${root})
    list(APPEND expanded_files_cc ${CMAKE_CURRENT_BINARY_DIR}/${name}.cc)
    list(APPEND expanded_files_h  ${CMAKE_CURRENT_BINARY_DIR}/${name}.h)
  endforeach(sig)
  unset(name)

  #create a command to generate the source files
  add_custom_command(
    OUTPUT ${expanded_files_cc}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}.cc.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
    ${root} ${root}.cc.t ${ARGN}
  )

  #create a command to generate the header files
  add_custom_command(
    OUTPUT ${expanded_files_h}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}.h.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
    ${root} ${root}.h.t ${ARGN}
  )

  #make source files depends on headers to force generation
  set_source_files_properties(${expanded_files_cc}
    PROPERTIES OBJECT_DEPENDS "${expanded_files_h}"
  )

  #install rules for the generated files
  list(APPEND generated_sources ${expanded_files_cc})
  list(APPEND generated_headers ${expanded_files_h})

endmacro(GR_EXPAND_X_CC_H)

macro(GR_EXPAND_X_CC_H_IMPL component root)

  include(GrPython)

  file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
"#!${PYTHON_EXECUTABLE}

import sys, os, re
sys.path.append('${GR_RUNTIME_PYTHONPATH}')
sys.path.append('${CMAKE_SOURCE_DIR}/python')
os.environ['srcdir'] = '${CMAKE_CURRENT_SOURCE_DIR}'
os.chdir('${CMAKE_CURRENT_BINARY_DIR}')

if __name__ == '__main__':
    import build_utils
    root, inp = sys.argv[1:3]
    for sig in sys.argv[3:]:
        name = re.sub ('X+', sig, root)
        d = build_utils.standard_dict(name, sig, '${component}')
        build_utils.expand_template(d, inp, '_impl')
")

  #make a list of all the generated files
  unset(expanded_files_cc_impl)
  unset(expanded_files_h_impl)
  unset(expanded_files_h)
  foreach(sig ${ARGN})
    string(REGEX REPLACE "X+" ${sig} name ${root})
    list(APPEND expanded_files_cc_impl ${CMAKE_CURRENT_BINARY_DIR}/${name}_impl.cc)
    list(APPEND expanded_files_h_impl ${CMAKE_CURRENT_BINARY_DIR}/${name}_impl.h)
    list(APPEND expanded_files_h ${CMAKE_CURRENT_BINARY_DIR}/../include/gnuradio/${component}/${name}.h)
  endforeach(sig)
  unset(name)

  #create a command to generate the _impl.cc files
  add_custom_command(
    OUTPUT ${expanded_files_cc_impl}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}_impl.cc.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
    ${root} ${root}_impl.cc.t ${ARGN}
  )

  #create a command to generate the _impl.h files
  add_custom_command(
    OUTPUT ${expanded_files_h_impl}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${root}_impl.h.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
    ${CMAKE_CURRENT_BINARY_DIR}/generate_helper.py
    ${root} ${root}_impl.h.t ${ARGN}
  )

  #make _impl.cc source files depend on _impl.h to force generation
  set_source_files_properties(${expanded_files_cc_impl}
    PROPERTIES OBJECT_DEPENDS "${expanded_files_h_impl}"
  )

  #make _impl.h source files depend on headers to force generation
  set_source_files_properties(${expanded_files_h_impl}
    PROPERTIES OBJECT_DEPENDS "${expanded_files_h}"
  )

  #install rules for the generated files
  list(APPEND generated_sources ${expanded_files_cc_impl})
  list(APPEND generated_headers ${expanded_files_h_impl})

endmacro(GR_EXPAND_X_CC_H_IMPL)
