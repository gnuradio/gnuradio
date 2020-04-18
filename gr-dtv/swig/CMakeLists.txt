# Copyright 2014,2019,2020 Free Software Foundation, Inc.
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

set(GR_SWIG_LIBRARIES gnuradio-dtv gnuradio-fft)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/dtv_swig.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/dtv_swig.py
@ONLY)

# We split up the swig files into multiple sections to minimize the
# memory overhead. If a .i file grows too large, create a new file
# named 'dtv_swigN.i' and add it to this list.
#
# Also add the line "from swig_dtvN import *" line to
# dtv_swig.py.in.
set(GR_SWIG_DTV_IFILES
  dtv_swig0
  dtv_swig1
  dtv_swig2
)

foreach(swigfile ${GR_SWIG_DTV_IFILES})
  set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/${swigfile}_doc.i)
  set(GR_SWIG_DOC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/gnuradio/dtv
    ${CMAKE_CURRENT_BINARY_DIR}/../include/gnuradio/dtv
  )
  set(GR_SWIG_DOCS_TARGET_DEPS runtime_swig_swig_doc)
  set(GR_SWIG_LIBRARIES gnuradio-dtv gnuradio-fft)
  GR_SWIG_MAKE(${swigfile} ${swigfile}.i)

  GR_SWIG_INSTALL(
    TARGETS ${swigfile}
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/dtv
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
    FILES ${CMAKE_CURRENT_BINARY_DIR}/dtv_swig.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/dtv
    COMPONENT "dtv_python"
)
