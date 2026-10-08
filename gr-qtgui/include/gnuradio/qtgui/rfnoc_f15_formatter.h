/* -*- c++ -*- */
/*
 * Copyright 2023 Ettus Research, A National Instruments Brand
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_QTGUI_RFNOC_F15_FORMATTER_H
#define INCLUDED_QTGUI_RFNOC_F15_FORMATTER_H

#include <gnuradio/hier_block2.h>
#include <gnuradio/qtgui/api.h>

namespace gr {
namespace qtgui {

/*!
 * \brief Formatter for the RFNoC F15 display
 */
class QTGUI_API rfnoc_f15_formatter : virtual public hier_block2
{
public:
    using sptr = std::shared_ptr<rfnoc_f15_formatter>;

    ~rfnoc_f15_formatter() override {}

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

#endif /* INCLUDED_QTGUI_RFNOC_F15_FORMATTER_H */