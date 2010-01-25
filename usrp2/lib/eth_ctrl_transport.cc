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

#include "eth_ctrl_transport.h"
#include <gruel/inet.h>
#include <cstring>
#include <stdexcept>

static const usrp2::u2_mac_addr broadcast_mac_addr("ff:ff:ff:ff:ff:ff");

usrp2::eth_ctrl_transport::eth_ctrl_transport(const std::string &ifc, const u2_mac_addr &mac)
 : transport("ethernet control"), d_mac(mac){

    //create raw ethernet device
    d_eth_ctrl = new ethernet();
    if (!d_eth_ctrl->open(ifc, htons(U2_CTRL_ETHERTYPE)))
        throw std::runtime_error("Unable to open/register USRP2 control protocol");

    //create and attach packet filter
    if (memcmp(&d_mac, &broadcast_mac_addr, sizeof(d_mac)) == 0)
        d_pf_ctrl = pktfilter::make_ethertype_inbound(U2_CTRL_ETHERTYPE, d_eth_ctrl->mac());
    else
        d_pf_ctrl = pktfilter::make_ethertype_inbound_target(U2_CTRL_ETHERTYPE, (const unsigned char*)&d_mac);
    if (!d_pf_ctrl || !d_eth_ctrl->attach_pktfilter(d_pf_ctrl))
        throw std::runtime_error("Unable to attach packet filter for control packets.");

    memset(d_padding, 0, sizeof(d_padding));
}

usrp2::eth_ctrl_transport::~eth_ctrl_transport(){
    delete d_pf_ctrl;
    d_eth_ctrl->close();
    delete d_eth_ctrl;
}

bool usrp2::eth_ctrl_transport::sendv(const iovec *iov, size_t iovlen){
    //create a new iov array with a space for ethernet header and padding
    // and move the current iovs to the center of the new array
    size_t all_iov_len = iovlen + 2;
    iovec all_iov[all_iov_len];
    for (size_t i = 0; i < iovlen; i++){
        all_iov[i+1] = iov[i];
    }
    //setup a new ethernet header
    u2_eth_packet_t hdr;
    hdr.ehdr.ethertype = htons(U2_CTRL_ETHERTYPE);
    memcpy(&hdr.ehdr.dst, &d_mac, 6);
    memcpy(&hdr.ehdr.src, d_eth_ctrl->mac(), 6);
    //feed the first iov the header
    all_iov[0].iov_base = &hdr;
    all_iov[0].iov_len = sizeof(hdr);
    //get number of bytes in current iovs
    int num_bytes = 0;
    for (size_t i = 0; i < all_iov_len-1; i++){
        num_bytes += all_iov[i].iov_len;
    }
    //handle padding, must be at least minimum length
    all_iov[all_iov_len-1].iov_base = d_padding;
    all_iov[all_iov_len-1].iov_len = std::max(int(ethernet::MIN_PKTLEN)-num_bytes, 0);
    return d_eth_ctrl->write_packetv(all_iov, all_iov_len) > 0;
}

void usrp2::eth_ctrl_transport::recv(data_handler *handler){
    int recv_len = d_eth_ctrl->read_packet_timeout(d_buff, ethernet::MAX_PKTLEN, 100); // FIXME magic timeout
    //strip the ethernet headers from the buffer
    if (recv_len > (signed)sizeof(u2_eth_packet_t)){
        data_handler::result result = (*handler)(d_buff + sizeof(u2_eth_packet_t), recv_len - sizeof(u2_eth_packet_t));
        if (result == data_handler::DONE) return; //get out of here
    }
}
