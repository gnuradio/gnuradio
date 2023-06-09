/* -*- c++ -*- */
/*
 * Copyright 2023 Ettus Research, A National Instruments Brand
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_QTGUI_FOSPHOR_FORMATTER_H
#define INCLUDED_QTGUI_FOSPHOR_FORMATTER_H

#include <gnuradio/block.h>
#include <gnuradio/qtgui/api.h>

namespace gr {
namespace qtgui {

/*!
 * \brief Generic CPM modulator
 * \ingroup modulators_blk
 */
class QTGUI_API fosphor_formatter : virtual public block
{
public:
    using sptr = std::shared_ptr<fosphor_formatter>;

    ~fosphor_formatter() override {}

    /*!
     * Make formatter block for fosphor_display
     *
     */
    static sptr make(int fft_size,
                     int num_bins,
                     int input_decim,
                     int waterfall_decim,
                     int histo_decim,
                     double scale,
                     double alpha,
                     double epsilon,
                     double trise,
                     double tdecay);
};

} /* namespace qtgui */
} /* namespace gr */

#endif /* INCLUDED_QTGUI_FOSPHOR_FORMATTER_H */
