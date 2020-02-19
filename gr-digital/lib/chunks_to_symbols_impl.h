/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef CHUNKS_TO_SYMBOLS_IMPL_H
#define CHUNKS_TO_SYMBOLS_IMPL_H

#include <gnuradio/digital/chunks_to_symbols.h>

namespace gr {
namespace digital {

template <class IN_T, class OUT_T>
class chunks_to_symbols_impl : public chunks_to_symbols<IN_T, OUT_T>
{
private:
    const int d_D;
    std::vector<OUT_T> d_symbol_table;

public:
    chunks_to_symbols_impl(const std::vector<OUT_T>& symbol_table, const int D = 1);

    ~chunks_to_symbols_impl();

    void handle_set_symbol_table(pmt::pmt_t symbol_table_pmt);
    void set_symbol_table(const std::vector<OUT_T>& symbol_table);

    int D() const { return d_D; }
    std::vector<OUT_T> symbol_table() const { return d_symbol_table; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    bool check_topology(int ninputs, int noutputs) { return ninputs == noutputs; }
};

} /* namespace digital */
} /* namespace gr */

#endif /* CHUNKS_TO_SYMBOLS_IMPL_H */
