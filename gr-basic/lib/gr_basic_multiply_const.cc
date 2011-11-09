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

#include <gr_basic_multiply_const.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <complex>
#include <volk/volk.h>

/***********************************************************************
 * Generic multiply const implementation
 **********************************************************************/
template <typename type>
class gr_basic_multiply_const_generic : public basic_multiply_const{
public:
    gr_basic_multiply_const_generic(const size_t vlen):
        gr_sync_block(
            "multiply const generic",
            gr_make_io_signature (1, 1, sizeof(type)*vlen),
            gr_make_io_signature (1, 1, sizeof(type)*vlen)
        ),
        _vlen(vlen)
    {
        //NOP
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        const size_t n_nums = noutput_items * _vlen;
        type *out = reinterpret_cast<type *>(output_items[0]);
        const type *in = reinterpret_cast<const type *>(input_items[0]);

        //simple vec len 1 for the fast
        if (_vlen == 1){
            const type val = _val[0];
            for (size_t i = 0; i < n_nums; i++){
                out[i] = in[i] * val;
            }
        }

        //general case for any vlen
        else{
            for (size_t i = 0; i < n_nums; i++){
                out[i] = in[i] * _val[i%_val.size()];
            }
        }
        return noutput_items;
    }

    void set_value(const std::vector<double> &val){
        _val.resize(val.size());
        for (size_t i = 0; i < val.size(); i++){
            _val[i] = type(val[i]);
        }
    }

private:
    const size_t _vlen;
    std::vector<type> _val;
};

/***********************************************************************
 * Adder factory function
 **********************************************************************/
basic_multiply_const::sptr basic_make_multiply_const(
    op_type type, const size_t vlen
){
    switch(type){
    case OP_FC64: return basic_multiply_const::sptr(new gr_basic_multiply_const_generic<double>(2*vlen));
    case OP_F64: return basic_multiply_const::sptr(new gr_basic_multiply_const_generic<double>(vlen));

    case OP_FC32: return basic_multiply_const::sptr(new gr_basic_multiply_const_generic<float>(2*vlen));
    case OP_F32: return basic_multiply_const::sptr(new gr_basic_multiply_const_generic<float>(vlen));

    case OP_SC64: return basic_multiply_const::sptr(new gr_basic_multiply_const_generic<int64_t>(2*vlen));
    case OP_S64: return basic_multiply_const::sptr(new gr_basic_multiply_const_generic<int64_t>(vlen));

    case OP_SC32: return basic_multiply_const::sptr(new gr_basic_multiply_const_generic<int32_t>(2*vlen));
    case OP_S32: return basic_multiply_const::sptr(new gr_basic_multiply_const_generic<int32_t>(vlen));

    case OP_SC16: return basic_multiply_const::sptr(new gr_basic_multiply_const_generic<int16_t>(2*vlen));
    case OP_S16: return basic_multiply_const::sptr(new gr_basic_multiply_const_generic<int16_t>(vlen));

    case OP_SC8: return basic_multiply_const::sptr(new gr_basic_multiply_const_generic<int8_t>(2*vlen));
    case OP_S8: return basic_multiply_const::sptr(new gr_basic_multiply_const_generic<int8_t>(vlen));

    default: throw std::invalid_argument("basic_make_multiply got unknown multiply type");
    }
}
