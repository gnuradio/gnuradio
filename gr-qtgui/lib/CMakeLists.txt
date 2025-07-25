# Copyright 2010-2011,2013,2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup the QT file generations stuff
########################################################################

set(QTGUI_SOURCES
    DisplayPlot.cc
    FrequencyDisplayPlot.cc
    EyeDisplayPlot.cc
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
    eyedisplaysform.cc
    eyedisplayform.cc
    eyecontrolpanel.cc
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
    eye_sink_c_impl.cc
    eye_sink_f_impl.cc
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
    matrix_display.cc
    matrix_sink_impl.cc
)

set(QTGUI_LIBS
    gnuradio-runtime
    gnuradio-fft
    gnuradio-filter
    Volk::volk
    qwt::qwt
    Qt5::Widgets
)

if(ENABLE_GR_QTGUI_OPENGL)
    qt5_add_resources(RFNOC_F15_RC_SRCS rfnoc_f15.qrc)
    list(APPEND QTGUI_SOURCES
        axis.c
        rfnoc_f15_display_impl.cc
        QRfnocF15ColorMapper.cc
        QRfnocF15Surface.cc
        ${RFNOC_F15_RC_SRCS}
    )
    find_package(OpenGL)
    list(APPEND PRIVATE_LIBS
        Qt5::OpenGL
        OpenGL::GL
    )
endif()

add_library(
    gnuradio-qtgui
    ${QTGUI_SOURCES}
)

# Set up QWT library linkage:
target_compile_definitions(gnuradio-qtgui PRIVATE -DQWT_DLL)
target_include_directories(
    gnuradio-qtgui
    PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(gnuradio-qtgui
    PUBLIC ${QTGUI_LIBS}
    PRIVATE ${PRIVATE_LIBS}
)

set(qtgui_mod_includedir ${CMAKE_CURRENT_SOURCE_DIR}/../include/gnuradio/qtgui)
qt5_wrap_cpp(
    qtgui_moc_sources
    ${qtgui_mod_includedir}/spectrumdisplayform.h
    ${qtgui_mod_includedir}/displayform.h
    ${qtgui_mod_includedir}/eyedisplaysform.h
    ${qtgui_mod_includedir}/eyedisplayform.h
    ${qtgui_mod_includedir}/timedisplayform.h
    ${qtgui_mod_includedir}/eyecontrolpanel.h
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
    ${qtgui_mod_includedir}/EyeDisplayPlot.h
    ${qtgui_mod_includedir}/FrequencyDisplayPlot.h
    ${qtgui_mod_includedir}/TimeDomainDisplayPlot.h
    ${qtgui_mod_includedir}/TimeRasterDisplayPlot.h
    ${qtgui_mod_includedir}/WaterfallDisplayPlot.h
    ${qtgui_mod_includedir}/ConstellationDisplayPlot.h
    ${qtgui_mod_includedir}/HistogramDisplayPlot.h
    ${qtgui_mod_includedir}/VectorDisplayPlot.h
    edit_box_msg_impl.h
    matrix_display.h
    matrix_display_signal.h
    )
if(ENABLE_GR_QTGUI_OPENGL)
    qt5_wrap_cpp(
        qtgui_moc_sources
        QRfnocF15Surface.h
        QRfnocF15ColorMapper.h
        )
endif()
target_sources(gnuradio-qtgui PRIVATE ${qtgui_moc_sources})
qt5_wrap_ui(qtgui_ui_hdrs spectrumdisplayform.ui)
target_sources(gnuradio-qtgui PRIVATE ${qtgui_ui_hdrs})

#FIXME the sources expect <foo>.ui.h, but the macros generate ui_foo.h
#avoid changing the sources by generating the header with the include
set(spectrum_ui_hdr ${CMAKE_CURRENT_BINARY_DIR}/spectrumdisplayform.ui.h)
if(NOT EXISTS ${spectrum_ui_hdr})
    file(WRITE ${spectrum_ui_hdr} "#include <ui_spectrumdisplayform.h>\n")
endif()

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${PROJECT_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-qtgui.rc.in
                   ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-qtgui.rc @ONLY)

    target_sources(gnuradio-qtgui PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-qtgui.rc)
endif(MSVC)

if(BUILD_SHARED_LIBS)
    gr_library_foo(gnuradio-qtgui Qwt Qt5Widgets)
endif()
