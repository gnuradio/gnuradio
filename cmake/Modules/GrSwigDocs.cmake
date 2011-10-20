# Copyright 2011 Free Software Foundation, Inc.
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

if(DEFINED __INCLUDED_GR_SWIG_DOCS_CMAKE)
    return()
endif()
set(__INCLUDED_GR_SWIG_DOCS_CMAKE TRUE)

include(GrPython)

########################################################################
# Builds a swig documentation file to be generated into python docstrings
# Usage: GR_SWIG_MAKE_DOCS(output_file input_path input_path....)
#
# Set the following variable to specify extra dependent targets:
#   - GR_SWIG_DOCS_EXTRA_DEPS
########################################################################
function(GR_SWIG_MAKE_DOCS output_file)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)

        #setup the input files variable list, quote formated
        set(input_files ${ARGN})
        unset(INPUT_PATHS)
        foreach(input_path ${ARGN})
            set(INPUT_PATHS "${INPUT_PATHS} \"${input_path}\"")
        endforeach(input_path)

        #determine the output directory
        get_filename_component(OUTPUT_DIRECTORY ${output_file} PATH)
        set(OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/swig_docs)
        make_directory(${OUTPUT_DIRECTORY})

        #generate the Doxyfile used by doxygen
        configure_file(
            ${CMAKE_SOURCE_DIR}/docs/doxygen/Doxyfile.swig_doc.in
            ${OUTPUT_DIRECTORY}/Doxyfile
        @ONLY)

        #call doxygen on the Doxyfile + input headers
        add_custom_command(
            OUTPUT ${OUTPUT_DIRECTORY}/xml/index.xml
            DEPENDS ${input_files} ${GR_SWIG_DOCS_EXTRA_DEPS}
            COMMAND ${DOXYGEN_EXECUTABLE} ${OUTPUT_DIRECTORY}/Doxyfile
            COMMENT "Generating doxygen xml for swig docs"
        )

        #call the swig_doc script on the xml files
        add_custom_command(
            OUTPUT ${output_file}
            DEPENDS ${input_files} ${OUTPUT_DIRECTORY}/xml/index.xml
            COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B}
                ${CMAKE_SOURCE_DIR}/docs/doxygen/swig_doc.py
                ${OUTPUT_DIRECTORY}/xml
                ${output_file}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs/doxygen
        )

    else(DOXYGEN_FOUND)
        file(WRITE ${output_file} "\n") #no doxygen -> empty file
    endif(DOXYGEN_FOUND)
endfunction(GR_SWIG_MAKE_DOCS)
