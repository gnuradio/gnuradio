# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Create the doxygen configuration file
########################################################################
file(TO_NATIVE_PATH ${PROJECT_SOURCE_DIR} top_srcdir)
file(TO_NATIVE_PATH ${PROJECT_BINARY_DIR} top_builddir)
file(TO_NATIVE_PATH ${PROJECT_SOURCE_DIR} abs_top_srcdir)
file(TO_NATIVE_PATH ${PROJECT_BINARY_DIR} abs_top_builddir)

set(HAVE_DOT ${DOXYGEN_DOT_FOUND})
set(enable_html_docs YES)
set(enable_latex_docs NO)
set(enable_xml_docs YES)

if(MATHJAX2_FOUND)
    set(enable_mathjax YES)
else()
    message(WARNING "MathJax 2 not found, HTML equations might not be properly rendered.")
    set(enable_mathjax NO)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
               ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/other/build_guide.dox.in
               ${CMAKE_CURRENT_BINARY_DIR}/other/build_guide.dox @ONLY)

set(BUILT_DIRS ${CMAKE_CURRENT_BINARY_DIR}/xml ${CMAKE_CURRENT_BINARY_DIR}/html)

########################################################################
# Make and install doxygen docs
########################################################################
add_custom_command(
    OUTPUT ${BUILT_DIRS}
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${GENERATED_DEPS}
    COMMENT "Generating documentation with doxygen")

add_custom_target(doxygen_target ALL DEPENDS ${BUILT_DIRS})

if(ENABLE_DOXYGEN)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gnuradio_docstrings.json
        COMMAND
            ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/docs/doxygen/update_pydoc.py
            "scrape" "--xml_path" ${PROJECT_BINARY_DIR}/docs/doxygen/xml "--json_path"
            ${CMAKE_CURRENT_BINARY_DIR}/gnuradio_docstrings.json
        COMMENT "Scraping generated documentation for docstrings ..."
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/update_pydoc.py doxygen_target)

    add_custom_target(gnuradio_docstrings ALL
                      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gnuradio_docstrings.json)
endif(ENABLE_DOXYGEN)

install(DIRECTORY ${BUILT_DIRS} DESTINATION ${GR_PKG_DOC_DIR})
