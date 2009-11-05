/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDED_ETH_DATA_TRANSPORT_H
#define INCLUDED_ETH_DATA_TRANSPORT_H

#include <usrp2/data_handler.h>
#include "transport.h"
#include "eth_buffer.h"
#include "pktfilter.h"
#include "usrp2_impl.h"

namespace usrp2{

    class eth_data_transport: public transport{
    private:
        eth_buffer    *d_eth_data;	// packet ring buffered data frames
        pktfilter     *d_pf_data;
        u2_mac_addr_t d_mac;
        int           d_tx_seqno;
        int           d_rx_seqno;
        data_handler::result operator()(const void *base, size_t len);
        unsigned int   d_num_rx_frames;
        unsigned int   d_num_rx_missing;
        unsigned int   d_num_rx_overruns;
        unsigned int   d_num_rx_bytes;

    public:
        eth_data_transport(const std::string &ifc, u2_mac_addr_t mac, size_t rx_bufsize);
        ~eth_data_transport();
        int sendv(const iovec *iov, size_t iovlen);
        sbuff::sptr recv();
        void init();
};


} // namespace usrp2

#endif /* INCLUDED_ETH_DATA_TRANSPORT_H */
