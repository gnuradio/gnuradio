/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "chunks_to_symbols_impl.h"
#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>
#include <cassert>

namespace gr {
namespace digital {


void set_vector_from_pmt(std::vector<gr_complex>& symbol_table,
                         const pmt::pmt_t& symbol_table_pmt)
{
    size_t length;
    const gr_complex* elements = pmt::c32vector_elements(symbol_table_pmt, length);
    symbol_table.assign(elements, elements + length);
}

void set_vector_from_pmt(std::vector<float>& symbol_table,
                         const pmt::pmt_t& symbol_table_pmt)
{
    size_t length;
    const float* elements = pmt::f32vector_elements(symbol_table_pmt, length);
    symbol_table.assign(elements, elements + length);
}

template <class IN_T, class OUT_T>
typename chunks_to_symbols<IN_T, OUT_T>::sptr
chunks_to_symbols<IN_T, OUT_T>::make(const std::vector<OUT_T>& symbol_table,
                                     const unsigned int D)
{
    return gnuradio::make_block_sptr<chunks_to_symbols_impl<IN_T, OUT_T>>(symbol_table,
                                                                          D);
}

template <class IN_T, class OUT_T>
chunks_to_symbols_impl<IN_T, OUT_T>::chunks_to_symbols_impl(
    const std::vector<OUT_T>& symbol_table, const unsigned int D)
    : sync_interpolator("chunks_to_symbols",
                        io_signature::make(1, -1, sizeof(IN_T)),
                        io_signature::make(1, -1, sizeof(OUT_T)),
                        D),
      d_D(D),
      d_symbol_table(symbol_table),
      symbol_table_key(pmt::mp("set_symbol_table"))
{
    this->message_port_register_in(symbol_table_key);
    this->set_msg_handler(symbol_table_key, [this](const pmt::pmt_t& msg) {
        this->handle_set_symbol_table(msg);
    });
}

template <class IN_T, class OUT_T>
chunks_to_symbols_impl<IN_T, OUT_T>::~chunks_to_symbols_impl()
{
}

template <class IN_T, class OUT_T>
void chunks_to_symbols_impl<IN_T, OUT_T>::handle_set_symbol_table(
    const pmt::pmt_t& symbol_table_pmt)
{
    set_vector_from_pmt(d_symbol_table, symbol_table_pmt);
}


template <class IN_T, class OUT_T>
void chunks_to_symbols_impl<IN_T, OUT_T>::set_symbol_table(
    const std::vector<OUT_T>& symbol_table)
{
    gr::thread::scoped_lock lock(this->d_setlock);
    d_symbol_table = symbol_table;
}

template <class IN_T, class OUT_T>
int chunks_to_symbols_impl<IN_T, OUT_T>::work(int noutput_items,
                                              gr_vector_const_void_star& input_items,
                                              gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock lock(this->d_setlock);
    auto nstreams = input_items.size();
    for (unsigned int m = 0; m < nstreams; m++) {
        const auto* in = reinterpret_cast<const IN_T*>(input_items[m]);
        // NB: The compiler can't know whether all specializations of `chunks_to_symbol`
        // are subclasses of gr::block. Hence the "this->" hoop we have to jump through to
        // access members of parent classes.
        size_t in_count = this->nitems_read(m);

        auto out = reinterpret_cast<OUT_T*>(output_items[m]);

        std::vector<tag_t> tags;
        this->get_tags_in_range(
            tags, m, this->nitems_read(m), this->nitems_read(m) + noutput_items / d_D);

        // per tag: all the samples leading up to this tag can be handled straightforward
        // with the current settings
        if (d_D == 1) {
            for (const auto& tag : tags) {
                for (; in_count < tag.offset; ++in_count) {
                    auto key = static_cast<unsigned int>(*in);
                    *out = d_symbol_table[key];
                    ++out;
                    ++in;
                }
                if (tag.key == symbol_table_key) {
                    handle_set_symbol_table(tag.value);
                }
            }

            // after the last tag, continue working on the remaining items
            for (; in < reinterpret_cast<const IN_T*>(input_items[m]) + noutput_items;
                 ++in) {
                auto key = static_cast<unsigned int>(*in);
                *out = d_symbol_table[key];
                ++out;
            }
        } else { // the multi-dimensional case
            for (const auto& tag : tags) {
                for (; in_count < tag.offset; ++in_count) {
                    auto key = static_cast<unsigned int>(*in) * d_D;
                    for (unsigned int idx = 0; idx < d_D; ++idx) {
                        *out = d_symbol_table[key + idx];
                        ++out;
                    }
                    ++in;
                }
                if (tag.key == symbol_table_key) {
                    handle_set_symbol_table(tag.value);
                }
            }

            // after the last tag, continue working on the remaining items
            for (;
                 in < reinterpret_cast<const IN_T*>(input_items[m]) + noutput_items / d_D;
                 ++in) {
                auto key = static_cast<unsigned int>(*in) * d_D;
                for (unsigned int idx = 0; idx < d_D; ++idx) {
                    *out = d_symbol_table[key + idx];
                    ++out;
                }
            }
        }
    }
    return noutput_items;
}
template class chunks_to_symbols<std::uint8_t, float>;
template class chunks_to_symbols<std::uint8_t, gr_complex>;
template class chunks_to_symbols<std::int16_t, float>;
template class chunks_to_symbols<std::int16_t, gr_complex>;
template class chunks_to_symbols<std::int32_t, float>;
template class chunks_to_symbols<std::int32_t, gr_complex>;

} /* namespace digital */
} /* namespace gr */
