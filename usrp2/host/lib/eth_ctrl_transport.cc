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

//FIXME this is the third instance of this function, find it a home
static bool
parse_mac_addr(const std::string &s, u2_mac_addr_t *p)
{
    p->addr[0] = 0x00;		// Matt's IAB
    p->addr[1] = 0x50;
    p->addr[2] = 0xC2;
    p->addr[3] = 0x85;
    p->addr[4] = 0x30;
    p->addr[5] = 0x00;

    int len = s.size();

    switch (len){
      
    case 5:
      return sscanf(s.c_str(), "%hhx:%hhx", &p->addr[4], &p->addr[5]) == 2;
      
    case 17:
      return sscanf(s.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
            &p->addr[0], &p->addr[1], &p->addr[2],
            &p->addr[3], &p->addr[4], &p->addr[5]) == 6;
    default:
      return false;
    }
}


usrp2::eth_ctrl_transport::eth_ctrl_transport(const std::string &ifc, props *p) : transport("ethernet control"){

    //create raw ethernet device
    d_eth_ctrl = new ethernet();
    if (!d_eth_ctrl->open(ifc, htons(U2_CTRL_ETHERTYPE)))
        throw std::runtime_error("Unable to open/register USRP2 control protocol");

    //extract mac addr
    parse_mac_addr(p->addr, &d_usrp_mac);

    //create and attach packet filter
    d_pf_ctrl = pktfilter::make_ethertype_inbound_target(U2_CTRL_ETHERTYPE, (const unsigned char*)&(d_usrp_mac.addr));
    if (!d_pf_ctrl || !d_eth_ctrl->attach_pktfilter(d_pf_ctrl))
        throw std::runtime_error("Unable to attach packet filter for control packets.");
}

usrp2::eth_ctrl_transport::~eth_ctrl_transport(){
    delete d_pf_ctrl;
    d_eth_ctrl->close();
    delete d_eth_ctrl;
}

//FIXME clean this up, probably when we get vrt headers
//eth transport is only responsible for eth headers and transport headers
//that leaves the u2 fixed headers to be handled by the usrp2 impl
typedef struct {
  u2_eth_hdr_t		ehdr;
  u2_transport_hdr_t	thdr;
} u2_eth_packet_only_t;

int usrp2::eth_ctrl_transport::send(const void *buff, int len){
    //return d_eth_ctrl->write_packet(buff, len);
    //setup a new ethernet header
    u2_eth_packet_only_t hdr;
    hdr.ehdr.ethertype = htons(U2_CTRL_ETHERTYPE);
    memcpy(&hdr.ehdr.dst, d_usrp_mac.addr, 6);
    memcpy(&hdr.ehdr.src, d_eth_ctrl->mac(), 6);
    hdr.thdr.flags = 0; // FIXME transport header values?
    hdr.thdr.seqno = 0;
    hdr.thdr.ack = 0;
    //load the buffer with header and control data
    uint8_t packet[len+sizeof(hdr)];
    memcpy(packet, &hdr, sizeof(hdr));
    memcpy(packet+sizeof(hdr), buff, len);
    return d_eth_ctrl->write_packet(packet, len+sizeof(hdr));
}

int usrp2::eth_ctrl_transport::recv(void **buff){
    int recv_len = d_eth_ctrl->read_packet_dont_block(d_buff, sizeof(d_buff));
    if (recv_len > sizeof(u2_eth_packet_only_t)){
        *buff = d_buff + sizeof(u2_eth_packet_only_t);
        return recv_len - sizeof(u2_eth_packet_only_t);
    }
    boost::this_thread::sleep(gruel::get_new_timeout(0.05)); //50ms timeout
    return 0; //nothing yet
}
