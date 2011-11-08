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

/***********************************************************************
 * Adder implementation with complex float 32
 **********************************************************************/
class gr_basic_add_fc32 : public basic_add{
public:
    typedef std::complex<float> my_type;

    gr_basic_add_fc32(const size_t vlen):
        gr_sync_block(
            "add fc32",
            gr_make_io_signature (1, -1, sizeof(my_type)*vlen),
            gr_make_io_signature (1, 1, sizeof(my_type)*vlen)
        ),
        _vlen(vlen)
    {
        //TODO this is where you set output multiple for volk
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        const size_t n_nums = noutput_items * _vlen;
        my_type *out = reinterpret_cast<my_type *>(output_items[0]);
        const my_type *in0 = reinterpret_cast<const my_type *>(input_items[0]);

        for (size_t n = 1; n < input_items.size(); n++){
            const my_type *in = reinterpret_cast<const my_type *>(input_items[n]);
            //TODO - this is where you call into volk
            for (size_t i = 0; i < n_nums; i++){
                out[i] = in0[i] + in[i];
            }
            in0 = out; //for next input, we do output += input
        }
    }

private:
    const size_t _vlen;
};

/***********************************************************************
 * Adder factory function
 **********************************************************************/
boost::shared_ptr<basic_add> basic_make_add(
    add_type type, const size_t vlen
){
    switch(type){
    case ADD_FC32: return boost::shared_ptr<basic_add>(new gr_basic_add_fc32(vlen));
    default: throw std::invalid_argument("basic_make_add got unknown add type");
    }
}
