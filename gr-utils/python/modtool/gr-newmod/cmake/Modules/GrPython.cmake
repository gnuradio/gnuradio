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

if(DEFINED __INCLUDED_GR_PYTHON_CMAKE)
    return()
endif()
set(__INCLUDED_GR_PYTHON_CMAKE TRUE)

########################################################################
# Setup the python interpreter:
# This allows the user to specify a specific interpreter,
# or finds the interpreter via the built-in cmake module.
########################################################################
#this allows the user to override PYTHON_EXECUTABLE
if(PYTHON_EXECUTABLE)

    set(PYTHONINTERP_FOUND TRUE)

#otherwise if not set, try to automatically find it
else(PYTHON_EXECUTABLE)

    #use the built-in find script
    find_package(PythonInterp 2)

    #and if that fails use the find program routine
    if(NOT PYTHONINTERP_FOUND)
        find_program(PYTHON_EXECUTABLE NAMES python python2 python2.7 python2.6 python2.5)
        if(PYTHON_EXECUTABLE)
            set(PYTHONINTERP_FOUND TRUE)
        endif(PYTHON_EXECUTABLE)
    endif(NOT PYTHONINTERP_FOUND)

endif(PYTHON_EXECUTABLE)

#make the path to the executable appear in the cmake gui
set(PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE} CACHE FILEPATH "python interpreter")

#make sure we can use -B with python (introduced in 2.6)
if(PYTHON_EXECUTABLE)
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -B -c ""
        OUTPUT_QUIET ERROR_QUIET
        RESULT_VARIABLE PYTHON_HAS_DASH_B_RESULT
    )
    if(PYTHON_HAS_DASH_B_RESULT EQUAL 0)
        set(PYTHON_DASH_B "-B")
    endif()
endif(PYTHON_EXECUTABLE)

########################################################################
# Check for the existence of a python module:
# - desc a string description of the check
# - mod the name of the module to import
# - cmd an additional command to run
# - have the result variable to set
########################################################################
macro(GR_PYTHON_CHECK_MODULE desc mod cmd have)
    message(STATUS "")
    message(STATUS "Python checking for ${desc}")
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -c "
#########################################
try: import ${mod}
except: exit(-1)
try: assert ${cmd}
except: exit(-1)
#########################################"
        RESULT_VARIABLE ${have}
    )
    if(${have} EQUAL 0)
        message(STATUS "Python checking for ${desc} - found")
        set(${have} TRUE)
    else(${have} EQUAL 0)
        message(STATUS "Python checking for ${desc} - not found")
        set(${have} FALSE)
    endif(${have} EQUAL 0)
endmacro(GR_PYTHON_CHECK_MODULE)

########################################################################
# Sets the python installation directory GR_PYTHON_DIR
########################################################################
execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "
from distutils import sysconfig
print sysconfig.get_python_lib(plat_specific=True, prefix='')
" OUTPUT_VARIABLE GR_PYTHON_DIR OUTPUT_STRIP_TRAILING_WHITESPACE
)
file(TO_CMAKE_PATH ${GR_PYTHON_DIR} GR_PYTHON_DIR)

########################################################################
# Create an always-built target with a unique name
# Usage: GR_UNIQUE_TARGET(<description> <dependencies list>)
########################################################################
function(GR_UNIQUE_TARGET desc)
    file(RELATIVE_PATH reldir ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import re, hashlib
unique = hashlib.md5('${reldir}${ARGN}').hexdigest()[:5]
print(re.sub('\\W', '_', '${desc} ${reldir} ' + unique))"
    OUTPUT_VARIABLE _target OUTPUT_STRIP_TRAILING_WHITESPACE)
    add_custom_target(${_target} ALL DEPENDS ${ARGN})
endfunction(GR_UNIQUE_TARGET)

########################################################################
# Install python sources (also builds and installs byte-compiled python)
########################################################################
function(GR_PYTHON_INSTALL)
    include(CMakeParseArgumentsCopy)
    CMAKE_PARSE_ARGUMENTS(GR_PYTHON_INSTALL "" "DESTINATION;COMPONENT" "FILES;PROGRAMS" ${ARGN})

    ####################################################################
    if(GR_PYTHON_INSTALL_FILES)
    ####################################################################
        install(${ARGN}) #installs regular python files

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

        #the command to generate the pyc files
        add_custom_command(
            DEPENDS ${pysrcfiles} OUTPUT ${pycfiles}
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
            COMPONENT ${GR_PYTHON_INSTALL_COMPONENT}
        )


    ####################################################################
    elseif(GR_PYTHON_INSTALL_PROGRAMS)
    ####################################################################
        file(TO_NATIVE_PATH ${PYTHON_EXECUTABLE} pyexe_native)

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
                \"open('${pyexefile}', 'w').write('\#!${pyexe_native}\\n'+open('${pyfile}').read())\"
                COMMENT "Shebangin ${pyfile_name}"
            )

            #on windows, python files need an extension to execute
            get_filename_component(pyfile_ext ${pyfile} EXT)
            if(WIN32 AND NOT pyfile_ext)
                set(pyfile_name "${pyfile_name}.py")
            endif()

            install(PROGRAMS ${pyexefile} RENAME ${pyfile_name}
                DESTINATION ${GR_PYTHON_INSTALL_DESTINATION}
                COMPONENT ${GR_PYTHON_INSTALL_COMPONENT}
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
srcs, gens = files[:len(files)/2], files[len(files)/2:]
for src, gen in zip(srcs, gens):
    py_compile.compile(file=src, cfile=gen, doraise=True)
")
