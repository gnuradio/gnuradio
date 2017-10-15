# Copyright 2012 Free Software Foundation, Inc.
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

########################################################################
# Setup swig generation
########################################################################
include(GrPython)
include(GrSwig)

set(GR_SWIG_INCLUDE_DIRS
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_SWIG_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

if(ENABLE_GR_CTRLPORT)
  list(APPEND GR_SWIG_FLAGS "-DGR_CTRLPORT")
endif(ENABLE_GR_CTRLPORT)

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

foreach(swigfile ${GR_SWIG_BLOCK_IFILES})
  set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/${swigfile}_doc.i)
  set(GR_SWIG_DOC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/gnuradio/blocks
    ${CMAKE_CURRENT_BINARY_DIR}/../include/gnuradio/blocks
  )
  set(GR_SWIG_DOCS_TARGET_DEPS runtime_swig_swig_doc)
  set(GR_SWIG_TARGET_DEPS blocks_generated_includes)
  set(GR_SWIG_LIBRARIES gnuradio-blocks)
  GR_SWIG_MAKE(${swigfile} ${swigfile}.i)

  GR_SWIG_INSTALL(
    TARGETS ${swigfile}
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/blocks
    COMPONENT "blocks_python")

  list(APPEND SWIGFILES ${swigfile}.i)
  list(APPEND SWIGDOCFILES ${CMAKE_CURRENT_BINARY_DIR}/${swigfile}_doc.i)
endforeach(swigfile)

install(
    FILES
    ${SWIGFILES}
    ${SWIGDOCFILES}
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "blocks_swig"
)

# Install the Python file that pulls in the swig built files.
GR_PYTHON_INSTALL(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/blocks_swig.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/blocks
    COMPONENT "blocks_python"
)
