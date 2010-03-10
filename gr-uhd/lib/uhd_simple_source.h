/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#ifndef INCLUDED_UHD_SIMPLE_SOURCE_H
#define INCLUDED_UHD_SIMPLE_SOURCE_H

#include <gr_sync_block.h>
#include <uhd/simple_device.hpp>

class uhd_simple_source;

boost::shared_ptr<uhd_simple_source>
uhd_make_simple_source(const std::string &args, const std::string &type);

class uhd_simple_source : public gr_sync_block{
public:
    uhd_simple_source(const std::string &args, const std::string &type);
    ~uhd_simple_source(void);

    void set_samp_rate(double rate);
    double get_samp_rate(void);

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    );

protected:
    uhd::simple_device::sptr _dev;
    std::string _type;
    size_t _sizeof_samp;
    bool _is_streaming;
};

#endif /* INCLUDED_UHD_SIMPLE_SOURCE_H */
