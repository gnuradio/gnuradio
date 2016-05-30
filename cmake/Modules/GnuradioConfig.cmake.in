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
  set(GR_REQUIRED_COMPONENTS RUNTIME ANALOG BLOCKS DIGITAL FFT FILTER PMT)
endif()

# Allows us to use all .cmake files in this directory
list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_LIST_DIR}")

# Easily access all libraries and includes of GNU Radio
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

    if(NOT PC_GNURADIO_${EXTVAR}_FOUND)
        set(PC_GNURADIO_${EXTVAR}_LIBRARIES ${LIBFILE})
    endif()

    set(INCVAR_NAME "GNURADIO_${EXTVAR}_INCLUDE_DIRS")
    set(LIBVAR_NAME "GNURADIO_${EXTVAR}_LIBRARIES")
    set(PC_INCDIR ${PC_GNURADIO_${EXTVAR}_INCLUDEDIR})
    set(PC_LIBDIR ${PC_GNURADIO_${EXTVAR}_LIBDIR})

    # look for include files
    FIND_PATH(
        ${INCVAR_NAME}
        NAMES ${INCFILE}
        HINTS $ENV{GNURADIO_RUNTIME_DIR}/include
            ${PC_INCDIR}
            ${CMAKE_INSTALL_PREFIX}/include
        PATHS /usr/local/include
              /usr/include
              "@CMAKE_INSTALL_PREFIX@/include"
              "@VOLK_INSTALL_INCLUDE_DIR@"
    )

    # look for libs
    foreach(libname ${PC_GNURADIO_${EXTVAR}_LIBRARIES})
        FIND_LIBRARY(
            ${LIBVAR_NAME}_${libname}
            NAMES ${libname}
            HINTS $ENV{GNURADIO_RUNTIME_DIR}/lib
                ${PC_LIBDIR}
                ${CMAKE_INSTALL_PREFIX}/lib/
                ${CMAKE_INSTALL_PREFIX}/lib64/
            PATHS /usr/local/lib
                  /usr/local/lib64
                  /usr/lib
                  /usr/lib64
                  "@CMAKE_INSTALL_PREFIX@/lib"
                  "@VOLK_INSTALL_LIBRARY_DIR@"
        )
	list(APPEND ${LIBVAR_NAME} ${${LIBVAR_NAME}_${libname}})
    endforeach(libname)

    set(${LIBVAR_NAME} ${${LIBVAR_NAME}} PARENT_SCOPE)

    # show results
    message(" * INCLUDES=${GNURADIO_${EXTVAR}_INCLUDE_DIRS}")
    message(" * LIBS=${GNURADIO_${EXTVAR}_LIBRARIES}")

    # append to all includes and libs list
    set(GNURADIO_ALL_INCLUDE_DIRS ${GNURADIO_ALL_INCLUDE_DIRS} ${GNURADIO_${EXTVAR}_INCLUDE_DIRS} PARENT_SCOPE)
    set(GNURADIO_ALL_LIBRARIES    ${GNURADIO_ALL_LIBRARIES}    ${GNURADIO_${EXTVAR}_LIBRARIES}    PARENT_SCOPE)

    FIND_PACKAGE_HANDLE_STANDARD_ARGS(GNURADIO_${EXTVAR} DEFAULT_MSG GNURADIO_${EXTVAR}_LIBRARIES GNURADIO_${EXTVAR}_INCLUDE_DIRS)
    message("GNURADIO_${EXTVAR}_FOUND = ${GNURADIO_${EXTVAR}_FOUND}")
    set(GNURADIO_${EXTVAR}_FOUND ${GNURADIO_${EXTVAR}_FOUND} PARENT_SCOPE)

    # generate an error if the module is missing
    if(NOT GNURADIO_${EXTVAR}_FOUND)
       message(FATAL_ERROR "Required GNU Radio Component: ${EXTVAR} missing!")
    endif()

    MARK_AS_ADVANCED(GNURADIO_${EXTVAR}_LIBRARIES GNURADIO_${EXTVAR}_INCLUDE_DIRS)

endfunction()

GR_MODULE(RUNTIME gnuradio-runtime gnuradio/top_block.h gnuradio-runtime)
GR_MODULE(ANALOG gnuradio-analog gnuradio/analog/api.h gnuradio-analog)
GR_MODULE(ATSC gnuradio-atsc gnuradio/atsc/api.h gnuradio-atsc)
GR_MODULE(AUDIO gnuradio-audio gnuradio/audio/api.h gnuradio-audio)
GR_MODULE(BLOCKS gnuradio-blocks gnuradio/blocks/api.h gnuradio-blocks)
GR_MODULE(CHANNELS gnuradio-channels gnuradio/channels/api.h gnuradio-channels)
GR_MODULE(DIGITAL gnuradio-digital gnuradio/digital/api.h gnuradio-digital)
GR_MODULE(FCD gnuradio-fcd gnuradio/fcd_api.h gnuradio-fcd)
GR_MODULE(FEC gnuradio-fec gnuradio/fec/api.h gnuradio-fec)
GR_MODULE(FFT gnuradio-fft gnuradio/fft/api.h gnuradio-fft)
GR_MODULE(FILTER gnuradio-filter gnuradio/filter/api.h gnuradio-filter)
GR_MODULE(NOAA gnuradio-noaa gnuradio/noaa/api.h gnuradio-noaa)
GR_MODULE(PAGER gnuradio-pager gnuradio/pager/api.h gnuradio-pager)
GR_MODULE(QTGUI gnuradio-qtgui gnuradio/qtgui/api.h gnuradio-qtgui)
GR_MODULE(TRELLIS gnuradio-trellis gnuradio/trellis/api.h gnuradio-trellis)
GR_MODULE(UHD gnuradio-uhd gnuradio/uhd/api.h gnuradio-uhd)
GR_MODULE(VOCODER gnuradio-vocoder gnuradio/vocoder/api.h gnuradio-vocoder)
GR_MODULE(WAVELET gnuradio-wavelet gnuradio/wavelet/api.h gnuradio-wavelet)
GR_MODULE(WXGUI gnuradio-wxgui gnuradio/wxgui/api.h gnuradio-wxgui)
GR_MODULE(ZEROMQ gnuradio-zeromq gnuradio/zeromq/api.h gnuradio-zeromq)
GR_MODULE(PMT gnuradio-runtime pmt/pmt.h gnuradio-pmt)
GR_MODULE(VOLK volk volk/volk.h volk)

list(REMOVE_DUPLICATES GNURADIO_ALL_INCLUDE_DIRS)
list(REMOVE_DUPLICATES GNURADIO_ALL_LIBRARIES)
