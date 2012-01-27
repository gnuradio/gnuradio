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

#ifndef INCLUDED_GR_UHD_AMSG_SOURCE_H
#define INCLUDED_GR_UHD_AMSG_SOURCE_H

#include <gr_uhd_api.h>
#include <uhd/usrp/multi_usrp.hpp>
#include <gr_msg_queue.h>

class uhd_amsg_source;

/*!
 * \brief Make a new USRP asynchronous message-based source block.
 * \ingroup uhd_blk
 */
GR_UHD_API boost::shared_ptr<uhd_amsg_source> uhd_make_amsg_source(
    const uhd::device_addr_t &device_addr,
    gr_msg_queue_sptr msgq
);

class GR_UHD_API uhd_amsg_source{
public:
    /*!
     * Convert a raw asynchronous message to an asynchronous metatdata object.
     * \return The asynchronous metadata object.
     */
    static uhd::async_metadata_t msg_to_async_metadata_t(const gr_message_sptr msg);

};

#endif /* INCLUDED_GR_UHD_AMSG_SOURCE_H */
