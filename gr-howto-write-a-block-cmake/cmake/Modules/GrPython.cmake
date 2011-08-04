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

IF(DEFINED __INCLUDED_GR_PYTHON_CMAKE)
    RETURN()
ENDIF()
SET(__INCLUDED_GR_PYTHON_CMAKE TRUE)

########################################################################
# Setup the python interpreter:
# This allows the user to specify a specific interpreter,
# or finds the interpreter via the built-in cmake module.
########################################################################
#this allows the user to override PYTHON_EXECUTABLE
IF(PYTHON_EXECUTABLE)

    SET(PYTHONINTERP_FOUND TRUE)

#otherwise if not set, try to automatically find it
ELSE(PYTHON_EXECUTABLE)

    #use the built-in find script
    FIND_PACKAGE(PythonInterp)

    #and if that fails use the find program routine
    IF(NOT PYTHONINTERP_FOUND)
        FIND_PROGRAM(PYTHON_EXECUTABLE NAMES python python2.7 python2.6 python2.5)
        IF(PYTHON_EXECUTABLE)
            SET(PYTHONINTERP_FOUND TRUE)
        ENDIF(PYTHON_EXECUTABLE)
    ENDIF(NOT PYTHONINTERP_FOUND)

ENDIF(PYTHON_EXECUTABLE)

#make the path to the executable appear in the cmake gui
SET(PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE} CACHE FILEPATH "python interpreter")

########################################################################
# Check for the existence of a python module:
# - desc a string description of the check
# - mod the name of the module to import
# - cmd an additional command to run
# - have the result variable to set
########################################################################
MACRO(GR_PYTHON_CHECK_MODULE desc mod cmd have)
    MESSAGE(STATUS "")
    MESSAGE(STATUS "Python checking for ${desc}")
    EXECUTE_PROCESS(
        COMMAND ${PYTHON_EXECUTABLE} -c "
#########################################
try: import ${mod}
except: exit(-1)
try: assert ${cmd}
except: exit(-1)
#########################################"
        RESULT_VARIABLE ${have}
    )
    IF(${have} EQUAL 0)
        MESSAGE(STATUS "Python checking for ${desc} - found")
        SET(${have} TRUE)
    ELSE(${have} EQUAL 0)
        MESSAGE(STATUS "Python checking for ${desc} - not found")
        SET(${have} FALSE)
    ENDIF(${have} EQUAL 0)
ENDMACRO(GR_PYTHON_CHECK_MODULE)

########################################################################
# Sets the python installation directory GR_PYTHON_DIR
########################################################################
EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -c "
from distutils import sysconfig
print sysconfig.get_python_lib(plat_specific=True, prefix='')
" OUTPUT_VARIABLE GR_PYTHON_DIR OUTPUT_STRIP_TRAILING_WHITESPACE
)
FILE(TO_CMAKE_PATH ${GR_PYTHON_DIR} GR_PYTHON_DIR)

########################################################################
# Create an always-built target with a unique name
# Usage: GR_UNIQUE_TARGET(<description> <dependencies list>)
########################################################################
FUNCTION(GR_UNIQUE_TARGET desc)
    FILE(RELATIVE_PATH reldir ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})
    EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} -c "import re, hashlib
unique = hashlib.md5('${reldir}${ARGN}').hexdigest()[:5]
print(re.sub('\\W', '_', '${desc} ${reldir} ' + unique))"
    OUTPUT_VARIABLE _target OUTPUT_STRIP_TRAILING_WHITESPACE)
    ADD_CUSTOM_TARGET(${_target} ALL DEPENDS ${ARGN})
ENDFUNCTION(GR_UNIQUE_TARGET)

########################################################################
# Install python sources (also builds and installs byte-compiled python)
########################################################################
FUNCTION(GR_PYTHON_INSTALL)
    INCLUDE(CMakeParseArgumentsCopy)
    CMAKE_PARSE_ARGUMENTS(GR_PYTHON_INSTALL "" "DESTINATION;COMPONENT" "FILES;PROGRAMS" ${ARGN})

    ####################################################################
    IF(GR_PYTHON_INSTALL_FILES)
    ####################################################################
        INSTALL(${ARGN}) #installs regular python files

        FOREACH(pyfile ${GR_PYTHON_INSTALL_FILES})
            GET_FILENAME_COMPONENT(pyfile_name ${pyfile} NAME)
            GET_FILENAME_COMPONENT(pyfile ${pyfile} ABSOLUTE)
            STRING(REPLACE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" pycfile "${pyfile}c")
            LIST(APPEND python_install_gen_targets ${pycfile})

            GET_FILENAME_COMPONENT(pycfile_path ${pycfile} PATH)
            FILE(MAKE_DIRECTORY ${pycfile_path})

            #create a command to generate the byte-compiled pyc file
            ADD_CUSTOM_COMMAND(
                OUTPUT ${pycfile} DEPENDS ${pyfile}
                COMMAND ${PYTHON_EXECUTABLE} -c
                \"import py_compile\; py_compile.compile(file='${pyfile}', cfile='${pycfile}', doraise=True)\"
                COMMENT "Byte-compiling ${pyfile_name}"
            )
            INSTALL(FILES ${pycfile}
                DESTINATION ${GR_PYTHON_INSTALL_DESTINATION}
                COMPONENT ${GR_PYTHON_INSTALL_COMPONENT}
            )
        ENDFOREACH(pyfile)

    ####################################################################
    ELSEIF(GR_PYTHON_INSTALL_PROGRAMS)
    ####################################################################
        FILE(TO_NATIVE_PATH ${PYTHON_EXECUTABLE} pyexe_native)

        FOREACH(pyfile ${GR_PYTHON_INSTALL_PROGRAMS})
            GET_FILENAME_COMPONENT(pyfile_name ${pyfile} NAME)
            GET_FILENAME_COMPONENT(pyfile ${pyfile} ABSOLUTE)
            STRING(REPLACE "${CMAKE_SOURCE_DIR}" "${CMAKE_BINARY_DIR}" pyexefile "${pyfile}.exe")
            LIST(APPEND python_install_gen_targets ${pyexefile})

            GET_FILENAME_COMPONENT(pyexefile_path ${pyexefile} PATH)
            FILE(MAKE_DIRECTORY ${pyexefile_path})

            ADD_CUSTOM_COMMAND(
                OUTPUT ${pyexefile} DEPENDS ${pyfile}
                COMMAND ${PYTHON_EXECUTABLE} -c
                \"open('${pyexefile}', 'w').write('\#!${pyexe_native}\\n'+open('${pyfile}').read())\"
                COMMENT "Shebangin ${pyfile_name}"
            )

            #on windows, python files need an extension to execute
            GET_FILENAME_COMPONENT(pyfile_ext ${pyfile} EXT)
            IF(WIN32 AND NOT pyfile_ext)
                SET(pyfile_name "${pyfile_name}.py")
            ENDIF()

            INSTALL(PROGRAMS ${pyexefile} RENAME ${pyfile_name}
                DESTINATION ${GR_PYTHON_INSTALL_DESTINATION}
                COMPONENT ${GR_PYTHON_INSTALL_COMPONENT}
            )
        ENDFOREACH(pyfile)

    ENDIF()

    GR_UNIQUE_TARGET("pygen" ${python_install_gen_targets})

ENDFUNCTION(GR_PYTHON_INSTALL)
