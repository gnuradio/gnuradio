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

#include <gr_basic_divide.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <complex>
#include <volk/volk.h>

/***********************************************************************
 * Generic divideer implementation
 **********************************************************************/
template <typename type>
class gr_basic_divide_generic : public gr_basic_divide{
public:
    gr_basic_divide_generic(const size_t vlen):
        gr_sync_block(
            "divide generic",
            gr_make_io_signature (1, -1, sizeof(type)*vlen),
            gr_make_io_signature (1, 1, sizeof(type)*vlen)
        ),
        _vlen(vlen)
    {
        //due to implementation, only first 2 ports can be inplaced
        this->set_inplace(true, 0);
        this->set_inplace(true, 1);
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        const size_t n_nums = noutput_items * _vlen;
        type *out = reinterpret_cast<type *>(output_items[0]);
        const type *in0 = reinterpret_cast<const type *>(input_items[0]);

        for (size_t n = 1; n < input_items.size(); n++){
            const type *in = reinterpret_cast<const type *>(input_items[n]);
            for (size_t i = 0; i < n_nums; i++){
                out[i] = in0[i] / in[i];
            }
            in0 = out; //for next input, we do output /= input
        }

        return noutput_items;
    }

private:
    const size_t _vlen;
};

/***********************************************************************
 * factory function
 **********************************************************************/
gr_basic_divide::sptr gr_basic_divide::make(op_type type, const size_t vlen){
    switch(type){
    case OP_FC64: return sptr(new gr_basic_divide_generic<std::complex<double> >(vlen));
    case OP_F64: return sptr(new gr_basic_divide_generic<double>(vlen));

    case OP_FC32: return sptr(new gr_basic_divide_generic<std::complex<float> >(vlen));
    case OP_F32: return sptr(new gr_basic_divide_generic<float>(vlen));

    case OP_SC64: return sptr(new gr_basic_divide_generic<std::complex<int64_t> >(vlen));
    case OP_S64: return sptr(new gr_basic_divide_generic<int64_t>(vlen));

    case OP_SC32: return sptr(new gr_basic_divide_generic<std::complex<int32_t> >(vlen));
    case OP_S32: return sptr(new gr_basic_divide_generic<int32_t>(vlen));

    case OP_SC16: return sptr(new gr_basic_divide_generic<std::complex<int16_t> >(vlen));
    case OP_S16: return sptr(new gr_basic_divide_generic<int16_t>(vlen));

    case OP_SC8: return sptr(new gr_basic_divide_generic<std::complex<int8_t> >(vlen));
    case OP_S8: return sptr(new gr_basic_divide_generic<int8_t>(vlen));

    default: throw std::invalid_argument("make divide got unknown type");
    }
}
