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

#include <usrp2_eth_packet.h>
#include <usrp2/props.h>
#include "transport.h"
#include "ethernet.h"
#include "pktfilter.h"

namespace usrp2{

    class eth_ctrl_transport: public transport{
    private:
        ethernet      *d_eth_ctrl;  // unbuffered control frames
        pktfilter     *d_pf_ctrl;
        u2_mac_addr   d_mac;
        uint8_t       d_buff[ethernet::MAX_PKTLEN];
        uint8_t       d_padding[ethernet::MIN_PKTLEN];

    public:
        /*!
         * \brief Create a new transport for the raw ethernet control
         * \param ifc the ethernet device name
         * \param mac the destination mac address
         */
        eth_ctrl_transport(const std::string &ifc, const u2_mac_addr &mac);
        ~eth_ctrl_transport();
        bool sendv(const iovec *iov, size_t iovlen);
        void recv(data_handler *handler);
};


} // namespace usrp2

#endif /* INCLUDED_ETH_CTRL_TRANSPORT_H */
