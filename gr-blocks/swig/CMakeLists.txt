# Copyright 2012,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup swig generation
########################################################################
include(GrPython)
include(GrSwig)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/blocks_swig.py.in
    ${CMAKE_CURRENT_BINARY_DIR}/blocks_swig.py
@ONLY)

# We split up the swig files into multiple sections to minimize the
# memory overhead. If a .i file grows too large, create a new file
# named 'blocks_swigN.i' and add it to this list.
#
# Also add the line "from swig_blocksN import *" line to
# blocks_swig.py.in.
set(GR_SWIG_BLOCK_IFILES
  blocks_swig0
  blocks_swig1
  blocks_swig2
  blocks_swig3
  blocks_swig4
  blocks_swig5
  blocks_swig6
  blocks_swig7
  blocks_swig8
  blocks_swig9
  blocks_swig10
  )

set(GR_SWIG_INCLUDE_DIRS $<TARGET_PROPERTY:runtime_swig,INCLUDE_DIRECTORIES>)
set(GR_SWIG_TARGET_DEPS runtime_swig)
if(ENABLE_GR_CTRLPORT)
    set(GR_SWIG_FLAGS "-DGR_CTRLPORT")
endif()

foreach(swigfile ${GR_SWIG_BLOCK_IFILES})
  set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/${swigfile}_doc.i)
  set(GR_SWIG_DOC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/gnuradio/blocks
    ${CMAKE_CURRENT_BINARY_DIR}/../include/gnuradio/blocks
  )
  set(GR_SWIG_DOCS_TARGET_DEPS runtime_swig_swig_doc)
  set(GR_SWIG_LIBRARIES gnuradio-blocks)
  set(GR_SWIG_MODULES runtime_swig)
  GR_SWIG_MAKE(${swigfile} ${swigfile}.i)

  GR_SWIG_INSTALL(
    TARGETS ${swigfile}
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/blocks
  )
  list(APPEND SWIGFILES ${swigfile}.i)
  list(APPEND SWIGDOCFILES ${CMAKE_CURRENT_BINARY_DIR}/${swigfile}_doc.i)
endforeach(swigfile)

install(
    FILES
    ${SWIGFILES}
    ${SWIGDOCFILES}
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
)

# Install the Python file that pulls in the swig built files.
GR_PYTHON_INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/blocks_swig.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/blocks
)
