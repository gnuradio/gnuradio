# Copyright 2012,2016,2019 Free Software Foundation, Inc.
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

set(GR_SWIG_LIBRARIES gnuradio-trellis gnuradio-digital)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/trellis_swig.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/trellis_swig.py
@ONLY)

# We split up the swig files into multiple sections to minimize the
# memory overhead. If a .i file grows too large, create a new file
# named 'trellis_swigN.i' and add it to this list.
#
# Also add the line "from swig_trellisN import *" line to
# trellis_swig.py.in.
set(GR_SWIG_TRELLIS_IFILES
  trellis_swig0
  trellis_swig1
)

foreach(swigfile ${GR_SWIG_TRELLIS_IFILES})
  set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/${swigfile}_doc.i)
  set(GR_SWIG_DOC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/gnuradio/trellis
    ${CMAKE_CURRENT_BINARY_DIR}/../include/gnuradio/trellis
  )
  set(GR_SWIG_DOCS_TARGET_DEPS runtime_swig_swig_doc)
  set(GR_SWIG_LIBRARIES gnuradio-trellis gnuradio-digital)
  GR_SWIG_MAKE(${swigfile} ${swigfile}.i)

  GR_SWIG_INSTALL(
    TARGETS ${swigfile}
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/trellis
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
    FILES ${CMAKE_CURRENT_BINARY_DIR}/trellis_swig.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/trellis
    COMPONENT "trellis_python"
)
