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
 : transport("ethernet control"), d_mac(mac), d_tx_seqno(0), d_rx_seqno(0),
 d_num_rx_frames(0), d_num_rx_missing(0), d_num_rx_overruns(0), d_num_rx_bytes(0){

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
    //create a new iov array with a space for ethernet header
    // and move the current iovs to the center of the new array
    size_t all_iov_len = iovlen + 1;
    iovec all_iov[all_iov_len];
    for (size_t i = 0; i < iovlen; i++){
        all_iov[i+1] = iov[i];
    }
    //setup a new ethernet header
    u2_eth_packet_t hdr;
    hdr.ehdr.ethertype = htons(U2_DATA_ETHERTYPE);
    memcpy(&hdr.ehdr.dst, d_mac.addr, 6);
    memcpy(&hdr.ehdr.src, d_eth_data->mac(), 6);
    hdr.thdr.flags = 0; // FIXME transport header values?
    hdr.thdr.seqno = d_tx_seqno++;
    hdr.thdr.ack = 0;
    //feed the first iov the header
    all_iov[0].iov_base = &hdr;
    all_iov[0].iov_len = sizeof(hdr);
    return d_eth_data->tx_framev(all_iov, all_iov_len);
}

std::vector<usrp2::sbuff::sptr> usrp2::eth_data_transport::recv(){
    std::vector<sbuff::sptr> sbs;
    DEBUG_LOG(":");
    // Receive available frames from ethernet buffer.  Handler will
    // process control frames, enqueue data packets in channel
    // rings, and signal blocked API threads
    std::vector<iovec> iovs = d_eth_data->rx_framev(100); // FIXME magic timeout
    for (size_t i = 0; i < iovs.size(); i++){
        void *base = iovs[i].iov_base;
        size_t len = iovs[i].iov_len;

        u2_eth_packet_t *hdr = (u2_eth_packet_t *)base;
        d_num_rx_frames++;
        d_num_rx_bytes += len;

        /* --- FIXME start of fake transport layer handler --- */

        if (d_rx_seqno != -1) {
          int expected_seqno = (d_rx_seqno + 1) & 0xFF;
          int seqno = hdr->thdr.seqno; 
          
          if (seqno != expected_seqno) {
            DEBUG_LOG("S"); // missing sequence number
            int missing = seqno - expected_seqno;
            if (missing < 0)
                missing += 256;
            d_num_rx_overruns++;
            d_num_rx_missing += missing;
          }
        }

        d_rx_seqno = hdr->thdr.seqno;

        /* --- end of fake transport layer handler --- */

        //drop the ethernet and transport headers
        sbs.push_back(sbuff::make(
            (uint8_t*)base + sizeof(u2_eth_packet_t),
            len - sizeof(u2_eth_packet_t),
            boost::bind(&eth_buffer::release_frame, d_eth_data, base)));
    }
    return sbs;
}
