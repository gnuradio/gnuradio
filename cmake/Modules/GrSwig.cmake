# Copyright 2010-2011,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

if(DEFINED __INCLUDED_GR_SWIG_CMAKE)
    return()
endif()
set(__INCLUDED_GR_SWIG_CMAKE TRUE)

include(GrPython)

########################################################################
# Builds a swig documentation file to be generated into python docstrings
# Usage: GR_SWIG_MAKE_DOCS(output_file input_path input_path....)
#
# Set the following variable to specify extra dependent targets:
#   - GR_SWIG_DOCS_SOURCE_DEPS
#   - GR_SWIG_DOCS_TARGET_DEPS
########################################################################
function(GR_SWIG_MAKE_DOCS output_file)
    if(ENABLE_DOXYGEN)

        #setup the input files variable list, quote formatted
        set(input_files)
        unset(INPUT_PATHS)
        foreach(input_path ${ARGN})
            if(IS_DIRECTORY ${input_path}) #when input path is a directory
                file(GLOB input_path_h_files ${input_path}/*.h)
            else() #otherwise its just a file, no glob
                set(input_path_h_files ${input_path})
            endif()
            list(APPEND input_files ${input_path_h_files})
            set(INPUT_PATHS "${INPUT_PATHS} \"${input_path}\"")
        endforeach(input_path)

        #determine the output directory
        get_filename_component(name ${output_file} NAME_WE)
        get_filename_component(OUTPUT_DIRECTORY ${output_file} PATH)
        set(OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/${name}_swig_docs)
        make_directory(${OUTPUT_DIRECTORY})

        #generate the Doxyfile used by doxygen
        configure_file(
            ${CMAKE_SOURCE_DIR}/docs/doxygen/Doxyfile.swig_doc.in
            ${OUTPUT_DIRECTORY}/Doxyfile
        @ONLY)

        #Create a dummy custom command that depends on other targets
        include(GrMiscUtils)
        GR_GEN_TARGET_DEPS(_${name}_tag tag_deps ${GR_SWIG_DOCS_TARGET_DEPS})

        #call doxygen on the Doxyfile + input headers
        add_custom_command(
            OUTPUT ${OUTPUT_DIRECTORY}/xml/index.xml
            DEPENDS ${input_files} ${GR_SWIG_DOCS_SOURCE_DEPS} ${tag_deps}
            COMMAND ${DOXYGEN_EXECUTABLE} ${OUTPUT_DIRECTORY}/Doxyfile
            COMMENT "Generating doxygen xml for ${name} docs"
        )

        #call the swig_doc script on the xml files
        add_custom_command(
            OUTPUT ${output_file}
            DEPENDS ${input_files} ${stamp-file} ${OUTPUT_DIRECTORY}/xml/index.xml
            COMMAND ${PYTHON_EXECUTABLE} -B
                ${CMAKE_SOURCE_DIR}/docs/doxygen/swig_doc.py
                ${OUTPUT_DIRECTORY}/xml
                ${output_file}
            COMMENT "Generating python docstrings for ${name}"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/docs/doxygen
        )

    else(ENABLE_DOXYGEN)
        file(WRITE ${output_file} "\n") #no doxygen -> empty file
    endif(ENABLE_DOXYGEN)
endfunction(GR_SWIG_MAKE_DOCS)

########################################################################
# Build a swig target for the common gnuradio use case. Usage:
# GR_SWIG_MAKE(target ifile ifile ifile...)
#
# Set the following variables before calling:
#   - GR_SWIG_FLAGS
#   - GR_SWIG_INCLUDE_DIRS
#   - GR_SWIG_LIBRARIES
#   - GR_SWIG_SOURCE_DEPS
#   - GR_SWIG_TARGET_DEPS
#   - GR_SWIG_DOC_FILE
#   - GR_SWIG_DOC_DIRS
########################################################################
macro(GR_SWIG_MAKE name)
    set(ifiles ${ARGN})

    #do swig doc generation if specified
    if(GR_SWIG_DOC_FILE)
        set(GR_SWIG_DOCS_SOURCE_DEPS ${GR_SWIG_SOURCE_DEPS})
        list(APPEND GR_SWIG_DOCS_TARGET_DEPS ${GR_SWIG_TARGET_DEPS})
        GR_SWIG_MAKE_DOCS(${GR_SWIG_DOC_FILE} ${GR_SWIG_DOC_DIRS})
        add_custom_target(${name}_swig_doc DEPENDS ${GR_SWIG_DOC_FILE})
        list(APPEND GR_SWIG_TARGET_DEPS ${name}_swig_doc)
    endif()

    #prepend local swig directories
    list(INSERT GR_SWIG_INCLUDE_DIRS 0 "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")
    list(INSERT GR_SWIG_INCLUDE_DIRS 0 "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>")

    #setup the swig flags with flags and include directories
    set(CMAKE_SWIG_FLAGS -fvirtual -modern -keyword -w511 -w314 -relativeimport -py3 -module ${name} ${GR_SWIG_FLAGS})

    #set the C++ property on the swig .i file so it builds
    set_source_files_properties(${ifiles} PROPERTIES CPLUSPLUS ON)

    #setup the actual swig library target to be built
    include(UseSWIG)
    swig_add_library(${name} LANGUAGE python SOURCES ${ifiles})
    if(${name} STREQUAL "runtime_swig")
      set_target_properties(runtime_swig PROPERTIES DEFINE_SYMBOL "gnuradio_runtime_EXPORTS")
    endif(${name} STREQUAL "runtime_swig")
    set_target_properties(${name} PROPERTIES
      SWIG_USE_TARGET_INCLUDE_DIRECTORIES TRUE
      )
    target_include_directories(${name} PUBLIC ${GR_SWIG_INCLUDE_DIRS})
    set_property(TARGET ${name} PROPERTY SWIG_DEPENDS ${GR_SWIG_TARGET_DEPS})
    target_link_libraries(${name} Python::Python ${GR_SWIG_LIBRARIES})
endmacro(GR_SWIG_MAKE)

########################################################################
# Install swig targets generated by GR_SWIG_MAKE. Usage:
# GR_SWIG_INSTALL(
#   TARGETS target target target...
#   [DESTINATION destination]
# )
########################################################################
macro(GR_SWIG_INSTALL)

    include(CMakeParseArgumentsCopy)
    CMAKE_PARSE_ARGUMENTS(GR_SWIG_INSTALL "" "DESTINATION" "TARGETS" ${ARGN})

    foreach(name ${GR_SWIG_INSTALL_TARGETS})
      install(TARGETS ${name}
          DESTINATION ${GR_SWIG_INSTALL_DESTINATION}
        )

        include(GrPython)
        GR_PYTHON_INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name}.py
          DESTINATION ${GR_SWIG_INSTALL_DESTINATION}
          DEPENDS ${name}
        )

    endforeach(name)

endmacro(GR_SWIG_INSTALL)
