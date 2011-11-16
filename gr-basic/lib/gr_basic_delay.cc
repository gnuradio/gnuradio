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

#include <gr_basic_delay.h>
#include <gr_io_signature.h>
#include <cstring> //memcpy
#include <gruel/thread.h>

/***********************************************************************
 * Generic delay implementation
 **********************************************************************/
class gr_basic_delay_impl : public gr_basic_delay{
public:
    gr_basic_delay_impl(const size_t itemsize):
        gr_block(
            "basic delay block",
            gr_make_io_signature (1, 1, itemsize),
            gr_make_io_signature (1, 1, itemsize)
        ),
        _itemsize(itemsize)
    {
        this->set_delay(0);
    }

    void set_delay(const int nitems){
        gruel::scoped_lock l(_delay_mutex);
        _delay_items = nitems;
    }

    int general_work(
        int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        gruel::scoped_lock l(_delay_mutex);
        const int delta = int64_t(nitems_read(0)) - int64_t(nitems_written(0)) - _delay_items;

        //consume but not produce (drops samples)
        if (delta < 0){
            consume_each(std::min(ninput_items[0], -delta));
            return 0;
        }

        //produce but not consume (inserts zeros)
        if (delta > 0){
            noutput_items = std::min(noutput_items, delta);
            std::memset(output_items[0], 0, noutput_items*_itemsize);
            return noutput_items;
        }

        //otherwise just memcpy
        noutput_items = std::min(noutput_items, ninput_items[0]);
        std::memcpy(output_items[0], input_items[0], noutput_items*_itemsize);
        consume_each(noutput_items);
        return noutput_items;
    }

private:
    int _delay_items;
    const size_t _itemsize;
    gruel::mutex _delay_mutex;
};

/***********************************************************************
 * Delay factory function
 **********************************************************************/
gr_basic_delay::sptr gr_basic_delay::make(const size_t itemsize){
    return sptr(new gr_basic_delay_impl(itemsize));
}
