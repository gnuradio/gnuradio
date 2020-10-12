/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "eye_sink_f_impl.h"

#include <gnuradio/block_detail.h>
#include <gnuradio/buffer.h>
#include <gnuradio/fft/fft.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/prefs.h>

#include <qwt_symbol.h>
#include <volk/volk.h>

#include <string.h>

namespace gr {
namespace qtgui {

eye_sink_f::sptr eye_sink_f::make(int size,
                                  double samp_rate,
                                  const std::string& name,
                                  unsigned int nconnections,
                                  QWidget* parent)
{
    return gnuradio::make_block_sptr<eye_sink_f_impl>(
        size, samp_rate, name, nconnections, parent);
}

eye_sink_f_impl::eye_sink_f_impl(int size,
                                 double samp_rate,
                                 const std::string& name,
                                 unsigned int nconnections,
                                 QWidget* parent)
    : sync_block("eye_sink_f",
                 io_signature::make(0, nconnections, sizeof(float)),
                 io_signature::make(0, 0, 0)),
      d_size(size),
      d_buffer_size(2 * size),
      d_samp_rate(samp_rate),
      d_name(name),
      d_nconnections(nconnections),
      d_parent(parent)
{
    if (nconnections > 24)
        throw std::runtime_error("eye_sink_f only supports up to 24 inputs");

    // Required now for Qt; argc must be greater than 0 and argv
    // must have at least one valid character. Must be valid through
    // life of the qApplication:
    // http://harmattan-dev.nokia.com/docs/library/html/qt4/qapplication.html
    d_argc = 1;
    d_argv = new char;
    d_argv[0] = '\0';

    d_main_gui = NULL;

    // setup PDU handling input port
    message_port_register_in(pmt::mp("in"));
    set_msg_handler(pmt::mp("in"), [this](pmt::pmt_t msg) { this->handle_pdus(msg); });

    // +1 for the PDU buffer
    for (unsigned int n = 0; n < d_nconnections + 1; n++) {
        d_buffers.push_back(
            (double*)volk_malloc(d_buffer_size * sizeof(double), volk_get_alignment()));
        memset(d_buffers[n], 0, d_buffer_size * sizeof(double));

        d_fbuffers.push_back(
            (float*)volk_malloc(d_buffer_size * sizeof(float), volk_get_alignment()));
        memset(d_fbuffers[n], 0, d_buffer_size * sizeof(float));
    }

    // Set alignment properties for VOLK
    const int alignment_multiple = volk_get_alignment() / sizeof(float);
    set_alignment(std::max(1, alignment_multiple));

    d_tags = std::vector<std::vector<gr::tag_t>>(d_nconnections);

    initialize();

    d_main_gui->setNPoints(d_size); // setup GUI box with size
    set_trigger_mode(gr::qtgui::TRIG_MODE_FREE, gr::qtgui::TRIG_SLOPE_POS, 0, 0, 0);

    set_history(2);          // so we can look ahead for the trigger slope
    declare_sample_delay(1); // delay the tags for a history of 2
}

eye_sink_f_impl::~eye_sink_f_impl()
{
    if (!d_main_gui->isClosed())
        d_main_gui->close();

    // d_main_gui is a qwidget destroyed with its parent
    for (unsigned int n = 0; n < d_nconnections + 1; n++) {
        volk_free(d_buffers[n]);
        volk_free(d_fbuffers[n]);
    }

    delete d_argv;
}

bool eye_sink_f_impl::check_topology(int ninputs, int noutputs)
{
    return (unsigned int)(ninputs) == d_nconnections;
}

void eye_sink_f_impl::initialize()
{
    if (qApp != NULL) {
        d_qApplication = qApp;
    } else {
#if QT_VERSION >= 0x040500 && QT_VERSION < 0x050000
        std::string style = prefs::singleton()->get_string("qtgui", "style", "raster");
        QApplication::setGraphicsSystem(QString(style.c_str()));
#endif
        d_qApplication = new QApplication(d_argc, &d_argv);
    }

    // If a style sheet is set in the prefs file, enable it here.
    check_set_qss(d_qApplication);

    unsigned int numplots = (d_nconnections > 0) ? d_nconnections : 1;
    d_main_gui = new EyeDisplayForm(numplots, false, d_parent);
    d_main_gui->setNPoints(d_size);
    d_main_gui->setSampleRate(d_samp_rate);
    d_main_gui->disableLegend();

    // initialize update time to 10 times a second
    set_update_time(0.1);
    // initialize number of samples per symbol to 4
    set_samp_per_symbol(4);
}

void eye_sink_f_impl::exec_() { d_qApplication->exec(); }

QWidget* eye_sink_f_impl::qwidget() { return d_main_gui; }

#ifdef ENABLE_PYTHON
PyObject* eye_sink_f_impl::pyqwidget()
{
    PyObject* w = PyLong_FromVoidPtr((void*)d_main_gui);
    PyObject* retarg = Py_BuildValue("N", w);
    return retarg;
}
#else
void* eye_sink_f_impl::pyqwidget() { return NULL; }
#endif

void eye_sink_f_impl::set_y_axis(double min, double max)
{
    d_main_gui->setYaxis(min, max);
}

void eye_sink_f_impl::set_y_label(const std::string& label, const std::string& unit)
{
    d_main_gui->setYLabel(label, unit);
}

void eye_sink_f_impl::set_update_time(double t)
{
    // convert update time to ticks
    gr::high_res_timer_type tps = gr::high_res_timer_tps();
    d_update_time = t * tps;
    d_main_gui->setUpdateTime(t);
    d_last_time = 0;
}

void eye_sink_f_impl::set_samp_per_symbol(unsigned int sps)
{
    d_main_gui->setSamplesPerSymbol(sps);
}

void eye_sink_f_impl::set_title(const std::string& title)
{
    // set_title no longer used.
}

void eye_sink_f_impl::set_line_label(unsigned int which, const std::string& label)
{
    d_main_gui->setLineLabel(which, label.c_str());
}

void eye_sink_f_impl::set_line_color(unsigned int which, const std::string& color)
{
    d_main_gui->setLineColor(which, color.c_str());
}

void eye_sink_f_impl::set_line_width(unsigned int which, int width)
{
    d_main_gui->setLineWidth(which, width);
}

void eye_sink_f_impl::set_line_style(unsigned int which, int style)
{
    d_main_gui->setLineStyle(which, (Qt::PenStyle)style);
}

void eye_sink_f_impl::set_line_marker(unsigned int which, int marker)
{
    d_main_gui->setLineMarker(which, (QwtSymbol::Style)marker);
}

void eye_sink_f_impl::set_line_alpha(unsigned int which, double alpha)
{
    d_main_gui->setMarkerAlpha(which, (int)(255.0 * alpha));
}

void eye_sink_f_impl::set_trigger_mode(gr::qtgui::trigger_mode mode,
                                       gr::qtgui::trigger_slope slope,
                                       float level,
                                       float delay,
                                       int channel,
                                       const std::string& tag_key)
{
    gr::thread::scoped_lock lock(d_setlock);

    d_trigger_mode = mode;
    d_trigger_slope = slope;
    d_trigger_level = level;
    d_trigger_delay = static_cast<int>(delay * d_samp_rate);
    d_trigger_channel = channel;
    d_trigger_tag_key = pmt::intern(tag_key);
    d_triggered = false;
    d_trigger_count = 0;
    int d_sps = d_main_gui->getSamplesPerSymbol();

    if ((d_trigger_delay < 0) || (d_trigger_delay > 2 * d_sps)) {
        GR_LOG_WARN(
            d_logger,
            boost::format("Trigger delay (%1%) outside of display range (0:%2%).") %
                (d_trigger_delay / d_samp_rate) % ((2 * d_sps) / d_samp_rate));
        d_trigger_delay = std::max(0, std::min(2 * d_sps, d_trigger_delay));
        delay = d_trigger_delay / d_samp_rate;
    }

    d_main_gui->setTriggerMode(d_trigger_mode);
    d_main_gui->setTriggerSlope(d_trigger_slope);
    d_main_gui->setTriggerLevel(d_trigger_level);
    d_main_gui->setTriggerDelay(delay);
    d_main_gui->setTriggerChannel(d_trigger_channel);
    d_main_gui->setTriggerTagKey(tag_key);

    _reset();
}

void eye_sink_f_impl::set_size(int width, int height)
{
    d_main_gui->resize(QSize(width, height));
}

std::string eye_sink_f_impl::title() { return d_main_gui->title().toStdString(); }

std::string eye_sink_f_impl::line_label(unsigned int which)
{
    return d_main_gui->lineLabel(0).toStdString();
}

std::string eye_sink_f_impl::line_color(unsigned int which)
{
    return d_main_gui->lineColor(0).toStdString();
}

int eye_sink_f_impl::line_width(unsigned int which) { return d_main_gui->lineWidth(0); }

int eye_sink_f_impl::line_style(unsigned int which) { return d_main_gui->lineStyle(0); }

int eye_sink_f_impl::line_marker(unsigned int which) { return d_main_gui->lineMarker(0); }

double eye_sink_f_impl::line_alpha(unsigned int which)
{
    return (double)(d_main_gui->markerAlpha(0)) / 255.0;
}

void eye_sink_f_impl::set_nsamps(const int newsize)
{
    if (newsize != d_size) {
        gr::thread::scoped_lock lock(d_setlock);

        // Set new size and reset buffer index
        // (throws away any currently held data, but who cares?)
        d_size = newsize;
        d_buffer_size = 2 * d_size;
        int d_sps = d_main_gui->getSamplesPerSymbol();

        // Resize buffers and replace data
        for (unsigned int n = 0; n < d_nconnections + 1; n++) {
            volk_free(d_buffers[n]);
            d_buffers[n] = (double*)volk_malloc(d_buffer_size * sizeof(double),
                                                volk_get_alignment());
            memset(d_buffers[n], 0, d_buffer_size * sizeof(double));

            volk_free(d_fbuffers[n]);
            d_fbuffers[n] =
                (float*)volk_malloc(d_buffer_size * sizeof(float), volk_get_alignment());
            memset(d_fbuffers[n], 0, d_buffer_size * sizeof(float));
        }

        // If delay was set beyond the new boundary, pull it back.
        if (d_trigger_delay > 2 * d_sps) {
            GR_LOG_WARN(d_logger,
                        boost::format("Trigger delay (%1%) outside of display range "
                                      "(0:%2%). Moving to 50%% point.") %
                            (d_trigger_delay / d_samp_rate) %
                            ((2 * d_sps) / d_samp_rate));
            d_trigger_delay = d_sps;
            d_main_gui->setTriggerDelay(d_trigger_delay / d_samp_rate);
        }

        d_main_gui->setNPoints(d_size);
        _reset();
    }
}

void eye_sink_f_impl::set_samp_rate(const double samp_rate)
{
    gr::thread::scoped_lock lock(d_setlock);
    d_samp_rate = samp_rate;
    d_main_gui->setSampleRate(d_samp_rate);
}

int eye_sink_f_impl::nsamps() const { return d_size; }

void eye_sink_f_impl::enable_stem_plot(bool en)
{
    /* Used by parent class, do not remove*/
}

void eye_sink_f_impl::enable_menu(bool en) { d_main_gui->enableMenu(en); }

void eye_sink_f_impl::enable_grid(bool en) { d_main_gui->setGrid(en); }

void eye_sink_f_impl::enable_autoscale(bool en) { d_main_gui->autoScale(en); }

void eye_sink_f_impl::enable_semilogx(bool en)
{
    /* Used by parent class, do not remove*/
}

void eye_sink_f_impl::enable_semilogy(bool en)
{
    /* Used by parent class, do not remove*/
}

void eye_sink_f_impl::enable_control_panel(bool en)
{
    if (en)
        d_main_gui->setupControlPanel();
    else
        d_main_gui->teardownControlPanel();
}

void eye_sink_f_impl::enable_tags(unsigned int which, bool en)
{
    d_main_gui->setTagMenu(which, en);
}

void eye_sink_f_impl::enable_tags(bool en)
{
    for (unsigned int n = 0; n < d_nconnections; ++n) {
        d_main_gui->setTagMenu(n, en);
    }
}

void eye_sink_f_impl::enable_axis_labels(bool en) { d_main_gui->setAxisLabels(en); }

void eye_sink_f_impl::disable_legend() { d_main_gui->disableLegend(); }

void eye_sink_f_impl::reset()
{
    gr::thread::scoped_lock lock(d_setlock);
    _reset();
}

void eye_sink_f_impl::_reset()
{
    unsigned int n;
    if (d_trigger_delay) {
        for (n = 0; n < d_nconnections; n++) {
            // Move the tail of the buffers to the front. This section
            // represents data that might have to be plotted again if a
            // trigger occurs and we have a trigger delay set.  The tail
            // section is between (d_end-d_trigger_delay) and d_end.
            memmove(d_fbuffers[n],
                    &d_fbuffers[n][d_end - d_trigger_delay],
                    d_trigger_delay * sizeof(float));

            // Also move the offsets of any tags that occur in the tail
            // section so they would be plotted again, too.
            std::vector<gr::tag_t> tmp_tags;
            for (size_t t = 0; t < d_tags[n].size(); t++) {
                if (d_tags[n][t].offset > (uint64_t)(d_size - d_trigger_delay)) {
                    d_tags[n][t].offset =
                        d_tags[n][t].offset - (d_size - d_trigger_delay);
                    tmp_tags.push_back(d_tags[n][t]);
                }
            }
            d_tags[n] = tmp_tags;
        }
    }
    // Otherwise, just clear the local list of tags.
    else {
        for (n = 0; n < d_nconnections; n++) {
            d_tags[n].clear();
        }
    }

    // Reset the start and end indices.
    d_start = 0;
    d_end = d_size;

    // Reset the trigger. If in free running mode, ignore the
    // trigger delay and always set trigger to true.
    if (d_trigger_mode == gr::qtgui::TRIG_MODE_FREE) {
        d_index = 0;
        d_triggered = true;
    } else {
        d_index = d_trigger_delay;
        d_triggered = false;
    }
}

void eye_sink_f_impl::_npoints_resize()
{
    int newsize = d_main_gui->getNPoints();
    set_nsamps(newsize);
}

void eye_sink_f_impl::_adjust_tags(int adj)
{
    for (size_t n = 0; n < d_tags.size(); n++) {
        for (size_t t = 0; t < d_tags[n].size(); t++) {
            d_tags[n][t].offset += adj;
        }
    }
}

void eye_sink_f_impl::_gui_update_trigger()
{
    d_trigger_mode = d_main_gui->getTriggerMode();
    d_trigger_slope = d_main_gui->getTriggerSlope();
    d_trigger_level = d_main_gui->getTriggerLevel();
    d_trigger_channel = d_main_gui->getTriggerChannel();
    d_trigger_count = 0;
    int d_sps = d_main_gui->getSamplesPerSymbol();

    float delayf = d_main_gui->getTriggerDelay();
    int delay = static_cast<int>(delayf * d_samp_rate);

    if (delay != d_trigger_delay) {
        // We restrict the delay to be within the window of time being
        // plotted.
        if ((delay < 0) || (delay > 2 * d_sps)) {
            GR_LOG_WARN(
                d_logger,
                boost::format("Trigger delay (%1%) outside of display range (0:%2%).") %
                    (delay / d_samp_rate) % ((2 * d_sps) / d_samp_rate));
            delay = std::max(0, std::min(2 * d_sps, delay));
            delayf = delay / d_samp_rate;
        }

        d_trigger_delay = delay;
        d_main_gui->setTriggerDelay(delayf);
        _reset();
    }

    std::string tagkey = d_main_gui->getTriggerTagKey();
    d_trigger_tag_key = pmt::intern(tagkey);
}

void eye_sink_f_impl::_test_trigger_tags(int nitems)
{
    int trigger_index;

    uint64_t nr = nitems_read(d_trigger_channel);
    std::vector<gr::tag_t> tags;
    get_tags_in_range(tags, d_trigger_channel, nr, nr + nitems + 1, d_trigger_tag_key);
    if (!tags.empty()) {
        trigger_index = tags[0].offset - nr;
        int start = d_index + trigger_index - d_trigger_delay - 1;
        if (start >= 0) {
            d_triggered = true;
            d_start = start;
            d_end = d_start + d_size;
            d_trigger_count = 0;
            _adjust_tags(-d_start);
        }
    }
}

void eye_sink_f_impl::_test_trigger_norm(int nitems, gr_vector_const_void_star inputs)
{
    int trigger_index;
    const float* in = (const float*)inputs[d_trigger_channel];
    for (trigger_index = 0; trigger_index < nitems; trigger_index++) {
        d_trigger_count++;

        // Test if trigger has occurred based on the input stream,
        // channel number, and slope direction
        if (_test_trigger_slope(&in[trigger_index])) {
            d_triggered = true;
            d_start = d_index + trigger_index - d_trigger_delay;
            d_end = d_start + d_size;
            d_trigger_count = 0;
            _adjust_tags(-d_start);
            break;
        }
    }

    // If using auto trigger mode, trigger periodically even
    // without a trigger event.
    if ((d_trigger_mode == gr::qtgui::TRIG_MODE_AUTO) && (d_trigger_count > d_size)) {
        d_triggered = true;
        d_trigger_count = 0;
    }
}

bool eye_sink_f_impl::_test_trigger_slope(const float* in) const
{
    float x0, x1;
    x0 = in[0];
    x1 = in[1];

    if (d_trigger_slope == gr::qtgui::TRIG_SLOPE_POS)
        return ((x0 <= d_trigger_level) && (x1 > d_trigger_level));
    else
        return ((x0 >= d_trigger_level) && (x1 < d_trigger_level));
}

int eye_sink_f_impl::work(int noutput_items,
                          gr_vector_const_void_star& input_items,
                          gr_vector_void_star& output_items)
{
    unsigned int n = 0, idx = 0;
    const float* in;

    _npoints_resize();
    _gui_update_trigger();

    gr::thread::scoped_lock lock(d_setlock);

    int nfill = d_end - d_index;                 // how much room left in buffers
    int nitems = std::min(noutput_items, nfill); // num items we can put in buffers

    // If auto, normal, or tag trigger, look for the trigger
    if ((d_trigger_mode != gr::qtgui::TRIG_MODE_FREE) && !d_triggered) {
        // trigger off a tag key (first one found)
        if (d_trigger_mode == gr::qtgui::TRIG_MODE_TAG) {
            _test_trigger_tags(nitems);
        }
        // Normal or Auto trigger
        else {
            _test_trigger_norm(nitems, input_items);
        }
    }

    // Copy data into the buffers.
    for (n = 0; n < d_nconnections; n++) {
        in = (const float*)input_items[idx];
        memcpy(&d_fbuffers[n][d_index], &in[1], nitems * sizeof(float));

        uint64_t nr = nitems_read(idx);
        std::vector<gr::tag_t> tags;
        get_tags_in_range(tags, idx, nr, nr + nitems);
        for (size_t t = 0; t < tags.size(); t++) {
            tags[t].offset = tags[t].offset - nr + (d_index - d_start - 1);
        }
        d_tags[idx].insert(d_tags[idx].end(), tags.begin(), tags.end());
        idx++;
    }
    d_index += nitems;

    // If we've have a trigger and a full d_size of items in the buffers, plot.
    if ((d_triggered) && (d_index == d_end)) {
        // Copy data to be plotted to start of buffers.
        for (n = 0; n < d_nconnections; n++) {
            volk_32f_convert_64f(d_buffers[n], &d_fbuffers[n][d_start], d_size);
        }

        // Plot if we are able to update
        if (gr::high_res_timer_now() - d_last_time > d_update_time) {
            d_last_time = gr::high_res_timer_now();
            d_qApplication->postEvent(d_main_gui,
                                      new TimeUpdateEvent(d_buffers, d_size, d_tags));
        }

        // We've plotting, so reset the state
        _reset();
    }

    // If we've filled up the buffers but haven't triggered, reset.
    if (d_index == d_end) {
        _reset();
    }

    return nitems;
}

void eye_sink_f_impl::handle_pdus(pmt::pmt_t msg)
{
    size_t len;
    pmt::pmt_t dict, samples;

    // Test to make sure this is either a PDU or a uniform vector of
    // samples. Get the samples PMT and the dictionary if it's a PDU.
    // If not, we throw an error and exit.
    if (pmt::is_pair(msg)) {
        dict = pmt::car(msg);
        samples = pmt::cdr(msg);
    } else if (pmt::is_uniform_vector(msg)) {
        samples = msg;
    } else {
        throw std::runtime_error("eye_sink_c: message must be either "
                                 "a PDU or a uniform vector of samples.");
    }

    len = pmt::length(samples);

    const float* in;
    if (pmt::is_f32vector(samples)) {
        in = (const float*)pmt::f32vector_elements(samples, len);
    } else {
        throw std::runtime_error("eye_sink_f: unknown data type "
                                 "of samples; must be float.");
    }

    // Plot if we're past the last update time
    if (gr::high_res_timer_now() - d_last_time > d_update_time) {
        d_last_time = gr::high_res_timer_now();

        set_nsamps(len);

        volk_32f_convert_64f(d_buffers[d_nconnections], in, len);

        std::vector<std::vector<gr::tag_t>> t;
        d_qApplication->postEvent(d_main_gui, new TimeUpdateEvent(d_buffers, len, t));
    }
}

} /* namespace qtgui */
} /* namespace gr */
