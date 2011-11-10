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

#include <gr_basic_sig_source.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <complex>
#include <cmath>
#include <boost/math/special_functions/round.hpp>

static const size_t wave_table_size = 4096;

/***********************************************************************
 * Helper routines for conversion
 **********************************************************************/
template <typename type> void conv(const std::complex<double> &in, std::complex<type> &out){
    out = std::complex<type>(in);
}

template <typename type> void conv(const std::complex<double> &in, type &out){
    out = type(in.real());
}

/***********************************************************************
 * Generic add const implementation
 **********************************************************************/
template <typename type>
class gr_basic_sig_source_impl : public gr_basic_sig_source{
public:
    gr_basic_sig_source_impl(void):
        gr_sync_block(
            "signal source",
            gr_make_io_signature (0, 0, 0),
            gr_make_io_signature (1, 1, sizeof(type))
        ),
        _index(0), _step(0),
        _table(wave_table_size),
        _offset(0.0), _scaler(1.0),
        _wave("CONST")
    {
        this->update_table();
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        type *out = reinterpret_cast<type *>(output_items[0]);
        for (size_t i = 0; i < size_t(noutput_items); i++){
            out[i] = _table[_index % wave_table_size];
            _index += _step;
        }
        return noutput_items;
    }

    void set_waveform(const std::string &wave){
        _wave = wave;
        this->update_table();
    }

    void set_offset(const std::complex<double> &offset){
        _offset = offset;
        this->update_table();
    }

    void set_scaler(const std::complex<double> &scaler){
        _scaler = scaler;
        this->update_table();
    }

    void set_frequency(const double freq){
        _step = boost::math::iround(freq*_table.size());
    }

    void update_table(void){
        if (_wave == "CONST"){
            for (size_t i = 0; i < _table.size(); i++){
                this->set_elem(i, 1.0);
            }
        }
        else if (_wave == "COSINE"){
            for (size_t i = 0; i < _table.size(); i++){
                this->set_elem(i, std::pow(M_E, std::complex<double>(0, M_PI*2*i/_table.size())));
            }
        }
        else if (_wave == "RAMP"){
            for (size_t i = 0; i < _table.size(); i++){
                const size_t q = (i+(3*_table.size())/4)%_table.size();
                this->set_elem(i, std::complex<double>(
                    2.0*i/(_table.size()-1) - 1.0,
                    2.0*q/(_table.size()-1) - 1.0
                ));
            }
        }
        else if (_wave == "SQUARE"){
            for (size_t i = 0; i < _table.size(); i++){
                const size_t q = (i+(3*_table.size())/4)%_table.size();
                this->set_elem(i, std::complex<double>(
                    (i < _table.size()/2)? 0.0 : 1.0,
                    (q < _table.size()/2)? 0.0 : 1.0
                ));
            }
        }
        else throw std::invalid_argument("sig source got unknown wave type: " + _wave);
    }

    inline void set_elem(const size_t index, const std::complex<double> &val){
        conv(_scaler * val + _offset, _table[index]);
    }

private:
    size_t _index;
    size_t _step;
    std::vector<type> _table;
    std::complex<double> _offset, _scaler;
    std::string _wave;
};

/***********************************************************************
 * Adder factory function
 **********************************************************************/
gr_basic_sig_source::sptr gr_basic_sig_source::make(op_type type){
    switch(type){
    case OP_FC64: return sptr(new gr_basic_sig_source_impl<std::complex<double> >());
    case OP_F64: return sptr(new gr_basic_sig_source_impl<double>());

    case OP_FC32: return sptr(new gr_basic_sig_source_impl<std::complex<float> >());
    case OP_F32: return sptr(new gr_basic_sig_source_impl<float>());

    case OP_SC64: return sptr(new gr_basic_sig_source_impl<std::complex<int64_t> >());
    case OP_S64: return sptr(new gr_basic_sig_source_impl<int64_t>());

    case OP_SC32: return sptr(new gr_basic_sig_source_impl<std::complex<int32_t> >());
    case OP_S32: return sptr(new gr_basic_sig_source_impl<int32_t>());

    case OP_SC16: return sptr(new gr_basic_sig_source_impl<std::complex<int16_t> >());
    case OP_S16: return sptr(new gr_basic_sig_source_impl<int16_t>());

    case OP_SC8: return sptr(new gr_basic_sig_source_impl<std::complex<int8_t> >());
    case OP_S8: return sptr(new gr_basic_sig_source_impl<int8_t>());

    default: throw std::invalid_argument("make sig source got unknown type");
    }
}
