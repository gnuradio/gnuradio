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

#include <gnuradio/sync_block.h>
#include <gnuradio/io_signature.h>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <iostream>
#include <complex>

class tag_source_demo : public gr::sync_block
{
public:

    tag_source_demo(
        const uint64_t start_secs,
        const double start_fracs,
        const double samp_rate,
        const double idle_duration,
        const double burst_duration,
        const std::string &length_tag_name = ""
    ):
        sync_block(
            "uhd tag source demo",
            gr::io_signature::make(0, 0, 0),
            gr::io_signature::make(1, 1, sizeof(std::complex<float>))
        ),
        _time_secs(start_secs),
        _time_fracs(start_fracs),
        _samp_rate(samp_rate),
        _samps_per_burst(samp_rate*burst_duration),
        _cycle_duration(idle_duration + burst_duration),
        _samps_left_in_burst(1), //immediate EOB
        _do_new_burst(false),
        _firstrun(not length_tag_name.empty()),
        _length_tag_key(length_tag_name.empty() ? pmt::PMT_NIL : pmt::string_to_symbol(length_tag_name))
    {
        //NOP
    }

    void make_time_tag(const uint64_t tag_count)
    {
        const pmt::pmt_t key = pmt::string_to_symbol("tx_time");
        const pmt::pmt_t value = pmt::make_tuple(
            pmt::from_uint64(_time_secs),
            pmt::from_double(_time_fracs)
        );
        const pmt::pmt_t srcid = pmt::string_to_symbol(this->name());
        this->add_item_tag(0/*chan0*/, tag_count, key, value, srcid);
    }

    void make_sob_tag(const uint64_t tag_count)
    {
        const pmt::pmt_t key = pmt::string_to_symbol("tx_sob");
        const pmt::pmt_t value = pmt::PMT_T;
        const pmt::pmt_t srcid = pmt::string_to_symbol(this->name());
        this->add_item_tag(0/*chan0*/, tag_count, key, value, srcid);
    }

    void make_eob_tag(const uint64_t tag_count)
    {
        const pmt::pmt_t key = pmt::string_to_symbol("tx_eob");
        const pmt::pmt_t value = pmt::PMT_T;
        const pmt::pmt_t srcid = pmt::string_to_symbol(this->name());
        this->add_item_tag(0/*chan0*/, tag_count, key, value, srcid);
    }

    void make_length_tag(const uint64_t tag_count, const uint64_t burst_len)
    {
        if (pmt::is_null(_length_tag_key)) {
            //no length_tag was specified at initialization; make a tx_sob tag instead
            this->make_sob_tag(tag_count);
            return;
        }
        const pmt::pmt_t key = _length_tag_key;
        const pmt::pmt_t value = pmt::from_long((long)burst_len);
        const pmt::pmt_t srcid = pmt::string_to_symbol(this->name());
        this->add_item_tag(0/*chan0*/, tag_count, key, value, srcid);
    }

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
        //load the output with a constant
        std::complex<float> *output = reinterpret_cast<std::complex<float> *>(output_items[0]);
        for (size_t i = 0; i < size_t(noutput_items); i++) {
            output[i] = std::complex<float>(0.7, 0.7);
        }

        //Handle the start of burst condition.
        //Tag a start of burst and timestamp.
        //Increment the time for the next burst.
        if (_do_new_burst) {
            _do_new_burst = false;
            _samps_left_in_burst = _samps_per_burst;

            if (pmt::is_null(_length_tag_key))
                this->make_sob_tag(this->nitems_written(0));
            else
#if 1
                this->make_length_tag(this->nitems_written(0), _samps_left_in_burst);
#else
                //Test usrp_sink's ability to cancel remainder of burst if new length_tag is found early
                //sets burst time to 10% greater than the cycle duration to guarantee early length_tag
                //In a real implementation the user should guard against this so that the number of
                //samples promised by the length_tag are actually processed by the usrp_sink.
                this->make_length_tag(this->nitems_written(0), uint64_t(1.1 * _samp_rate * _cycle_duration));
#endif
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
            if (pmt::is_null(_length_tag_key))
                this->make_eob_tag(this->nitems_written(0) + _samps_left_in_burst - 1);
            else if (_firstrun){
                _firstrun = false;
                this->make_length_tag(this->nitems_written(0), 1);
            }
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
    bool _firstrun;
    const pmt::pmt_t _length_tag_key;

};
