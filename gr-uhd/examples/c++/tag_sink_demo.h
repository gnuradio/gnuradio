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

#include <gr_sync_block.h>
#include <gr_io_signature.h>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <complex>

class tag_sink_demo : public gr_sync_block{
public:

    tag_sink_demo(void):
        gr_sync_block(
            "uhd tag sink demo",
            gr_make_io_signature(1, 1, sizeof(std::complex<float>)),
            gr_make_io_signature(0, 0, 0)
        )
    {
        //NOP
    }

    int work(
        int ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        //grab all "rx time" tags in this work call
        const uint64_t samp0_count = this->nitems_read(0);
        std::vector<gr_tag_t> rx_time_tags;
        get_tags_in_range(rx_time_tags, 0, samp0_count, samp0_count + ninput_items, pmt::pmt_string_to_symbol("rx_time"));

        //print all tags
        BOOST_FOREACH(const gr_tag_t &rx_time_tag, rx_time_tags){
            const uint64_t offset = rx_time_tag.offset;
            const pmt::pmt_t &value = rx_time_tag.value;

            std::cout << boost::format("Full seconds %u, Frac seconds %f, abs sample offset %u")
                % pmt::pmt_to_uint64(pmt::pmt_tuple_ref(value, 0))
                % pmt::pmt_to_double(pmt::pmt_tuple_ref(value, 1))
                % offset
            << std::endl;
        }

        return ninput_items;
    }
};
