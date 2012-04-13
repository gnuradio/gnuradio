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

class tag_source_demo : public gr_sync_block{
public:

    tag_source_demo(
        const uint64_t start_secs,
        const double start_fracs,
        const double samp_rate,
        const double idle_duration,
        const double burst_duration
    ):
        gr_sync_block(
            "uhd tag source demo",
            gr_make_io_signature(0, 0, 0),
            gr_make_io_signature(1, 1, sizeof(std::complex<float>))
        ),
        _time_secs(start_secs),
        _time_fracs(start_fracs),
        _samp_rate(samp_rate),
        _samps_per_burst(samp_rate*burst_duration),
        _cycle_duration(idle_duration + burst_duration),
        _samps_left_in_burst(1), //immediate EOB
        _do_new_burst(false)
    {
        //NOP
    }

    void make_time_tag(const uint64_t tag_count){;
        const pmt::pmt_t key = pmt::pmt_string_to_symbol("tx_time");
        const pmt::pmt_t value = pmt::pmt_make_tuple(
            pmt::pmt_from_uint64(_time_secs),
            pmt::pmt_from_double(_time_fracs)
        );
        const pmt::pmt_t srcid = pmt::pmt_string_to_symbol(this->name());
        this->add_item_tag(0/*chan0*/, tag_count, key, value, srcid);
    }

    void make_sob_tag(const uint64_t tag_count){
        const pmt::pmt_t key = pmt::pmt_string_to_symbol("tx_sob");
        const pmt::pmt_t value = pmt::PMT_T;
        const pmt::pmt_t srcid = pmt::pmt_string_to_symbol(this->name());
        this->add_item_tag(0/*chan0*/, tag_count, key, value, srcid);
    }

    void make_eob_tag(const uint64_t tag_count){;
        const pmt::pmt_t key = pmt::pmt_string_to_symbol("tx_eob");
        const pmt::pmt_t value = pmt::PMT_T;
        const pmt::pmt_t srcid = pmt::pmt_string_to_symbol(this->name());
        this->add_item_tag(0/*chan0*/, tag_count, key, value, srcid);
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        //load the output with a constant
        std::complex<float> *output = reinterpret_cast<std::complex<float> *>(output_items[0]);
        for (size_t i = 0; i < size_t(noutput_items); i++){
            output[i] = std::complex<float>(0.7, 0.7);
        }

        //Handle the start of burst condition.
        //Tag a start of burst and timestamp.
        //Increment the time for the next burst.
        if (_do_new_burst){
            _do_new_burst = false;
            _samps_left_in_burst = _samps_per_burst;

            this->make_sob_tag(this->nitems_written(0));
            this->make_time_tag(this->nitems_written(0));

            _time_fracs += _cycle_duration;
            double intpart; //normalize
            _time_fracs = std::modf(_time_fracs, &intpart);
            _time_secs += uint64_t(intpart);
        }

        //Handle the end of burst condition.
        //Tag an end of burst and return early.
        //the next work call will be a start of burst.
        if (_samps_left_in_burst < size_t(noutput_items)){
            this->make_eob_tag(this->nitems_written(0) + _samps_left_in_burst - 1);
            _do_new_burst = true;
            noutput_items = _samps_left_in_burst;
        }

        _samps_left_in_burst -= noutput_items;
        return noutput_items;
    }

private:
    uint64_t _time_secs;
    double _time_fracs;
    const double _samp_rate;
    const uint64_t _samps_per_burst;
    const double _cycle_duration;
    uint64_t _samps_left_in_burst;
    bool _do_new_burst;

};
