# Copyright 2010-2016,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

if(DEFINED __INCLUDED_GR_PYTHON_CMAKE)
    return()
endif()
set(__INCLUDED_GR_PYTHON_CMAKE TRUE)

########################################################################
# Setup the python interpreter:
# This allows the user to specify a specific interpreter,
# or finds the interpreter via the built-in cmake module.
########################################################################

if (PYTHON_EXECUTABLE)
    message(STATUS "User set python executable ${PYTHON_EXECUTABLE}")
    find_package(PythonInterp ${GR_PYTHON_MIN_VERSION} REQUIRED)
else (PYTHON_EXECUTABLE)
    message(STATUS "PYTHON_EXECUTABLE not set - using default python3")
    find_package(PythonInterp ${GR_PYTHON_MIN_VERSION} REQUIRED)
endif (PYTHON_EXECUTABLE)

find_package(PythonLibs ${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR} EXACT)

if (CMAKE_CROSSCOMPILING)
    set(QA_PYTHON_EXECUTABLE "/usr/bin/python3")
else (CMAKE_CROSSCOMPILING)
    set(QA_PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE})
endif(CMAKE_CROSSCOMPILING)

#make the path to the executable appear in the cmake gui
set(PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE} CACHE FILEPATH "python interpreter")
set(QA_PYTHON_EXECUTABLE ${QA_PYTHON_EXECUTABLE} CACHE FILEPATH "python interpreter for QA tests")

add_library(Python::Python INTERFACE IMPORTED)
# Need to handle special cases where both debug and release
# libraries are available (in form of debug;A;optimized;B) in PYTHON_LIBRARIES
if(PYTHON_LIBRARY_DEBUG AND PYTHON_LIBRARY_RELEASE)
    set_target_properties(Python::Python PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${PYTHON_INCLUDE_DIRS}"
      INTERFACE_LINK_LIBRARIES "$<$<NOT:$<CONFIG:Debug>>:${PYTHON_LIBRARY_RELEASE}>;$<$<CONFIG:Debug>:${PYTHON_LIBRARY_DEBUG}>"
      )
else()
    set_target_properties(Python::Python PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${PYTHON_INCLUDE_DIRS}"
      INTERFACE_LINK_LIBRARIES "${PYTHON_LIBRARIES}"
      )
endif()

# separate target when linking to make an extension module, which should not
# link explicitly to the python library on Unix-like platforms
add_library(Python::Module INTERFACE IMPORTED)
if(WIN32)
    # identical to Python::Python, cannot be an alias because Python::Python is imported
    # Need to handle special cases where both debug and release
    # libraries are available (in form of debug;A;optimized;B) in PYTHON_LIBRARIES
    if(PYTHON_LIBRARY_DEBUG AND PYTHON_LIBRARY_RELEASE)
        set_target_properties(Python::Module PROPERTIES
          INTERFACE_INCLUDE_DIRECTORIES "${PYTHON_INCLUDE_DIRS}"
          INTERFACE_LINK_LIBRARIES "$<$<NOT:$<CONFIG:Debug>>:${PYTHON_LIBRARY_RELEASE}>;$<$<CONFIG:Debug>:${PYTHON_LIBRARY_DEBUG}>"
        )
    else()
        set_target_properties(Python::Module PROPERTIES
          INTERFACE_INCLUDE_DIRECTORIES "${PYTHON_INCLUDE_DIRS}"
          INTERFACE_LINK_LIBRARIES "${PYTHON_LIBRARIES}"
        )
    endif()
else()
    set_target_properties(Python::Module PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${PYTHON_INCLUDE_DIRS}"
    )
    if(APPLE)
        set_target_properties(Python::Module PROPERTIES
            INTERFACE_LINK_OPTIONS "LINKER:-undefined,dynamic_lookup"
        )
    endif(APPLE)
endif(WIN32)


########################################################################
# Check for the existence of a python module:
# - desc a string description of the check
# - mod the name of the module to import
# - cmd an additional command to run
# - have the result variable to set
########################################################################
macro(GR_PYTHON_CHECK_MODULE_RAW desc python_code have)
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -c "${python_code}"
        OUTPUT_QUIET ERROR_QUIET
        RESULT_VARIABLE return_code
    )
    if(return_code EQUAL 0)
        message(STATUS "Python checking for ${desc} - found")
        set(${have} TRUE)
    else()
        message(STATUS "Python checking for ${desc} - not found")
        set(${have} FALSE)
    endif()
endmacro(GR_PYTHON_CHECK_MODULE_RAW)

macro(GR_PYTHON_CHECK_MODULE desc mod cmd have)
    GR_PYTHON_CHECK_MODULE_RAW(
        "${desc}" "
#########################################
from distutils.version import LooseVersion
try:
    import ${mod}
    assert ${cmd}
except (ImportError, AssertionError): exit(-1)
except: pass
#########################################"
    "${have}")
endmacro(GR_PYTHON_CHECK_MODULE)

########################################################################
# Sets the python installation directory GR_PYTHON_DIR
########################################################################
if(NOT DEFINED GR_PYTHON_DIR)
execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "
from distutils import sysconfig
print(sysconfig.get_python_lib(plat_specific=True, prefix=''))
" OUTPUT_VARIABLE GR_PYTHON_DIR OUTPUT_STRIP_TRAILING_WHITESPACE
)
endif()
file(TO_CMAKE_PATH ${GR_PYTHON_DIR} GR_PYTHON_DIR)

########################################################################
# Sets the python relative installation directory GR_PYTHON_RELATIVE
########################################################################
if(NOT DEFINED GR_PYTHON_RELATIVE)
execute_process(COMMAND "${PYTHON_EXECUTABLE}" -c "
from distutils import sysconfig as sc
print(sc.get_python_lib(prefix='', plat_specific=True))
"
  OUTPUT_VARIABLE GR_PYTHON_RELATIVE  OUTPUT_STRIP_TRAILING_WHITESPACE
)
endif()


########################################################################
# Create an always-built target with a unique name
# Usage: GR_UNIQUE_TARGET(<description> <dependencies list>)
########################################################################
function(GR_UNIQUE_TARGET desc)
    file(RELATIVE_PATH reldir ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import re, hashlib
unique = hashlib.md5(b'${reldir}${ARGN}').hexdigest()[:5]
print(re.sub('\\W', '_', r'${desc} ${reldir} ' + unique))"
    OUTPUT_VARIABLE _target OUTPUT_STRIP_TRAILING_WHITESPACE)
    add_custom_target(${_target} ALL DEPENDS ${ARGN})
endfunction(GR_UNIQUE_TARGET)

########################################################################
# Install python sources (also builds and installs byte-compiled python)
########################################################################
function(GR_PYTHON_INSTALL)
    include(CMakeParseArgumentsCopy)
    CMAKE_PARSE_ARGUMENTS(GR_PYTHON_INSTALL "" "DESTINATION" "FILES;PROGRAMS;DIRECTORY;DEPENDS" ${ARGN})

    ####################################################################
    if(GR_PYTHON_INSTALL_FILES)
    ####################################################################
    install(
      FILES ${GR_PYTHON_INSTALL_FILES}
      DESTINATION ${GR_PYTHON_INSTALL_DESTINATION}
    )

        #create a list of all generated files
        unset(pysrcfiles)
        unset(pycfiles)
        unset(pyofiles)
        foreach(pyfile ${GR_PYTHON_INSTALL_FILES})
            get_filename_component(pyfile ${pyfile} ABSOLUTE)
            list(APPEND pysrcfiles ${pyfile})

            #determine if this file is in the source or binary directory
            file(RELATIVE_PATH source_rel_path ${CMAKE_CURRENT_SOURCE_DIR} ${pyfile})
            string(LENGTH "${source_rel_path}" source_rel_path_len)
            file(RELATIVE_PATH binary_rel_path ${CMAKE_CURRENT_BINARY_DIR} ${pyfile})
            string(LENGTH "${binary_rel_path}" binary_rel_path_len)

            #and set the generated path appropriately
            if(${source_rel_path_len} GREATER ${binary_rel_path_len})
                set(pygenfile ${CMAKE_CURRENT_BINARY_DIR}/${binary_rel_path})
            else()
                set(pygenfile ${CMAKE_CURRENT_BINARY_DIR}/${source_rel_path})
            endif()
            list(APPEND pycfiles ${pygenfile}c)
            list(APPEND pyofiles ${pygenfile}o)

            #ensure generation path exists
            get_filename_component(pygen_path ${pygenfile} PATH)
            file(MAKE_DIRECTORY ${pygen_path})

        endforeach(pyfile)

        if(NOT GR_PYTHON_INSTALL_DEPENDS)
          set(GR_PYTHON_INSTALL_DEPENDS ${pysrcfiles})
        endif()


        #the command to generate the pyc files
        add_custom_command(
            DEPENDS ${GR_PYTHON_INSTALL_DEPENDS} OUTPUT ${pycfiles}
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_BINARY_DIR}/python_compile_helper.py ${pysrcfiles} ${pycfiles}
        )

        #the command to generate the pyo files
        add_custom_command(
            DEPENDS ${pysrcfiles} OUTPUT ${pyofiles}
            COMMAND ${PYTHON_EXECUTABLE} -O ${CMAKE_BINARY_DIR}/python_compile_helper.py ${pysrcfiles} ${pyofiles}
        )

        #create install rule and add generated files to target list
        set(python_install_gen_targets ${pycfiles} ${pyofiles})
        install(FILES ${python_install_gen_targets}
            DESTINATION ${GR_PYTHON_INSTALL_DESTINATION}
        )

    ####################################################################
    elseif(GR_PYTHON_INSTALL_DIRECTORY)
    ####################################################################
    install(
      DIRECTORY ${GR_PYTHON_INSTALL_DIRECTORY}
      DESTINATION ${GR_PYTHON_INSTALL_DESTINATION}
    )


        # collect all python files in given directories
        # #############################################
        unset(pysrcfiles)
        foreach(pydir ${GR_PYTHON_INSTALL_DIRECTORY})
            file(GLOB_RECURSE pysrcfiles_tmp "${pydir}/*.py")
            list(APPEND pysrcfiles ${pysrcfiles_tmp})
        endforeach(pydir)

        # build target lists
        # ##################
        unset(pycfiles)  # pyc targets
        unset(pyofiles)  # pyo targets
        unset(pygen_paths)  # all paths of py[oc] targets
        foreach(pyfile ${pysrcfiles})
            # determine if this file is in the source or binary directory
            file(RELATIVE_PATH source_rel_path ${CMAKE_CURRENT_SOURCE_DIR} ${pyfile})
            string(LENGTH "${source_rel_path}" source_rel_path_len)
            file(RELATIVE_PATH binary_rel_path ${CMAKE_CURRENT_BINARY_DIR} ${pyfile})
            string(LENGTH "${binary_rel_path}" binary_rel_path_len)

            # and set the generated path appropriately
            if(${source_rel_path_len} GREATER ${binary_rel_path_len})
                set(pygenfile ${CMAKE_CURRENT_BINARY_DIR}/${binary_rel_path})
            else()
                set(pygenfile ${CMAKE_CURRENT_BINARY_DIR}/${source_rel_path})
            endif()
            list(APPEND pycfiles "${pygenfile}c")
            list(APPEND pyofiles "${pygenfile}o")

            get_filename_component(pygen_path "${pygenfile}" DIRECTORY)
            list(APPEND pygen_paths "${pygen_path}")
            file(MAKE_DIRECTORY "${pygen_path}")
        endforeach(pyfile)
        list(REMOVE_DUPLICATES pygen_paths)
        list(SORT pygen_paths)

        # generate the py[oc] files
        # #########################
        add_custom_command(
            DEPENDS ${pysrcfiles} OUTPUT ${pycfiles}
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_BINARY_DIR}/python_compile_helper.py ${pysrcfiles} ${pycfiles}
        )
        add_custom_command(
            DEPENDS ${pysrcfiles} OUTPUT ${pyofiles}
            COMMAND ${PYTHON_EXECUTABLE} -O ${CMAKE_BINARY_DIR}/python_compile_helper.py ${pysrcfiles} ${pyofiles}
        )
        set(python_install_gen_targets ${pycfiles} ${pyofiles})

        # per-directory install rules
        # ###########################
        foreach(pygen_path ${pygen_paths})
            # find all targets in that directory (no "list(FILTER ...)")
            unset(pygen_path_targets)
            foreach(pyget_target ${python_install_gen_targets})
                get_filename_component(pyget_target_path "${pyget_target}" PATH)
                if(pygen_path STREQUAL pyget_target_path)
                    list(APPEND pygen_path_targets "${pyget_target}")
                endif()
            endforeach(pyget_target)

            # install relative to current binary dir
            file(RELATIVE_PATH pygen_path_rel "${CMAKE_CURRENT_BINARY_DIR}" "${pygen_path}")
            list(SORT pygen_path_targets)
            install(
                FILES ${pygen_path_targets}
                DESTINATION "${GR_PYTHON_INSTALL_DESTINATION}/${pygen_path_rel}"
            )
        endforeach(pygen_path)

    ####################################################################
    elseif(GR_PYTHON_INSTALL_PROGRAMS)
    ####################################################################
        file(TO_NATIVE_PATH ${PYTHON_EXECUTABLE} pyexe_native)

        if (CMAKE_CROSSCOMPILING)
           set(pyexe_native "/usr/bin/env python")
        endif()

        foreach(pyfile ${GR_PYTHON_INSTALL_PROGRAMS})
            get_filename_component(pyfile_name ${pyfile} NAME)
            get_filename_component(pyfile ${pyfile} ABSOLUTE)
            string(REPLACE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" pyexefile "${pyfile}.exe")
            list(APPEND python_install_gen_targets ${pyexefile})

            get_filename_component(pyexefile_path ${pyexefile} PATH)
            file(MAKE_DIRECTORY ${pyexefile_path})

            add_custom_command(
                OUTPUT ${pyexefile} DEPENDS ${pyfile}
                COMMAND ${PYTHON_EXECUTABLE} -c
                "import re; R=re.compile('^\#!.*$\\n',flags=re.MULTILINE); open(r'${pyexefile}','w').write(r'\#!${pyexe_native}'+'\\n'+R.sub('',open(r'${pyfile}','r').read()))"
                COMMENT "Shebangin ${pyfile_name}"
                VERBATIM
            )

            #on windows, python files need an extension to execute
            get_filename_component(pyfile_ext ${pyfile} EXT)
            if(WIN32 AND NOT pyfile_ext)
                set(pyfile_name "${pyfile_name}.py")
            endif()

            install(PROGRAMS ${pyexefile} RENAME ${pyfile_name}
                DESTINATION ${GR_PYTHON_INSTALL_DESTINATION}
            )
        endforeach(pyfile)

    endif()

    GR_UNIQUE_TARGET("pygen" ${python_install_gen_targets})

endfunction(GR_PYTHON_INSTALL)

########################################################################
# Write the python helper script that generates byte code files
########################################################################
file(WRITE ${CMAKE_BINARY_DIR}/python_compile_helper.py "
import sys, py_compile
files = sys.argv[1:]
srcs, gens = files[:len(files)//2], files[len(files)//2:]
for src, gen in zip(srcs, gens):
    py_compile.compile(file=src, cfile=gen, doraise=True)
")
