/* -*- c++ -*- */
/*
 * Copyright 2015 Ettus Research
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_QTGUI_rfnoc_f15_DISPLAY_IMPL_H
#define INCLUDED_QTGUI_rfnoc_f15_DISPLAY_IMPL_H

#include <gnuradio/qtgui/rfnoc_f15_display.h>
#include <cstdint>

namespace gr {
namespace qtgui {

class QRfnocF15Surface;

/*!
 * \brief QT GUI Display block for RFNoC fosphor (implementation)
 * \ingroup ettus
 */
class QTGUI_API rfnoc_f15_display_impl : public rfnoc_f15_display
{
public:
    rfnoc_f15_display_impl(const int fft_bins = 256,
                           const int pwr_bins = 64,
                           const int wf_lines = 512,
                           QWidget* parent = NULL);
    ~rfnoc_f15_display_impl() override;

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

    QRfnocF15Surface* d_gui;

    int d_fft_bins;
    int d_pwr_bins;

    double d_center_freq = 0.0;
    double d_samp_rate = 0.0;
    int d_frame_rate = 0;

    bool d_aligned = false;
    int d_subframe = 0;
    //! Store the number of subframes per histogram frame
    const int d_subframe_num = 0;
    uint8_t* d_frame;
    // Required now for Qt; argc must be greater than 0 and argv
    // must have at least one valid character. Must be valid through
    // life of the qApplication:
    // http://harmattan-dev.nokia.com/docs/library/html/qt4/qapplication.html
    char d_zero = 0;
    int d_argc = 1;
    char* d_argv = &d_zero;
};

} // namespace qtgui
} // namespace gr

#endif /* INCLUDED_QTGUI_rfnoc_f15_DISPLAY_IMPL_H */
