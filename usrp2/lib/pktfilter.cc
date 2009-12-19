/* -*- c++ -*- */
/*
 * Copyright 2005,2007,2008 Free Software Foundation, Inc.
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

#include "pktfilter.h"
#include <iostream>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <features.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <assert.h>
#include <linux/types.h>
#include <linux/filter.h>	// packet filter

namespace usrp2 {
  
  /*
   * This is all based on the Berkeley Packet Filter (BPF) as implemented on Linux.
   *
   * The BPF allows you to run an interpreted program (a filter) in the
   * kernel that sorts through the packets looking for ones you are
   * interested in.  This eliminates the overhead of passing all of the
   * networking packets up into user space for filtering there.
   *
   * For documentation on this see
   * /usr/src/linux/Documentation/networking/filter.txt, The BSD
   * Berkeley Packet Filter manual page, and "The BSD Packet Filter: A
   * New Architecture for User-level Packet Capture", by Steven McCanne
   * and Van Jacobson.
   */
  
  pktfilter::pktfilter ()
    : d_len (0), d_inst (0)
  {
    // NOP
  }
  
  pktfilter::~pktfilter ()
  {
    delete [] d_inst;
  }
  
  inline static sock_filter
  make_stmt (__u16 code, __u32 k)
  {
    sock_filter	f;
    f.code = code;
    f.jt = 0;
    f.jf = 0;
    f.k = k;
    return f;
  }
  
  inline static sock_filter
  make_jump (__u16 code, __u32 k, __u8 jt, __u8 jf)
  {
    sock_filter	f;
    f.code = code;
    f.jt = jt;
    f.jf = jf;
    f.k = k;
    return f;
  }
  
  /*
   * Return a filter that harvests packets with the specified ethertype.
   */
  pktfilter *
  pktfilter::make_ethertype (unsigned short ethertype)
  {
    static const int MAX_LEN = 20;
    sock_filter	*inst = new sock_filter [MAX_LEN];
    pktfilter	*pf = new pktfilter ();
    
    // nothing quite like coding in assembly without the benefit of an assembler ;-)
    
    // ignore packets that don't have the right ethertype
    
    int i = 0;
    inst[i++] = make_stmt (BPF_LD|BPF_H|BPF_ABS, 12);	// load ethertype
    inst[i++] = make_jump (BPF_JMP|BPF_JEQ|BPF_K, ethertype, 1, 0);
    inst[i++] = make_stmt (BPF_RET|BPF_K, 0);		// return 0 (ignore packet)
    inst[i++] = make_stmt (BPF_RET|BPF_K, (unsigned) -1);	// return whole packet
    
    assert (i <= MAX_LEN);
    
    pf->d_inst = inst;
    pf->d_len = i;
    
    return pf;
  }
  
  /*
   * Return a filter that harvests inbound packets with the specified ethertype.
   * \param ethertype	the ethertype we're looking for
   * \param our_mac 	our ethernet MAC address so we can avoid pkts we sent
   */
  pktfilter *
  pktfilter::make_ethertype_inbound (unsigned short ethertype, const unsigned char *our_mac)
  {
    static const int MAX_LEN = 20;
    sock_filter	*inst = new sock_filter [MAX_LEN];
    pktfilter	*pf = new pktfilter ();
    
    __u16 smac_hi = (our_mac[0] << 8) | our_mac[1];
    __u32 smac_lo = (our_mac[2] << 24) | (our_mac[3] << 16) | (our_mac[4] << 8) | our_mac[5];
    
    // nothing quite like coding in assembly without the benefit of an assembler ;-)
    
    // ignore packets that have a different ethertype
    // and packets that have a source mac address == our_mac (packets we sent)
    
    int i = 0;
    inst[i++] = make_stmt (BPF_LD|BPF_H|BPF_ABS, 12);	// load ethertype
    inst[i++] = make_jump (BPF_JMP|BPF_JEQ|BPF_K, ethertype, 0, 5);
    inst[i++] = make_stmt (BPF_LD|BPF_W|BPF_ABS, 8);	// load low 32-bit of src mac
    inst[i++] = make_jump (BPF_JMP|BPF_JEQ|BPF_K, smac_lo, 0, 2);
    inst[i++] = make_stmt (BPF_LD|BPF_H|BPF_ABS, 6);	// load high 16-bits of src mac
    inst[i++] = make_jump (BPF_JMP|BPF_JEQ|BPF_K, smac_hi, 1, 0);
    inst[i++] = make_stmt (BPF_RET|BPF_K, (unsigned) -1);	// return whole packet
    inst[i++] = make_stmt (BPF_RET|BPF_K, 0);		// return 0 (ignore packet)
    
    assert (i <= MAX_LEN);
    
    pf->d_inst = inst;
    pf->d_len = i;
    
    return pf;
  }
  /*
   * Return a filter that harvests inbound packets with the specified ethertype and target USRP2 MAC address.
   * \param ethertype	the ethertype we're looking for
   * \param usrp_mac    our target USRP2 MAC address
   */
  pktfilter *
  pktfilter::make_ethertype_inbound_target (unsigned short ethertype, const unsigned char *usrp_mac)
  {
    static const int MAX_LEN = 20;
    sock_filter	*inst = new sock_filter [MAX_LEN];
    pktfilter	*pf = new pktfilter ();

    __u16 tmac_hi = (usrp_mac[0] << 8) | usrp_mac[1];
    __u32 tmac_lo = (usrp_mac[2] << 24) | (usrp_mac[3] << 16) | (usrp_mac[4] << 8) | usrp_mac[5];
    
    // ignore packets that have a different ethertype
    // and only return packets that have a source mac address == usrp_mac
    
    int i = 0;
    inst[i++] = make_stmt (BPF_LD|BPF_H|BPF_ABS, 12);	// load ethertype
    inst[i++] = make_jump (BPF_JMP|BPF_JEQ|BPF_K, ethertype, 0, 5);
    inst[i++] = make_stmt (BPF_LD|BPF_W|BPF_ABS, 8);	// load low 32-bit of src mac
    inst[i++] = make_jump (BPF_JMP|BPF_JEQ|BPF_K, tmac_lo, 0, 3);
    inst[i++] = make_stmt (BPF_LD|BPF_H|BPF_ABS, 6);	// load high 16-bits of src mac
    inst[i++] = make_jump (BPF_JMP|BPF_JEQ|BPF_K, tmac_hi, 0, 1);
    inst[i++] = make_stmt (BPF_RET|BPF_K, (unsigned) -1);	// return whole packet
    inst[i++] = make_stmt (BPF_RET|BPF_K, 0);		// return 0 (ignore packet)
    
    assert (i <= MAX_LEN);
    
    pf->d_inst = inst;
    pf->d_len = i;
    
    return pf;
  }
  
} // namespace usrp2
