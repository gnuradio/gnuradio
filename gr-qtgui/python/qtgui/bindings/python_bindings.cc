
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/pybind11.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

namespace py = pybind11;

void bind_QWidget(py::module&);
// void bind_ConstellationDisplayPlot(py::module&);
// void bind_DisplayPlot(py::module&);
// void bind_FrequencyDisplayPlot(py::module&);
// void bind_HistogramDisplayPlot(py::module&);
// void bind_SpectrumGUIClass(py::module&);
// void bind_TimeDomainDisplayPlot(py::module&);
// void bind_TimeRasterDisplayPlot(py::module&);
// void bind_VectorDisplayPlot(py::module&);
// void bind_WaterfallDisplayPlot(py::module&);
void bind_ber_sink_b(py::module&);
void bind_const_sink_c(py::module&);
void bind_constellationdisplayform(py::module&);
void bind_displayform(py::module&);
void bind_edit_box_msg(py::module&);
void bind_eye_sink_c(py::module&);
void bind_eye_sink_f(py::module&);
void bind_form_menus(py::module&);
void bind_freq_sink_c(py::module&);
void bind_freq_sink_f(py::module&);
void bind_freqcontrolpanel(py::module&);
void bind_freqdisplayform(py::module&);
void bind_histogram_sink_f(py::module&);
void bind_histogramdisplayform(py::module&);
void bind_number_sink(py::module&);
void bind_numberdisplayform(py::module&);
// void bind_plot_raster(py::module&);
// void bind_plot_waterfall(py::module&);
void bind_qtgui_types(py::module&);
void bind_sink_c(py::module&);
void bind_sink_f(py::module&);
// void bind_spectrumUpdateEvents(py::module&);
// void bind_spectrumdisplayform(py::module&);
// void bind_timeRasterGlobalData(py::module&);
void bind_time_raster_sink_b(py::module&);
void bind_time_raster_sink_f(py::module&);
void bind_time_sink_c(py::module&);
void bind_time_sink_f(py::module&);
void bind_timecontrolpanel(py::module&);
void bind_timedisplayform(py::module&);
void bind_timerasterdisplayform(py::module&);
void bind_trigger_mode(py::module&);
// void bind_utils(py::module&);
void bind_vector_sink_f(py::module&);
void bind_vectordisplayform(py::module&);
// void bind_waterfallGlobalData(py::module&);
void bind_waterfall_sink_c(py::module&);
void bind_waterfall_sink_f(py::module&);
void bind_waterfalldisplayform(py::module&);

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(qtgui_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    bind_QWidget(m);
    bind_qtgui_types(m);
    // bind_ConstellationDisplayPlot(m);
    // bind_DisplayPlot(m);
    // bind_FrequencyDisplayPlot(m);
    // bind_HistogramDisplayPlot(m);
    // bind_SpectrumGUIClass(m);
    // bind_TimeDomainDisplayPlot(m);
    // bind_TimeRasterDisplayPlot(m);
    // bind_VectorDisplayPlot(m);
    // bind_WaterfallDisplayPlot(m);
    bind_ber_sink_b(m);
    bind_const_sink_c(m);
    // bind_constellationdisplayform(m);
    // bind_displayform(m);
    bind_edit_box_msg(m);
    bind_eye_sink_c(m);
    bind_eye_sink_f(m);
    // bind_form_menus(m);
    bind_freq_sink_c(m);
    bind_freq_sink_f(m);
    // bind_freqcontrolpanel(m);
    // bind_freqdisplayform(m);
    bind_histogram_sink_f(m);
    // bind_histogramdisplayform(m);
    bind_number_sink(m);
    // bind_numberdisplayform(m);
    // // bind_plot_raster(m);
    // // bind_plot_waterfall(m);
    bind_sink_c(m);
    bind_sink_f(m);
    // // bind_spectrumUpdateEvents(m);
    // // bind_spectrumdisplayform(m);
    // // bind_timeRasterGlobalData(m);
    bind_time_raster_sink_b(m);
    bind_time_raster_sink_f(m);
    bind_time_sink_c(m);
    bind_time_sink_f(m);
    // bind_timecontrolpanel(m);
    // bind_timedisplayform(m);
    // bind_timerasterdisplayform(m);
    bind_trigger_mode(m);
    // // bind_utils(m);
    bind_vector_sink_f(m);
    // bind_vectordisplayform(m);
    // // bind_waterfallGlobalData(m);
    bind_waterfall_sink_c(m);
    bind_waterfall_sink_f(m);
    // bind_waterfalldisplayform(m);
}
