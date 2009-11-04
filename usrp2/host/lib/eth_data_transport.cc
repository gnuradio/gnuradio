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

#include "eth_data_transport.h"
#include <gruel/inet.h>
#include <gruel/realtime.h>
#include <gruel/sys_pri.h>
#include <iostream>

usrp2::eth_data_transport::eth_data_transport(const std::string &ifc, u2_mac_addr_t mac, size_t rx_bufsize)
 : transport("ethernet control"), d_mac(mac), d_tx_seqno(0){

    //create raw ethernet device
    d_eth_data = new eth_buffer(rx_bufsize);
    if (!d_eth_data->open(ifc, htons(U2_DATA_ETHERTYPE)))
        throw std::runtime_error("Unable to open/register USRP2 data protocol");

    //create and attach packet filter
    d_pf_data = pktfilter::make_ethertype_inbound_target(U2_DATA_ETHERTYPE, (const unsigned char*)&(d_mac.addr));
    if (!d_pf_data || !d_eth_data->attach_pktfilter(d_pf_data))
        throw std::runtime_error("Unable to attach packet filter for data packets.");
}

usrp2::eth_data_transport::~eth_data_transport(){
    delete d_pf_data;
    d_eth_data->close();
    delete d_eth_data;
}

void usrp2::eth_data_transport::init(){
    if (gruel::enable_realtime_scheduling(gruel::sys_pri::usrp2_backend()) != gruel::RT_OK)
        std::cerr << "usrp2: failed to enable realtime scheduling" << std::endl;
}

int usrp2::eth_data_transport::sendv(const iovec *iov, size_t iovlen){
   return 0;
}

int usrp2::eth_data_transport::recv(void **buff){
    return 0;
}
