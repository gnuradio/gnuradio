/*
 * Copyright 2011 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#define LV_HAVE_GENERIC
#include <volk/volk_32fc_x2_dot_prod_32fc_a.h>

#include <gr_filter_decim_fir.h>
#include <gr_io_signature.h>
#include <gruel/thread.h>
#include <algorithm>
#include <stdexcept>
#include <volk/volk.h>
#include <iostream>
#include <boost/shared_array.hpp>

/***********************************************************************
 * FIR filter FC32 implementation
 **********************************************************************/
class gr_filter_decim_fir_fc32 : public gr_filter_decim_fir{
public:
    typedef std::complex<float> type;

    gr_filter_decim_fir_fc32(const size_t decim):
        gr_sync_decimator(
            "FIR filter FC32",
            gr_make_io_signature (1, 1, sizeof(type)),
            gr_make_io_signature (1, 1, sizeof(type)),
            decim
        )
    {
        const int alignment_multiple = volk_get_alignment() / sizeof(type);
        set_output_multiple(std::max(16, alignment_multiple));
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        gruel::scoped_lock lock(_taps_mutex);
        type *out = reinterpret_cast<type *>(output_items[0]);
        const type *in = reinterpret_cast<const type *>(input_items[0]);
        const size_t num_bytes = this->history() * sizeof(type);

        for (size_t i = 0; i < size_t(noutput_items); i++){
            volk_32fc_x2_conjugate_dot_prod_32fc_a(out, in, _taps, num_bytes);
            out++;
            in += this->decimation();
        }

        return noutput_items;
    }

    void set_taps(const std::vector<std::complex<double> > &taps_){
        gruel::scoped_lock lock(_taps_mutex);

        //copy the new taps in and update history
        std::vector<type> taps(taps_.size());
        for (size_t i = 0; i < taps.size(); i++){
            taps[i] = type(taps_[i]);
        }
        if (taps.size() % 2 == 1){ //pad to 2x multiple because volk
            taps.push_back(0.0);
        }
        std::reverse(taps.begin(), taps.end());
        this->set_history(taps.size());

        //copy taps in aligned memory
        //TODO the blob work can easily allocate managed aligned memory (so use that when its merged)
        const size_t num_bytes = this->history() * sizeof(type);
        const size_t align_pad = volk_get_alignment() - 1;
        _taps_mem = boost::shared_array<char>(new char[num_bytes + align_pad]);
        _taps = reinterpret_cast<type *>(size_t(_taps_mem.get() + align_pad) & ~align_pad);
        std::memcpy(_taps, &taps.front(), num_bytes);
    }

private:
    gruel::mutex _taps_mutex;
    boost::shared_array<char> _taps_mem;
    type *_taps;
};

/***********************************************************************
 * FIR filter factory function
 **********************************************************************/
gr_filter_decim_fir::sptr gr_filter_decim_fir::make(filter_type type, const size_t decim){
    switch(type){
    case FILTER_FC32_IO_FC32_TAPS: return sptr(new gr_filter_decim_fir_fc32(decim));
    case FILTER_F32_IO_F32_TAPS://TODO
    default: throw std::invalid_argument("make FIR filter got unknown type");
    }
}
