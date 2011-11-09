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

#include <gr_basic_multiply.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <complex>
#include <volk/volk.h>

/***********************************************************************
 * Generic multiplyer implementation
 **********************************************************************/
template <typename type>
class gr_basic_multiply_generic : public basic_multiply{
public:
    gr_basic_multiply_generic(const size_t vlen):
        gr_sync_block(
            "multiply generic",
            gr_make_io_signature (1, -1, sizeof(type)*vlen),
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
        const type *in0 = reinterpret_cast<const type *>(input_items[0]);

        for (size_t n = 1; n < input_items.size(); n++){
            const type *in = reinterpret_cast<const type *>(input_items[n]);
            for (size_t i = 0; i < n_nums; i++){
                out[i] = in0[i] * in[i];
            }
            in0 = out; //for next input, we do output *= input
        }

        return noutput_items;
    }

private:
    const size_t _vlen;
};

/***********************************************************************
 * Adder factory function
 **********************************************************************/
basic_multiply::sptr basic_make_multiply(
    multiply_type type, const size_t vlen
){
    switch(type){
    case MULTIPLY_FC64: return boost::shared_ptr<basic_multiply>(new gr_basic_multiply_generic<std::complex<double> >(vlen));
    case MULTIPLY_F64: return boost::shared_ptr<basic_multiply>(new gr_basic_multiply_generic<double>(vlen));

    case MULTIPLY_FC32: return boost::shared_ptr<basic_multiply>(new gr_basic_multiply_generic<std::complex<float> >(vlen));
    case MULTIPLY_F32: return boost::shared_ptr<basic_multiply>(new gr_basic_multiply_generic<float>(vlen));

    case MULTIPLY_SC64: return boost::shared_ptr<basic_multiply>(new gr_basic_multiply_generic<std::complex<int64_t> >(vlen));
    case MULTIPLY_S64: return boost::shared_ptr<basic_multiply>(new gr_basic_multiply_generic<int64_t>(vlen));

    case MULTIPLY_SC32: return boost::shared_ptr<basic_multiply>(new gr_basic_multiply_generic<std::complex<int32_t> >(vlen));
    case MULTIPLY_S32: return boost::shared_ptr<basic_multiply>(new gr_basic_multiply_generic<int32_t>(vlen));

    case MULTIPLY_SC16: return boost::shared_ptr<basic_multiply>(new gr_basic_multiply_generic<std::complex<int16_t> >(vlen));
    case MULTIPLY_S16: return boost::shared_ptr<basic_multiply>(new gr_basic_multiply_generic<int16_t>(vlen));

    case MULTIPLY_SC8: return boost::shared_ptr<basic_multiply>(new gr_basic_multiply_generic<std::complex<int8_t> >(vlen));
    case MULTIPLY_S8: return boost::shared_ptr<basic_multiply>(new gr_basic_multiply_generic<int8_t>(vlen));

    default: throw std::invalid_argument("basic_make_multiply got unknown multiply type");
    }
}
