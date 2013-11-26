# Copyright 2010-2011,2013 Free Software Foundation, Inc.
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
# Setup the QT file generations stuff
########################################################################
set(qtgui_mod_includedir ${CMAKE_CURRENT_SOURCE_DIR}/../include/gnuradio/qtgui)
set(qtgui_moc_hdrs
    ${qtgui_mod_includedir}/spectrumdisplayform.h
    ${qtgui_mod_includedir}/displayform.h
    ${qtgui_mod_includedir}/timedisplayform.h
    ${qtgui_mod_includedir}/timerasterdisplayform.h
    ${qtgui_mod_includedir}/freqdisplayform.h
    ${qtgui_mod_includedir}/constellationdisplayform.h
    ${qtgui_mod_includedir}/waterfalldisplayform.h
    ${qtgui_mod_includedir}/histogramdisplayform.h
    ${qtgui_mod_includedir}/form_menus.h
    ${qtgui_mod_includedir}/DisplayPlot.h
    ${qtgui_mod_includedir}/FrequencyDisplayPlot.h
    ${qtgui_mod_includedir}/TimeDomainDisplayPlot.h
    ${qtgui_mod_includedir}/TimeRasterDisplayPlot.h
    ${qtgui_mod_includedir}/WaterfallDisplayPlot.h
    ${qtgui_mod_includedir}/ConstellationDisplayPlot.h
    ${qtgui_mod_includedir}/HistogramDisplayPlot.h
)
QT4_WRAP_CPP(qtgui_moc_srcs ${qtgui_moc_hdrs})
QT4_WRAP_UI(qtgui_ui_hdrs spectrumdisplayform.ui)

#FIXME the sources expect <foo>.ui.h, but the macros generate ui_foo.h
#avoid changing the sources by generating the header with the include
set(spectrum_ui_hdr ${CMAKE_CURRENT_BINARY_DIR}/spectrumdisplayform.ui.h)
if(NOT EXISTS ${spectrum_ui_hdr})
    file(WRITE ${spectrum_ui_hdr} "#include <ui_spectrumdisplayform.h>\n")
endif(NOT EXISTS ${spectrum_ui_hdr})

set(qtgui_srcs
    ${qtgui_moc_srcs}
    ${qtgui_ui_hdrs}
    DisplayPlot.cc
    FrequencyDisplayPlot.cc
    TimeDomainDisplayPlot.cc
    TimeRasterDisplayPlot.cc
    timeRasterGlobalData.cc
    WaterfallDisplayPlot.cc
    waterfallGlobalData.cc
    ConstellationDisplayPlot.cc
    HistogramDisplayPlot.cc
    spectrumdisplayform.cc
    displayform.cc
    timedisplayform.cc
    timerasterdisplayform.cc
    freqdisplayform.cc
    constellationdisplayform.cc
    histogramdisplayform.cc
    waterfalldisplayform.cc
    SpectrumGUIClass.cc
    spectrumUpdateEvents.cc
    plot_waterfall.cc
    plot_raster.cc
    sink_c_impl.cc
    sink_f_impl.cc
    time_sink_c_impl.cc
    time_sink_f_impl.cc
    time_raster_sink_b_impl.cc
    time_raster_sink_f_impl.cc
    freq_sink_c_impl.cc
    freq_sink_f_impl.cc
    const_sink_c_impl.cc
    waterfall_sink_c_impl.cc
    waterfall_sink_f_impl.cc
    histogram_sink_f_impl.cc
    qtgui_util.cc
)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-qtgui.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-qtgui.rc
    @ONLY)

    list(APPEND qtgui_srcs
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-qtgui.rc
    )
endif(MSVC)

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${GR_QTGUI_INCLUDE_DIRS}
    ${GR_FFT_INCLUDE_DIRS}
    ${GR_FILTER_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${VOLK_INCLUDE_DIRS}
    ${QWT_INCLUDE_DIRS}
    ${QT_INCLUDE_DIRS}
    ${FFTW3F_INCLUDE_DIRS}
    ${LOG4CPP_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${PYTHON_INCLUDE_DIRS}
)

link_directories(
    ${QWT_LIBRARY_DIRS}
    ${FFTW3F_LIBRARY_DIRS}
    ${LOG4CPP_LIBRARY_DIRS}
    ${Boost_LIBRARY_DIRS}
)

include_directories(${PYTHON_INCLUDE_PATH}) #deprecated for dirs (cmake 2.6)

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
  include_directories(${ICE_INCLUDE_DIR})
endif(ENABLE_GR_CTRLPORT)

########################################################################
# Setup library
########################################################################
list(APPEND qtgui_libs
    gnuradio-runtime
    gnuradio-fft
    gnuradio-filter
    volk
    ${QWT_LIBRARIES}
    ${QT_LIBRARIES}
    ${PYTHON_LIBRARIES}
    ${FFTW3F_LIBRARIES}
    ${LOG4CPP_LIBRARIES}
)

add_definitions(-DQWT_DLL) #setup QWT library linkage
add_library(gnuradio-qtgui SHARED ${qtgui_srcs})
target_link_libraries(gnuradio-qtgui ${qtgui_libs})
GR_LIBRARY_FOO(gnuradio-qtgui RUNTIME_COMPONENT "qtgui_runtime" DEVEL_COMPONENT "qtgui_devel")
