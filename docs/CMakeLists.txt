# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup dependencies
########################################################################
find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)

########################################################################
# Register components
########################################################################
include(GrComponent)
# gr_register_component("doxygen" ENABLE_DOXYGEN DOXYGEN_FOUND)
gr_register_component("docs" ENABLE_DOCS DOXYGEN_FOUND)
# gr_register_component("man-pages" ENABLE_MANPAGES)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_DOCS)

    ########################################################################
    # Create the doxygen configuration file
    ########################################################################
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
                   ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

    set(DOXYGEN_BUILT_DIR ${CMAKE_CURRENT_BINARY_DIR}/xml)

    ########################################################################
    # Make doxygen docs
    ########################################################################
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/xml)
    set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUTPUT_DIR}/index.xml)
    set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR}) #Doxygen won't create this for us


    add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                       COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                       MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
                       COMMENT "Generating doxygen documentation")

    add_custom_target(doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})

    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/gnuradio_docstrings.json
        COMMAND
            ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/docs/update_pydoc.py
            "scrape" "--xml_path" ${PROJECT_BINARY_DIR}/docs/xml "--json_path"
            ${CMAKE_CURRENT_BINARY_DIR}/gnuradio_docstrings.json
        COMMENT "Scraping generated documentation for docstrings ..."
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/update_pydoc.py doxygen)

    add_custom_target(gnuradio_docstrings ALL
                      DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/gnuradio_docstrings.json)
    ########################################################################
    # Make and install sphinx docs
    ########################################################################
    set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/source)
    set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/docs/)

    configure_file(${SPHINX_SOURCE}/conf.py.in
                   ${SPHINX_SOURCE}/conf.py @ONLY)

    add_custom_target(docs ALL
                      COMMAND
                      ${SPHINX_EXECUTABLE} -b html
                      ${SPHINX_SOURCE} ${SPHINX_BUILD}
                      DEPENDS ${DOXYGEN_INDEX_FILE}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Generating documentation with Sphinx")

    install(DIRECTORY ${BUILT_DIR} DESTINATION ${GR_PKG_DOC_DIR})
endif(ENABLE_DOCS)

# if(ENABLE_MANPAGES)
#     add_subdirectory(man)
# endif(ENABLE_MANPAGES)
