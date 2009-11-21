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

#ifndef INCLUDED_ETH_CTRL_TRANSPORT_H
#define INCLUDED_ETH_CTRL_TRANSPORT_H

#include "transport.h"
#include "ethernet.h"
#include "pktfilter.h"
#include "usrp2_impl.h"

namespace usrp2{

    class eth_ctrl_transport: public transport{
    private:
        ethernet      *d_eth_ctrl;  // unbuffered control frames
        pktfilter     *d_pf_ctrl;
        u2_mac_addr_t d_mac;
        uint8_t       *d_buff;
        double_t      d_timeout;
        uint8_t       d_padding[ethernet::MIN_PKTLEN];

    public:
        /*!
         * \brief Create a new transport for the raw ethernet control
         * When the target is true, the packet filter is setup for the usrp mac address.
         * When the target is false, the packet filter is setup to ignore our mac address.
         * \param ifc the ethernet device name
         * \param mac the destination mac address
         * \param timeout the timeout in seconds (default 50ms)
         * \param target true for an inbound target
         */
        eth_ctrl_transport(const std::string &ifc, u2_mac_addr_t mac, double timeout=0.05, bool target = true);
        ~eth_ctrl_transport();
        bool sendv(const iovec *iov, size_t iovlen);
        sbuff_vec_t recv();
        size_t max_buffs(){return 3;}
};


} // namespace usrp2

#endif /* INCLUDED_ETH_CTRL_TRANSPORT_H */
