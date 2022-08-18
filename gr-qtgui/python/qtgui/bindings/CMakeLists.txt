include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(
    APPEND
    qtgui_python_files
    # ConstellationDisplayPlot_python.cc
    # DisplayPlot_python.cc
    # FrequencyDisplayPlot_python.cc
    # HistogramDisplayPlot_python.cc
    # SpectrumGUIClass_python.cc
    # TimeDomainDisplayPlot_python.cc
    # TimeRasterDisplayPlot_python.cc
    # VectorDisplayPlot_python.cc
    # WaterfallDisplayPlot_python.cc
    QWidget_python.cc
    ber_sink_b_python.cc
    const_sink_c_python.cc
    constellationdisplayform_python.cc
    displayform_python.cc
    edit_box_msg_python.cc
    eye_sink_c_python.cc
    eye_sink_f_python.cc
    form_menus_python.cc
    freq_sink_c_python.cc
    freq_sink_f_python.cc
    freqcontrolpanel_python.cc
    freqdisplayform_python.cc
    histogram_sink_f_python.cc
    histogramdisplayform_python.cc
    number_sink_python.cc
    numberdisplayform_python.cc
    # plot_raster_python.cc
    # plot_waterfall_python.cc
    qtgui_types_python.cc
    sink_c_python.cc
    sink_f_python.cc
    # spectrumUpdateEvents_python.cc
    # spectrumdisplayform_python.cc
    # timeRasterGlobalData_python.cc
    time_raster_sink_b_python.cc
    time_raster_sink_f_python.cc
    time_sink_c_python.cc
    time_sink_f_python.cc
    timecontrolpanel_python.cc
    timedisplayform_python.cc
    timerasterdisplayform_python.cc
    trigger_mode_python.cc
    # utils_python.cc
    vector_sink_f_python.cc
    vectordisplayform_python.cc
    # waterfallGlobalData_python.cc
    waterfall_sink_c_python.cc
    waterfall_sink_f_python.cc
    waterfalldisplayform_python.cc
    python_bindings.cc)

set(include_dirs ${Qt5Widgets_INCLUDE_DIRS})
if(QWT_FOUND)
    list(APPEND include_dirs ${QWT_INCLUDE_DIRS})
endif(QWT_FOUND)
list(JOIN include_dirs "," extra_includes)

gr_pybind_make_check_hash(qtgui ../../.. gr::qtgui "${qtgui_python_files}")

install(
    TARGETS qtgui_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/qtgui
    COMPONENT pythonapi)
