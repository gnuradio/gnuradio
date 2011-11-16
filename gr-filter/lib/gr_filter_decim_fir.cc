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

#include <gr_filter_decim_fir.h>
#include <gr_io_signature.h>
#include <gruel/thread.h>
#include <algorithm>
#include <stdexcept>
#include <volk/volk.h>
#include <iostream>

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
        //NOP
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        gruel::scoped_lock lock(_taps_mutex);
        const size_t num_taps = this->history();
        type *out = reinterpret_cast<type *>(output_items[0]);
        const type *in = reinterpret_cast<const type *>(input_items[0]);
        const type *taps = reinterpret_cast<const type *>(&_taps.front());

        for (size_t i = 0; i < size_t(noutput_items); i++){
            volk_32fc_x2_dot_prod_32fc_u(out+i, in, taps, num_taps);
            in += this->decimation();
        }

        return noutput_items;
    }

    void set_taps(const std::vector<std::complex<double> > &taps){
        gruel::scoped_lock lock(_taps_mutex);

        //copy the new taps in and update history
        _taps.resize(taps.size());
        for (size_t i = 0; i < taps.size(); i++){
            _taps[i] = type(taps[i]);
        }
        std::reverse(_taps.begin(), _taps.end());
        this->set_history(_taps.size());
    }

private:
    gruel::mutex _taps_mutex;
    std::vector<type> _taps;
};

/***********************************************************************
 * FIR filter F32 implementation
 **********************************************************************/
class gr_filter_decim_fir_f32 : public gr_filter_decim_fir{
public:
    typedef float type;

    gr_filter_decim_fir_f32(const size_t decim):
        gr_sync_decimator(
            "FIR filter F32",
            gr_make_io_signature (1, 1, sizeof(type)),
            gr_make_io_signature (1, 1, sizeof(type)),
            decim
        )
    {
        //NOP
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        gruel::scoped_lock lock(_taps_mutex);
        const size_t num_taps = this->history();
        type *out = reinterpret_cast<type *>(output_items[0]);
        const type *in = reinterpret_cast<const type *>(input_items[0]);
        const type *taps = reinterpret_cast<const type *>(&_taps.front());

        for (size_t i = 0; i < size_t(noutput_items); i++){
            volk_32f_x2_dot_prod_32f_u(out+i, in, taps, num_taps);
            in += this->decimation();
        }

        return noutput_items;
    }

    void set_taps(const std::vector<std::complex<double> > &taps){
        gruel::scoped_lock lock(_taps_mutex);

        //copy the new taps in and update history
        _taps.resize(taps.size());
        for (size_t i = 0; i < taps.size(); i++){
            _taps[i] = type(taps[i].real());
        }
        std::reverse(_taps.begin(), _taps.end());
        this->set_history(_taps.size());
    }

private:
    gruel::mutex _taps_mutex;
    std::vector<type> _taps;
};

/***********************************************************************
 * FIR filter factory function
 **********************************************************************/
gr_filter_decim_fir::sptr gr_filter_decim_fir::make(filter_type type, const size_t decim){
    switch(type){
    case FILTER_FC32_IO_FC32_TAPS: return sptr(new gr_filter_decim_fir_fc32(decim));
    case FILTER_F32_IO_F32_TAPS:   return sptr(new gr_filter_decim_fir_f32(decim));
    default: throw std::invalid_argument("make FIR filter got unknown type");
    }
}
