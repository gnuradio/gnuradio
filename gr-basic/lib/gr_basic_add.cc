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

#include <gr_basic_add.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <complex>
#include <volk/volk.h>

/***********************************************************************
 * Adder implementation with float32 - calls volk
 **********************************************************************/
class gr_basic_add_f32 : public basic_add{
public:
    gr_basic_add_f32(const size_t vlen):
        gr_sync_block(
            "add f32",
            gr_make_io_signature (1, -1, sizeof(float)*vlen),
            gr_make_io_signature (1, 1, sizeof(float)*vlen)
        ),
        _vlen(vlen)
    {
        const int alignment_multiple = volk_get_alignment() / (sizeof(float)*vlen);
        set_output_multiple(std::max(1, alignment_multiple));
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        const size_t n_nums = noutput_items * _vlen;
        float *out = reinterpret_cast<float *>(output_items[0]);
        const float *in0 = reinterpret_cast<const float *>(input_items[0]);

        for (size_t n = 1; n < input_items.size(); n++){
            const float *in = reinterpret_cast<const float *>(input_items[n]);
            volk_32f_x2_add_32f_a(out, in0, in, n_nums);
            in0 = out; //for next input, we do output += input
        }

        return noutput_items;
    }

private:
    const size_t _vlen;
};

/***********************************************************************
 * Generic adder implementation
 **********************************************************************/
template <typename type>
class gr_basic_add_generic : public basic_add{
public:
    gr_basic_add_generic(const size_t vlen):
        gr_sync_block(
            "add generic",
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
                out[i] = in0[i] + in[i];
            }
            in0 = out; //for next input, we do output += input
        }

        return noutput_items;
    }

private:
    const size_t _vlen;
};

/***********************************************************************
 * Adder factory function
 **********************************************************************/
basic_add::sptr basic_make_add(
    add_type type, const size_t vlen
){
    switch(type){
    case ADD_FC64: return boost::shared_ptr<basic_add>(new gr_basic_add_generic<double>(2*vlen));
    case ADD_F64: return boost::shared_ptr<basic_add>(new gr_basic_add_generic<double>(vlen));

    case ADD_FC32: return boost::shared_ptr<basic_add>(new gr_basic_add_f32(2*vlen));
    case ADD_F32: return boost::shared_ptr<basic_add>(new gr_basic_add_f32(vlen));

    case ADD_SC64: return boost::shared_ptr<basic_add>(new gr_basic_add_generic<int64_t>(2*vlen));
    case ADD_S64: return boost::shared_ptr<basic_add>(new gr_basic_add_generic<int64_t>(vlen));

    case ADD_SC32: return boost::shared_ptr<basic_add>(new gr_basic_add_generic<int32_t>(2*vlen));
    case ADD_S32: return boost::shared_ptr<basic_add>(new gr_basic_add_generic<int32_t>(vlen));

    case ADD_SC16: return boost::shared_ptr<basic_add>(new gr_basic_add_generic<int16_t>(2*vlen));
    case ADD_S16: return boost::shared_ptr<basic_add>(new gr_basic_add_generic<int16_t>(vlen));

    case ADD_SC8: return boost::shared_ptr<basic_add>(new gr_basic_add_generic<int8_t>(2*vlen));
    case ADD_S8: return boost::shared_ptr<basic_add>(new gr_basic_add_generic<int8_t>(vlen));

    default: throw std::invalid_argument("basic_make_add got unknown add type");
    }
}
