/* -*- c++ -*- */
/*
 * Copyright 2015 Ettus Research
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_QTGUI_FOSPHOR_DISPLAY_IMPL_H
#define INCLUDED_QTGUI_FOSPHOR_DISPLAY_IMPL_H

#include <gnuradio/qtgui/fosphor_display.h>
#include <cstdint>

namespace gr {
namespace qtgui {

class QFosphorSurface;

/*!
 * \brief QT GUI Display block for RFNoC fosphor (implementation)
 * \ingroup ettus
 */
class QTGUI_API fosphor_display_impl : public fosphor_display
{
public:
    fosphor_display_impl(const int fft_bins = 256,
                         const int pwr_bins = 64,
                         const int wf_lines = 512,
                         QWidget* parent = NULL);
    ~fosphor_display_impl() override;

    /* Block API */
    void set_frequency_range(const double center_freq, const double samp_rate) override;
    void set_waterfall(bool enabled) override;
    void set_grid(bool enabled) override;
    void set_palette(const std::string& name) override;
    void set_frame_rate(int fps) override;

    /* gr::block implementation */
    bool start() override;

    void forecast(int noutput_items, gr_vector_int& ninput_items_required) override;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    /* QT GUI Widget stuff */
    void exec_() override;
    QWidget* qwidget() override;

private:
    int _work_hist(const uint8_t* input, int n_items, int port);
    int _work_wf(const uint8_t* input, int n_items, int port);

    QFosphorSurface* d_gui;

    int d_fft_bins;
    int d_pwr_bins;
    int d_wf_lines;

    double d_center_freq = 0.0;
    double d_samp_rate = 0.0;
    int d_frame_rate = 0;

    bool d_aligned = false;
    int d_subframe = 0;
    //! Store the number of subframes per histogram frame
    const int d_subframe_num = 0;
    uint8_t* d_frame;
};

} // namespace qtgui
} // namespace gr

#endif /* INCLUDED_QTGUI_FOSPHOR_DISPLAY_IMPL_H */
