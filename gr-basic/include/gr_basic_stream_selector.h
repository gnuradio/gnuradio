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
#ifndef INCLUDED_GR_BASIC_STREAM_SELECTOR_H
#define INCLUDED_GR_BASIC_STREAM_SELECTOR_H

#include <gr_basic_api.h>
#include <gr_hier_block2.h>

/*!
 * The sector blocks allows streams to be dynamically routed at runtime.
 */
class GR_BASIC_API gr_basic_stream_selector : virtual public gr_hier_block2{
public:
    typedef boost::shared_ptr<gr_basic_stream_selector> sptr;

    /*!
     * Make a new stream selector block
     * \param in_sig signature to describe inputs
     * \param out_sig signature to describe outputs
     */
    static sptr make(gr_io_signature_sptr in_sig, gr_io_signature_sptr out_sig);

    /*!
     * Set the path for samples for each input ports.
     * Element i of paths specifies the output port index for input port i.
     * The value of the element may also be: -1 to block or -2 to consume.
     * \param paths a list of stream destinations for each port
     */
    virtual void set_paths(const std::vector<int> &paths) = 0;

};

#endif /* INCLUDED_GR_BASIC_STREAM_SELECTOR_H */
