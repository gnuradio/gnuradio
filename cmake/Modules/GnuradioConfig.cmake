# Copyright 2013 Free Software Foundation, Inc.
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

INCLUDE(FindPkgConfig)
INCLUDE(FindPackageHandleStandardArgs)

# if GR_REQUIRED_COMPONENTS is not defined, it will be set to the following list (all of them)
if(NOT GR_REQUIRED_COMPONENTS)
  set(GR_REQUIRED_COMPONENTS RUNTIME ANALOG ATSC AUDIO BLOCKS CHANNELS DIGITAL FCD FEC FFT FILTER NOAA PAGER QTGUI TRELLIS UHD VOCODER WAVELET WXGUI)
endif()

set(GNURADIO_ALL_LIBRARIES "")
set(GNURADIO_ALL_INCLUDE_DIRS "")

MACRO(LIST_CONTAINS var value)
  SET(${var})
  FOREACH(value2 ${ARGN})
    IF (${value} STREQUAL ${value2})
      SET(${var} TRUE)
    ENDIF(${value} STREQUAL ${value2})
  ENDFOREACH(value2)
ENDMACRO(LIST_CONTAINS)

function(GR_MODULE EXTVAR PCNAME INCFILE LIBFILE)

    LIST_CONTAINS(REQUIRED_MODULE ${EXTVAR} ${GR_REQUIRED_COMPONENTS})
    if(NOT REQUIRED_MODULE)
        #message("Ignoring GNU Radio Module ${EXTVAR}")
        return()
    endif()

    message("Checking for GNU Radio Module: ${EXTVAR}")

    # check for .pc hints
    PKG_CHECK_MODULES(PC_GNURADIO_${EXTVAR} ${PCNAME})

    set(INCVAR_NAME "GNURADIO_${EXTVAR}_INCLUDE_DIRS")
    set(LIBVAR_NAME "GNURADIO_${EXTVAR}_LIBRARIES")
    set(PC_INCDIR ${PC_GNURADIO_${EXTVAR}_INCLUDEDIR})
    set(PC_LIBDIR ${PC_GNURADIO_${EXTVAR}_LIBDIR})

    # look for include files
    FIND_PATH(
        ${INCVAR_NAME}
        NAMES ${INCFILE}
        HINTS $ENV{GNURADIO_RUNTIME_DIR}/include/gnuradio
            ${PC_INCDIR}/gnuradio/
            ${CMAKE_INSTALL_PREFIX}/include/gnuradio
        PATHS /usr/local/include/gnuradio
              /usr/include/gnuradio
    )

    # look for libs
    FIND_LIBRARY(
        ${LIBVAR_NAME}
        NAMES ${LIBFILE}
        HINTS $ENV{GNURADIO_RUNTIME_DIR}/lib
            ${PC_LIBDIR}
            ${CMAKE_INSTALL_PREFIX}/lib/
            ${CMAKE_INSTALL_PREFIX}/lib64/
        PATHS /usr/local/lib
              /usr/local/lib64
              /usr/lib
              /usr/lib64
    )

    # show results
    message(" * INCLUDES=${GNURADIO_${EXTVAR}_INCLUDE_DIRS}")
    message(" * LIBS=${GNURADIO_${EXTVAR}_LIBRARIES}")

    # append to all includes and libs list
    LIST(APPEND GNURADIO_ALL_INCLUDE_DIRS ${GNURADIO_${EXTVAR}_INCLUDE_DIRS})
    LIST(APPEND GNURADIO_ALL_LIBRARIES ${GNURADIO_${EXTVAR}_LIBRARIES})

    FIND_PACKAGE_HANDLE_STANDARD_ARGS(GNURADIO_${EXTVAR} DEFAULT_MSG GNURADIO_${EXTVAR}_LIBRARIES GNURADIO_${EXTVAR}_INCLUDE_DIRS)
    message("GNURADIO_${EXTVAR}_FOUND = ${GNURADIO_${EXTVAR}_FOUND}")
    set(GNURADIO_${EXTVAR}_FOUND ${GNURADIO_${EXTVAR}_FOUND} PARENT_SCOPE)

    # generate an error if the module is missing
    if(NOT GNURADIO_${EXTVAR}_FOUND)
       message(FATAL_ERROR "Required GNU Radio Component: ${EXTVAR} missing!")
    endif()

    MARK_AS_ADVANCED(GNURADIO_${EXTVAR}_LIBRARIES GNURADIO_${EXTVAR}_INCLUDE_DIRS)

endfunction()

GR_MODULE(RUNTIME gnuradio-runtime gr_top_block.h gnuradio-runtime)
GR_MODULE(ANALOG gnuradio-analog analog/api.h gnuradio-analog)
GR_MODULE(ATSC gnuradio-atsc atsc/api.h gnuradio-atsc)
GR_MODULE(AUDIO gnuradio-audio audio/api.h gnuradio-audio)
GR_MODULE(BLOCKS gnuradio-blocks blocks/api.h gnuradio-blocks)
GR_MODULE(CHANNELS gnuradio-channels channels/api.h gnuradio-channels)
GR_MODULE(DIGITAL gnuradio-digital digital/api.h gnuradio-digital)
GR_MODULE(FCD gnuradio-fcd fcd_api.h gnuradio-fcd)
GR_MODULE(FEC gnuradio-fec fec/api.h gnuradio-fec)
GR_MODULE(FFT gnuradio-fft fft/api.h gnuradio-fft)
GR_MODULE(FILTER gnuradio-filter filter/api.h gnuradio-filter)
GR_MODULE(NOAA gnuradio-noaa noaa/api.h gnuradio-noaa)
GR_MODULE(PAGER gnuradio-pager pager/api.h gnuradio-pager)
GR_MODULE(QTGUI gnuradio-qtgui qtgui/api.h gnuradio-qtgui)
GR_MODULE(TRELLIS gnuradio-trellis trellis/api.h gnuradio-trellis)
GR_MODULE(UHD gnuradio-uhd uhd/api.h gnuradio-uhd)
GR_MODULE(VOCODER gnuradio-vocoder vocoder/api.h gnuradio-vocoder)
GR_MODULE(WAVELET gnuradio-wavelet wavelet/api.h gnuradio-wavelet)
GR_MODULE(WXGUI gnuradio-wxgui wxgui/api.h gnuradio-wxgui)
