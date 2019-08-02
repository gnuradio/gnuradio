# Copyright 2010-2011,2013,2018 Free Software Foundation, Inc.
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


add_library(gnuradio-qtgui
    DisplayPlot.cc
    FrequencyDisplayPlot.cc
    TimeDomainDisplayPlot.cc
    TimeRasterDisplayPlot.cc
    timeRasterGlobalData.cc
    WaterfallDisplayPlot.cc
    waterfallGlobalData.cc
    ConstellationDisplayPlot.cc
    HistogramDisplayPlot.cc
    VectorDisplayPlot.cc
    spectrumdisplayform.cc
    displayform.cc
    timedisplayform.cc
    timecontrolpanel.cc
    timerasterdisplayform.cc
    freqdisplayform.cc
    freqcontrolpanel.cc
    constellationdisplayform.cc
    histogramdisplayform.cc
    numberdisplayform.cc
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
    number_sink_impl.cc
    qtgui_util.cc
    ber_sink_b_impl.cc
    vectordisplayform.cc
    vector_sink_f_impl.cc
    edit_box_msg_impl.cc
)
target_compile_definitions(gnuradio-qtgui PRIVATE -DQWT_DLL) #setup QWT library linkage
target_include_directories(gnuradio-qtgui
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
  PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
  )
target_link_libraries(gnuradio-qtgui PUBLIC
    gnuradio-runtime
    gnuradio-fft
    gnuradio-filter
    ${GR_VOLK_LIB}
    qwt::qwt
    Qt5::Widgets
)
if (WIN32)
  target_link_libraries(gnuradio-qtgui PUBLIC
    dwrite
  )
endif(WIN32)

include(GrPython)
if(ENABLE_PYTHON)
  target_compile_definitions(gnuradio-qtgui PUBLIC -DENABLE_PYTHON)
  target_link_libraries(gnuradio-qtgui PUBLIC
    Python::Python
  )
endif(ENABLE_PYTHON)

set(qtgui_mod_includedir ${CMAKE_CURRENT_SOURCE_DIR}/../include/gnuradio/qtgui)
QT5_WRAP_CPP(qtgui_moc_sources
    ${qtgui_mod_includedir}/spectrumdisplayform.h
    ${qtgui_mod_includedir}/displayform.h
    ${qtgui_mod_includedir}/timedisplayform.h
    ${qtgui_mod_includedir}/timecontrolpanel.h
    ${qtgui_mod_includedir}/timerasterdisplayform.h
    ${qtgui_mod_includedir}/freqdisplayform.h
    ${qtgui_mod_includedir}/freqcontrolpanel.h
    ${qtgui_mod_includedir}/constellationdisplayform.h
    ${qtgui_mod_includedir}/waterfalldisplayform.h
    ${qtgui_mod_includedir}/histogramdisplayform.h
    ${qtgui_mod_includedir}/numberdisplayform.h
    ${qtgui_mod_includedir}/vectordisplayform.h
    ${qtgui_mod_includedir}/form_menus.h
    ${qtgui_mod_includedir}/DisplayPlot.h
    ${qtgui_mod_includedir}/FrequencyDisplayPlot.h
    ${qtgui_mod_includedir}/TimeDomainDisplayPlot.h
    ${qtgui_mod_includedir}/TimeRasterDisplayPlot.h
    ${qtgui_mod_includedir}/WaterfallDisplayPlot.h
    ${qtgui_mod_includedir}/ConstellationDisplayPlot.h
    ${qtgui_mod_includedir}/HistogramDisplayPlot.h
    ${qtgui_mod_includedir}/VectorDisplayPlot.h
    edit_box_msg_impl.h
    )
  target_sources(gnuradio-qtgui PRIVATE
    ${qtgui_moc_sources}
    )
  QT5_WRAP_UI(qtgui_ui_hdrs spectrumdisplayform.ui)
  target_sources(gnuradio-qtgui PRIVATE
    ${qtgui_ui_hdrs}
    )


#FIXME the sources expect <foo>.ui.h, but the macros generate ui_foo.h
#avoid changing the sources by generating the header with the include
set(spectrum_ui_hdr ${CMAKE_CURRENT_BINARY_DIR}/spectrumdisplayform.ui.h)
if(NOT EXISTS ${spectrum_ui_hdr})
    file(WRITE ${spectrum_ui_hdr} "#include <ui_spectrumdisplayform.h>\n")
endif()

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-qtgui.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-qtgui.rc
    @ONLY)

    target_sources(gnuradio-qtgui PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-qtgui.rc
    )
endif(MSVC)


if(BUILD_SHARED_LIBS)
  GR_LIBRARY_FOO(gnuradio-qtgui Qwt Qt5Widgets FFTW3f)
endif()
