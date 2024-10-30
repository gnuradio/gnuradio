/* -*- c++ -*- */
/*
 * Copyright 2015 Ettus Research
 * Copyright 2020 Ettus Research, LLC. A National Instruments Brand
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_QTGUI_RFNOC_F15_DISPLAY_H
#define INCLUDED_QTGUI_RFNOC_F15_DISPLAY_H

#ifdef ENABLE_PYTHON
#include <Python.h>
#endif

#include <gnuradio/block.h>
#include <gnuradio/qtgui/api.h>
#include <qapplication.h>
#include <string>

namespace gr {
namespace qtgui {

/*! FFT Histogram/Waterfall display widget for RFNoC fosphor display
 *
 * This block can consume one or two inputs, each vectors of unsigned 8-bit
 * numbers. The vector length is \p fft_bins for both inputs.
 *
 * The histogram display is a 2D display widget which does not only plot the
 * current (average) FFT, but also the max-hold values for the FFT, and a
 * histogram of the FFT (also averaged over time). For every FFT bin, it plots
 * a histogram of power level distributions.
 *
 * The first input contains the histogram data for the FFT histogram. It expects
 * data in the following order:
 * - \p pwr_bins vectors of length \p fft_bins containing the histogram data for
 *   each FFT bin.
 * - One vector of length \p fft_bins containing the max values.
 * - One vector of length \p fft_bins containing the average values. After this
 *   vector, we expect an end-of-frame tag. This means end-of-frame tags on this
 *   input should be repeating every \p pwr_bins + 2 vectors.
 *
 * Averaging over time must be done by the upstream blocks. This block only does
 * the display, none of the calculations.
 *
 * The second input contains FFT data for a waterfall display. It is simply a
 * concatenation of FFT magnitude vectors (8-bit integers).
 *
 * \ingroup block_qtgui_rfnoc_f15_display
 */
class QTGUI_API rfnoc_f15_display : virtual public gr::block
{
public:
    typedef std::shared_ptr<rfnoc_f15_display> sptr;

    /*! Create an instance of an RFNoC fosphor display
     *
     * \param fft_bins Number of FFT bins. This is also the vector length of the
     *                 inputs, and the x-axis dimension on both the histogram
     *                 plot and the waterfall plot (if enabled).
     * \param pwr_bins The number of power bins histograms per FFT bin. This is
     *                 also the y-axis dimension of the histogram plot. This
     *                 must match the input provided (see block description).
     * \param wf_lines The depth of the waterfall plot history (or the y-axis
     *                 dimension of waterfall plot).
     * \param parent The QWidget parent, if any.
     *
     * \brief Return a shared_ptr to a new instance of ettus::rfnoc_f15_display
     */
    static sptr make(const int fft_bins = 256,
                     const int pwr_bins = 64,
                     const int wf_lines = 512,
                     QWidget* parent = nullptr);

    /*** Block API ***********************************************************/
    virtual void set_frequency_range(const double center_freq,
                                     const double samp_rate) = 0;
    virtual void set_waterfall(bool enabled) = 0;
    virtual void set_grid(bool enabled) = 0;
    virtual void set_palette(const std::string& name) = 0;
    virtual void set_frame_rate(int fps) = 0;

    /*** QT GUI Widget stuff *************************************************/
    virtual void exec_() = 0;
    virtual QWidget* qwidget() = 0;

    QApplication* d_qApplication;
};

} // namespace qtgui
} // namespace gr

#endif /* INCLUDED_QTGUI_RFNOC_F15_DISPLAY_H */
