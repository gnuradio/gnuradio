/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <usrp2_eth_packet.h>
#include <usrp2/usrp2.h>
#include <boost/scoped_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include "ethernet.h"
#include "pktfilter.h"
#include <string.h>
#include <iostream>
#include <stdexcept>
#include <cstdio>

#define FIND_DEBUG 0


// FIXME move to gruel

static struct timeval
time_duration_to_timeval(boost::posix_time::time_duration delta)
{
  long total_us = delta.total_microseconds();
  if (total_us < 0)
    throw std::invalid_argument("duration_to_time: delta is negative");

  struct timeval tv;
  tv.tv_sec =  total_us / 1000000;
  tv.tv_usec = total_us % 1000000;
  return tv;
}


namespace usrp2 {

  static props
  reply_to_props(const op_id_reply_t *r)
  {
    const uint8_t *mac = (const uint8_t *)&r->addr;
    char addr_buf[128];
    snprintf(addr_buf, sizeof(addr_buf), "%02x:%02x:%02x:%02x:%02x:%02x",
	     mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
      
    props p;
    p.addr = std::string(addr_buf);  
    p.hw_rev = ntohs(r->hw_rev);
    memcpy(p.fpga_md5sum, r->fpga_md5sum, sizeof(p.fpga_md5sum));
    memcpy(p.sw_md5sum, r->sw_md5sum, sizeof(p.sw_md5sum));
    return p;
  }

  static void
  read_replies(ethernet *enet, struct timeval timeout,
	       const std::string &target_addr, props_vector_t &result)
  {
    struct reply {
      u2_eth_packet_t	h;
      op_id_reply_t	op_id_reply;
    };
    
    uint8_t pktbuf[ethernet::MAX_PKTLEN];
    memset(pktbuf, 0, sizeof(pktbuf));

    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(enet->fd(), &read_fds);
    
    select(enet->fd()+1, &read_fds, 0, 0, &timeout);
    while(1) {
      memset(pktbuf, 0, sizeof(pktbuf));
      int len = enet->read_packet_dont_block(pktbuf, sizeof(pktbuf));
      if (len < 0){
	perror("usrp2_basic: read_packet_dont_block");
        return;
      }
      if (len == 0)
	break;
      
      reply *rp = (reply *)pktbuf;
      if (u2p_chan(&rp->h.fixed) != CONTROL_CHAN)  	// ignore
	continue;
      if (rp->op_id_reply.opcode != OP_ID_REPLY)	// ignore
	continue;
      
      props p = reply_to_props(&rp->op_id_reply);
      if (FIND_DEBUG)
	std::cerr << "usrp2::find: response from " << p.addr << std::endl;
      
      if ((target_addr == "") || (target_addr == p.addr))
	result.push_back(p);
    }
  }

  props_vector_t
  find(const std::string &ifc, const std::string &addr)
  {
    if (FIND_DEBUG) {
      std::cerr << "usrp2::find: Searching interface " << ifc << " for "
		<< (addr == "" ? "all USRP2s" : addr)
		<< std::endl;
    }
    
    props_vector_t result;
    struct command {
      u2_eth_packet_t	h;
      op_generic_t	op_id;
    };
    
    std::auto_ptr<ethernet> enet(new ethernet()); 
    
    if (!enet->open(ifc, htons(U2_ETHERTYPE)))
      return result;
    
    std::auto_ptr<pktfilter> pf(pktfilter::make_ethertype_inbound(U2_ETHERTYPE, enet->mac()));
    if (!enet->attach_pktfilter(pf.get()))
      return result;
    
    static u2_mac_addr_t broadcast_mac_addr =
      {{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }};
    
    uint8_t pktbuf[ethernet::MAX_PKTLEN];
    memset(pktbuf, 0, sizeof(pktbuf));
    
    command *c = (command *)pktbuf;
    c->h.ehdr.ethertype = htons(U2_ETHERTYPE);
    c->h.ehdr.dst = broadcast_mac_addr;
    memcpy(&c->h.ehdr.src, enet->mac(), 6);
    c->h.thdr.flags = 0;
    c->h.thdr.seqno = 0;
    c->h.thdr.ack = 0;
    u2p_set_word0(&c->h.fixed, 0, CONTROL_CHAN);
    u2p_set_timestamp(&c->h.fixed, -1);
    c->op_id.opcode = OP_ID;
    c->op_id.len = sizeof(c->op_id);
    int len = std::max((size_t) ethernet::MIN_PKTLEN, sizeof(command));
    if (enet->write_packet(c, len) != len)
      return result;
    
    if (FIND_DEBUG)
      std::cerr << "usrp2::find: broadcast ID command" << std::endl;
    
    /*
     * Gather all responses that occur within 50ms
     */
    boost::posix_time::ptime start(boost::posix_time::microsec_clock::universal_time());
    boost::posix_time::ptime limit(start + boost::posix_time::milliseconds(50));
    boost::posix_time::ptime now;

    while (1){
      now = boost::posix_time::microsec_clock::universal_time();
      if (now >= limit)
	break;

      boost::posix_time::time_duration delta(limit - now);
      struct timeval timeout = time_duration_to_timeval(delta);

      read_replies(enet.get(), timeout, addr, result);
    }
    return result;
  }
  
} // namespace usrp2

