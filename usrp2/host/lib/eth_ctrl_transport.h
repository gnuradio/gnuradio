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
        uint8_t              d_buff[1500]; //FIXME use MTU
        ethernet      *d_eth_ctrl;  // unbuffered control frames
        pktfilter     *d_pf_ctrl;
        u2_mac_addr_t d_usrp_mac;

    public:
        eth_ctrl_transport(const std::string &ifc, props *p);
        ~eth_ctrl_transport();
        int send(const void *buff, int len);
        int recv(void **buff);
};


} // namespace usrp2

#endif /* INCLUDED_ETH_CTRL_TRANSPORT_H */
