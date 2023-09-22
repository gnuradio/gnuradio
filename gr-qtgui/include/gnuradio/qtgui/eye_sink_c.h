/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_QTGUI_EYE_SINK_C_H
#define INCLUDED_QTGUI_EYE_SINK_C_H

#include <gnuradio/qtgui/api.h>
#include <gnuradio/qtgui/trigger_mode.h>
#include <gnuradio/sync_block.h>
#include <qapplication.h>

namespace gr {
namespace qtgui {

/*!
 * \brief A graphical sink to display signals eye patterns.
 * \ingroup qtgui
 *
 * \details
 * This is a QT-based graphical sink which takes a set of a complex
 * streams and plots them as an eye pattern. For each signal, both
 * the signal's I and Q eye patterns are plotted. Eye patterns are
 * 2 symbol's time long. Symbol rate must be an integer multiple of
 * the sample rate to obtain the eye pattern.
 *
 * Trigger occurs at the beginning of each stream used to plot the
 * eye pattern; whilst a real eye diagram would be triggered with
 * a (recovered) symbol clock. For these reasons, triggering of
 * noisy and/or unsynchronized signals may lead to incorrect eye
 * pattern.
 *
 * The sink supports plotting streaming complex data or
 * messages. The message port is named "in". The two modes cannot
 * be used simultaneously, and \p nconnections should be set to 0
 * when using the message mode. GRC handles this issue by
 * providing the "Complex Message" type that removes the streaming
 * port(s).
 *
 * This sink can plot messages that contain either uniform vectors
 * of complex 32 values (pmt::is_c32vector) or PDUs where the data
 * is a uniform vector of complex 32 values.
 */
class QTGUI_API eye_sink_c : virtual public gr::sync_block
{
public:
    // gr::qtgui::eye_sink_c::sptr
    typedef std::shared_ptr<eye_sink_c> sptr;

    /*!
     * \brief Build complex eye sink
     *
     * \param size number of points to plot at once
     * \param samp_rate sample rate (used to set x-axis labels)
     * \param nconnections number of signals connected to sink
     * \param parent a QWidget parent object, if any
     */
    static sptr make(int size,
                     double samp_rate,
                     unsigned int nconnections = 1,
                     QWidget* parent = NULL);

    virtual void exec_() = 0;
    virtual QWidget* qwidget() = 0;

    virtual void set_y_axis(double min, double max) = 0;
    virtual void set_y_label(const std::string& label, const std::string& unit = "") = 0;
    virtual void set_update_time(double t) = 0;
    virtual void set_samp_per_symbol(unsigned int sps) = 0;
    virtual void set_line_label(unsigned int which, const std::string& label) = 0;
    virtual void set_line_color(unsigned int which, const std::string& color) = 0;
    virtual void set_line_width(unsigned int which, int width) = 0;
    virtual void set_line_style(unsigned int which, int style) = 0;
    virtual void set_line_marker(unsigned int which, int marker) = 0;
    virtual void set_nsamps(const int newsize) = 0;
    virtual void set_samp_rate(const double samp_rate) = 0;
    virtual void set_line_alpha(unsigned int which, double alpha) = 0;

    /*!
     * Set up a trigger for the sink to know when to start
     * plotting. Useful to isolate events and avoid noise.
     *
     * The trigger modes are Free, Auto, Normal, and Tag (see
     * gr::qtgui::trigger_mode). The first three are like a normal
     * oscope trigger function. Free means free running with no
     * trigger, auto will trigger if the trigger event is seen, but
     * will still plot otherwise, and normal will hold until the
     * trigger event is observed. The Tag trigger mode allows us to
     * trigger off a specific stream tag. The tag trigger is based
     * only on the name of the tag, so when a tag of the given name
     * is seen, the trigger is activated.
     *
     * In auto and normal mode, we look for the slope of the of the
     * signal. Given a gr::qtgui::trigger_slope as either Positive
     * or Negative, if the value between two samples moves in the
     * given direction (x[1] > x[0] for Positive or x[1] < x[0] for
     * Negative), then the trigger is activated.
     *
     * With the complex eye sink, each input has two eye patterns
     * drawn for the real and imaginary parts of the signal. When
     * selecting the \p channel value, channel 0 is the real signal
     * and channel 1 is the imaginary signal. For more than 1 input
     * stream, channel 2i is the real part of the ith input and
     * channel (2i+1) is the imaginary part of the ith input
     * channel.
     *
     * The \p delay value is specified in time based off the sample
     * rate. If the sample rate of the block is set to 1, the delay
     * is then also the sample number offset. This is the offset
     * from the left-hand y-axis of the plot. It delays the signal
     * to show the trigger event at the given delay along with some
     * portion of the signal before the event. The delay must be
     * within 0 - t_max where t_max is the maximum amount of time
     * displayed on the eye pattern equal to 2 symbol time.
     *
     * \param mode The trigger_mode: free, auto, normal, or tag.
     * \param slope The trigger_slope: positive or negative. Only
     *              used for auto and normal modes.
     * \param level The magnitude of the trigger even for auto or normal modes.
     * \param delay The delay (in units of time) for where the trigger happens.
     * \param channel Which input channel to use for the trigger events.
     * \param tag_key The name (as a string) of the tag to trigger off
     *                 of if using the tag mode.
     */
    virtual void set_trigger_mode(gr::qtgui::trigger_mode mode,
                                  gr::qtgui::trigger_slope slope,
                                  float level,
                                  float delay,
                                  int channel,
                                  const std::string& tag_key = "") = 0;

    virtual std::string title() = 0;
    virtual std::string line_label(unsigned int which) = 0;
    virtual std::string line_color(unsigned int which) = 0;
    virtual int line_width(unsigned int which) = 0;
    virtual int line_style(unsigned int which) = 0;
    virtual int line_marker(unsigned int which) = 0;
    virtual double line_alpha(unsigned int which) = 0;

    virtual void set_size(int width, int height) = 0;

    virtual void enable_menu(bool en = true) = 0;
    virtual void enable_grid(bool en = true) = 0;
    virtual void enable_autoscale(bool en = true) = 0;
    virtual void
    enable_stem_plot(bool en = true) = 0; // Used by parent class, do not remove
    virtual void
    enable_semilogx(bool en = true) = 0; // Used by parent class, do not remove
    virtual void
    enable_semilogy(bool en = true) = 0; // Used by parent class, do not remove
    virtual void enable_control_panel(bool en = true) = 0;
    virtual void enable_tags(unsigned int which, bool en) = 0;
    virtual void enable_tags(bool en) = 0;
    virtual void enable_axis_labels(bool en = true) = 0;
    virtual void disable_legend() = 0;

    virtual int nsamps() const = 0;
    virtual void reset() = 0;

    QApplication* d_qApplication;
};

} // namespace qtgui
} // namespace gr

#endif /* INCLUDED_QTGUI_EYE_SINK_C_H */
