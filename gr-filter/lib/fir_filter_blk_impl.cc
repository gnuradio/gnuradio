/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "fir_filter_blk_impl.h"
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include <type_traits>

namespace gr {
namespace filter {

template <class IN_T, class OUT_T, class TAP_T>
typename fir_filter_blk<IN_T, OUT_T, TAP_T>::sptr
fir_filter_blk<IN_T, OUT_T, TAP_T>::make(int decimation, const std::vector<TAP_T>& taps)
{
    return gnuradio::make_block_sptr<fir_filter_blk_impl<IN_T, OUT_T, TAP_T>>(decimation,
                                                                              taps);
}


template <class IN_T, class OUT_T, class TAP_T>
fir_filter_blk_impl<IN_T, OUT_T, TAP_T>::fir_filter_blk_impl(
    int decimation, const std::vector<TAP_T>& taps)
    : sync_decimator("fir_filter_blk<IN_T,OUT_T,TAP_T>",
                     io_signature::make(1, 1, sizeof(IN_T)),
                     io_signature::make(1, 1, sizeof(OUT_T)),
                     decimation),
      d_fir(taps),
      d_updated(false),
      d_last_tag(0),
      d_tag_applied(false)
{
    this->set_history(d_fir.ntaps());

    const int alignment_multiple = volk_get_alignment() / sizeof(float);
    this->set_alignment(std::max(1, alignment_multiple));
}

template <class IN_T, class OUT_T, class TAP_T>
void fir_filter_blk_impl<IN_T, OUT_T, TAP_T>::set_taps(const std::vector<TAP_T>& taps)
{
    gr::thread::scoped_lock l(this->d_setlock);
    d_fir.set_taps(taps);
    d_updated = true;
}

template <class IN_T, class OUT_T, class TAP_T>
std::vector<TAP_T> fir_filter_blk_impl<IN_T, OUT_T, TAP_T>::taps() const
{
    return d_fir.taps();
}

template <class IN_T, class OUT_T, class TAP_T>
std::vector<TAP_T> fir_filter_blk_impl<IN_T, OUT_T, TAP_T>::taps_from_pmt(pmt::pmt_t v)
{
    size_t n = 0;
    if constexpr (std::is_same_v<TAP_T, gr_complex>) {
        const gr_complex* e = pmt::c32vector_elements(v, n);
        return std::vector<TAP_T>(e, e + n);
    } else {
        const float* e = pmt::f32vector_elements(v, n);
        return std::vector<TAP_T>(e, e + n);
    }
}

template <class IN_T, class OUT_T, class TAP_T>
int fir_filter_blk_impl<IN_T, OUT_T, TAP_T>::work(int noutput_items,
                                                  gr_vector_const_void_star& input_items,
                                                  gr_vector_void_star& output_items)
{
    gr::thread::scoped_lock l(this->d_setlock);

    const IN_T* in = (const IN_T*)input_items[0];
    OUT_T* out = (OUT_T*)output_items[0];

    if (d_updated) {
        this->set_history(d_fir.ntaps());
        d_updated = false;
        return 0; // history requirements may have changed.
    }

    // A "taps" tag replaces the taps from the tagged item on. Outputs whose
    // input starts before the tag use the old taps, so stop the work call there.
    std::vector<TAP_T> new_taps;
    bool have_new_taps = false;
    std::vector<tag_t> tags;
    this->get_tags_in_window(
        tags, 0, 0, noutput_items * this->decimation(), pmt::mp("taps"));
    for (const auto& tag : tags) {
        if (d_tag_applied && tag.offset <= d_last_tag) {
            continue;
        }
        d_last_tag = tag.offset;
        d_tag_applied = true;
        new_taps = taps_from_pmt(tag.value);
        have_new_taps = true;
        const uint64_t rel = tag.offset - this->nitems_read(0);
        if (rel == 0) {
            d_fir.set_taps(new_taps);
            this->set_history(d_fir.ntaps());
            return 0; // history requirements may have changed.
        }
        noutput_items = (rel + this->decimation() - 1) / this->decimation();
        break;
    }

    if (this->decimation() == 1) {
        d_fir.filterN(out, in, noutput_items);
    } else {
        d_fir.filterNdec(out, in, noutput_items, this->decimation());
    }

    if (have_new_taps) {
        d_fir.set_taps(new_taps);
        d_updated = true;
    }

    return noutput_items;
}
template class fir_filter_blk<gr_complex, gr_complex, gr_complex>;
template class fir_filter_blk<gr_complex, gr_complex, float>;
template class fir_filter_blk<float, gr_complex, gr_complex>;
template class fir_filter_blk<float, float, float>;
template class fir_filter_blk<float, std::int16_t, float>;
template class fir_filter_blk<std::int16_t, gr_complex, gr_complex>;
} /* namespace filter */
} /* namespace gr */
