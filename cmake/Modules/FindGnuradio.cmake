INCLUDE(FindPkgConfig)
INCLUDE(FindPackageHandleStandardArgs)

# a subset of required modules can be set using the following syntax
#set(GR_REQUIRED_MODULES RUNTIME BLOCKS)

# if GR_REQUIRED_MODULES is not defined, it will be set to the following list (all of them)
if(NOT GR_REQUIRED_MODULES)
    set(GR_REQUIRED_MODULES RUNTIME ANALOG ATSC AUDIO BLOCKS CHANNELS DIGITAL FEC FFT FILTER NOAA PAGER QTGUI TRELLIS UHD VOCODER WAVELET PMT)
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

    LIST_CONTAINS(REQUIRED_MODULE ${EXTVAR} ${GR_REQUIRED_MODULES})
    if(NOT REQUIRED_MODULE)
        #message("Ignoring GNU Radio Module ${EXTVAR}")
        return()
    endif()

    message("Checking for GNU Radio Module: ${EXTVAR}")

    # check for .pc hints
    PKG_CHECK_MODULES(PC_GNURADIO_${EXTVAR} ${PCNAME})

    set(INCVAR_NAME "GNURADIO_${EXTVAR}_INCLUDE_DIRS")
    set(LIBVAR_NAME "GNURADIO_${EXTVAR}_LIBRARIES")
    set(PC_INCDIR "PC_GNURADIO_${EXTVAR}_INCLUDEDIR")
    set(PC_LIBDIR "PC_GNURADIO_${EXTVAR}_LIBDIR")

    # look for include files
    FIND_PATH(
        ${INCVAR_NAME}
        NAMES ${INCFILE}
        HINTS $ENV{GNURADIO_RUNTIME_DIR}/include/gnuradio
            ${PC_INCDIR}
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

GR_MODULE(CORE gnuradio-runtime gr_top_block.h gnuradio-runtime)
GR_MODULE(RUNTIME gnuradio-runtime gr_top_block.h gnuradio-runtime)
GR_MODULE(ANALOG gnuradio-analog analog/noise_type.h gnuradio-analog)
GR_MODULE(ATSC gnuradio-atsc atsc_api.h gnuradio-atsc)
GR_MODULE(AUDIO gnuradio-audio audio/sink.h gnuradio-audio)
GR_MODULE(BLOCKS gnuradio-blocks blocks/delay.h gnuradio-blocks)
GR_MODULE(CHANNELS gnuradio-channels channels/channel_model.h gnuradio-channels)
GR_MODULE(DIGITAL gnuradio-digital digital/lfsr.h gnuradio-digital)
GR_MODULE(FEC gnuradio-fec fec/rs.h gnuradio-fec)
GR_MODULE(FFT gnuradio-fft fft/fft.h gnuradio-fft)
GR_MODULE(FILTER gnuradio-filter filter/fir_filter.h gnuradio-filter)
GR_MODULE(NOAA gnuradio-noaa noaa/hrpt.h gnuradio-noaa)
GR_MODULE(PAGER gnuradio-pager noaa/flex_deinterleave.h gnuradio-pager)
GR_MODULE(QTGUI gnuradio-qtgui qtgui/utils.h gnuradio-qtgui)
GR_MODULE(TRELLIS gnuradio-trellis trellis/fsm.h gnuradio-trellis)
GR_MODULE(UHD gnuradio-uhd gr_uhd/usrp_sink.h gnuradio-uhd)
GR_MODULE(VOCODER gnuradio-vocoder vocoder/alaw_encode_sb.h gnuradio-vocoder)
GR_MODULE(WAVELET gnuradio-wavelet wavelet/wavelet_ff.h gnuradio-wavelet)
GR_MODULE(PMT gnuradio-pmt pmt/pmt.h gnuradio-pmt)


