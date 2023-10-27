/* -*- c++ -*- */
/*
 * Copyright 2015 Ettus Research
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rfnoc_f15_display_impl.h"

#include "QRfnocF15Surface.h"
#include <QApplication>
#include <QWidget>

namespace gr {
namespace qtgui {

rfnoc_f15_display::sptr rfnoc_f15_display::make(const int fft_bins,
                                                const int pwr_bins,
                                                const int wf_lines,
                                                QWidget* parent)
{
    return gnuradio::make_block_sptr<rfnoc_f15_display_impl>(
        fft_bins, pwr_bins, wf_lines, parent);
}

rfnoc_f15_display_impl::rfnoc_f15_display_impl(const int fft_bins,
                                               const int pwr_bins,
                                               const int wf_lines,
                                               QWidget* parent)
    : gr::block("rfnoc_f15_display",
                gr::io_signature::make(1, 2, fft_bins * sizeof(uint8_t)),
                gr::io_signature::make(0, 0, 0)),
      d_fft_bins(fft_bins),
      d_pwr_bins(pwr_bins),
      d_wf_lines(wf_lines),
      d_subframe_num(pwr_bins + 2)
{
    /* Message Port output */
    message_port_register_out(pmt::mp("cfg"));

    /* Frame buffer */
    d_frame = new uint8_t[fft_bins * d_subframe_num];

    /* QT stuff */
    if (qApp != NULL) {
        d_qApplication = qApp;
    } else {
        d_qApplication = new QApplication(d_argc, &d_argv);
    }

    d_gui = new QRfnocF15Surface(fft_bins, pwr_bins, wf_lines, parent);
    d_gui->setFocusPolicy(Qt::StrongFocus);
    d_gui->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

rfnoc_f15_display_impl::~rfnoc_f15_display_impl() { delete d_frame; }


void rfnoc_f15_display_impl::set_frequency_range(const double center_freq,
                                                 const double samp_rate)
{
    pmt::pmt_t msg = pmt::make_dict();

    if ((d_center_freq == center_freq) && (d_samp_rate == samp_rate))
        return;

    if ((d_samp_rate != samp_rate) && (samp_rate > 0.0) && (d_frame_rate > 0)) {
        int decim;

        decim = (int)(d_samp_rate / (d_fft_bins * d_pwr_bins * d_frame_rate));
        if (decim < 2)
            decim = 2;

        msg = pmt::dict_add(msg, pmt::string_to_symbol("decim"), pmt::from_long(decim));
    }

    d_gui->setFrequencyRange(center_freq, samp_rate);

    d_center_freq = center_freq;
    d_samp_rate = samp_rate;

    msg = pmt::dict_add(msg, pmt::string_to_symbol("clear"), pmt::from_long(1));

    message_port_pub(pmt::mp("cfg"), msg);
}

void rfnoc_f15_display_impl::set_waterfall(bool enabled) { d_gui->setWaterfall(enabled); }

void rfnoc_f15_display_impl::set_grid(bool enabled) { d_gui->setGrid(enabled); }

void rfnoc_f15_display_impl::set_palette(const std::string& name)
{
    d_gui->setPalette(name);
}

void rfnoc_f15_display_impl::set_frame_rate(int fps) { d_frame_rate = fps; }


bool rfnoc_f15_display_impl::start()
{
    pmt::pmt_t msg = pmt::make_dict();

    {
        int decim;

        decim = (int)(d_samp_rate / (d_fft_bins * d_pwr_bins * d_frame_rate));
        if (decim < 2)
            decim = 2;

        msg = pmt::dict_add(msg, pmt::string_to_symbol("decim"), pmt::from_long(decim));
    }

    msg = pmt::dict_add(msg, pmt::string_to_symbol("clear"), pmt::from_long(1));

    message_port_pub(pmt::mp("cfg"), msg);
    return true;
}

void rfnoc_f15_display_impl::forecast(int noutput_items,
                                      gr_vector_int& ninput_items_required)
{
    /* Need at least one item */
    ninput_items_required[0] = 1;

    /* Don't wait on WaterFall data */
    if (ninput_items_required.size() > 1)
        ninput_items_required[1] = 0;
}

int rfnoc_f15_display_impl::general_work(int noutput_items,
                                         gr_vector_int& ninput_items,
                                         gr_vector_const_void_star& input_items,
                                         gr_vector_void_star& output_items)
{
    int rv;

    /* Consume histogram */
    rv = _work_hist((const uint8_t*)input_items[0], ninput_items[0], 0);

    if (rv > 0)
        consume(0, rv);

    /* Consume waterfall, if available */
    if (ninput_items.size() > 1) {
        rv = _work_wf((const uint8_t*)input_items[1], ninput_items[1], 1);

        if (rv > 0) {
            consume(1, rv);
        }
    }

    return 0;
}

int rfnoc_f15_display_impl::_work_hist(const uint8_t* input, int n_items, int port)
{
    static const pmt::pmt_t EOB_KEY = pmt::string_to_symbol("rx_eob");
    std::vector<tag_t> v;
    const uint64_t nR = nitems_read(port);
    // End-of-frame index, relative to the input buffer
    int pEOF;

    /* Anything to do ? */
    if (n_items < 1)
        return 0;

    /* Grab all tags available, we'll need them either way */
    get_tags_in_range(v, port, nR, nR + n_items, EOB_KEY);

    /* If not aligned we just search for EOF */
    if (!d_aligned) {
        /* No tags ?  Drop all useless data while we wait for one */
        if (v.empty())
            return n_items;

        /* Start at zero right after the tag */
        d_subframe = 0;
        d_aligned = true;

        return v[0].offset - nR + 1;
    }

    /* Check alignment */
    pEOF = v.empty() ? -1 : (v[0].offset - nR);

    if (n_items >= (d_subframe_num - d_subframe)) {
        /* We have the end of the frame, must be an EOF on that */
        if (pEOF != (d_subframe_num - d_subframe - 1)) {
            d_aligned = false;
            return 0;
        }
    } else {
        /* We don't have the end of the frame yet, there can't be any EOF */
        if (pEOF >= 0) {
            d_aligned = false;
            return 0;
        }
    }

    /* Limit to expected frame boundary */
    if (n_items > (d_subframe_num - d_subframe))
        n_items = d_subframe_num - d_subframe;

    /* Copy the data */
    memcpy(&d_frame[d_fft_bins * d_subframe], input, n_items * d_fft_bins);

    d_subframe += n_items;

    /* Are we done ? */
    if (d_subframe == d_subframe_num) {
        /* Send the frame to the display surface */
        d_gui->sendFrame(d_frame, d_subframe_num * d_fft_bins);

        /* Start over */
        d_subframe = 0;
    }

    return n_items;
}

int rfnoc_f15_display_impl::_work_wf(const uint8_t* input, int n_items, int port)
{
    /* Anything to do ? */
    if (n_items < 1)
        return 0;

    /* Send them to the display surface */
    d_gui->sendWaterfall(input, n_items);

    return n_items;
}


void rfnoc_f15_display_impl::exec_() { d_qApplication->exec(); }

QWidget* rfnoc_f15_display_impl::qwidget() { return dynamic_cast<QWidget*>(d_gui); }

} // namespace qtgui
} // namespace gr
