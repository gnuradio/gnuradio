# Copyright 2011-2016,2019 Free Software Foundation, Inc.
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

set(GR_SWIG_INCLUDE_DIRS $<TARGET_PROPERTY:runtime_swig,INCLUDE_DIRECTORIES>)
set(GR_SWIG_TARGET_DEPS runtime_swig)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/digital_swig.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/digital_swig.py
@ONLY)

# We split up the swig files into multiple sections to minimize the
# memory overhead. If a .i file grows too large, create a new file
# named 'digital_swigN.i' and add it to this list.
#
# Also add the line "from swig_blocksN import *" line to
# digital_swig.py.in.
set(GR_SWIG_BLOCK_IFILES
  digital_swig0
  digital_swig1
  digital_swig2
)

foreach(swigfile ${GR_SWIG_BLOCK_IFILES})
  set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/${swigfile}_doc.i)
  set(GR_SWIG_DOC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/gnuradio/digital
    ${CMAKE_CURRENT_BINARY_DIR}/../include/gnuradio/digital
  )
  set(GR_SWIG_DOCS_TARGET_DEPS runtime_swig_swig_doc)
  set(GR_SWIG_LIBRARIES gnuradio-digital
    gnuradio-filter gnuradio-analog gnuradio-blocks)
  GR_SWIG_MAKE(${swigfile} ${swigfile}.i)

  GR_SWIG_INSTALL(
    TARGETS ${swigfile}
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/digital
  )

  list(APPEND SWIGFILES ${swigfile}.i)
  list(APPEND SWIGDOCFILES ${CMAKE_CURRENT_BINARY_DIR}/${swigfile}_doc.i)
endforeach(swigfile)

install(
  FILES
  ${SWIGFILES}
  constellation.i
  ofdm_equalizer.i
  packet_header.i
  ${SWIGDOCFILES}
  DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
)

# Install the Python file that pulls in the swig built files.
GR_PYTHON_INSTALL(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/digital_swig.py
  DESTINATION ${GR_PYTHON_DIR}/gnuradio/digital
)
