/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <fake_usrp.h>
#include <iostream>
#include <usrp_inband_usb_packet.h>
#include <mb_class_registry.h>
#include <vector>

typedef usrp_inband_usb_packet transport_pkt;   // makes conversion to gigabit easy

fake_usrp::fake_usrp()
{
  std::cout << "[fake_usrp] Initializing...\n";
}

fake_usrp::~fake_usrp() {}

long
fake_usrp::write_bus(transport_pkt *pkts, long n_bytes)
{
  std::cout << "[fake_usrp] Bytes over bus: " << n_bytes << "\n";

  // I'm assuming that a control packet cannot exist in a burst of data packets,
  // therefore i read only the first packet's channel in the current burst
  if(pkts[0].chan() == 0x1f) {
    return control_block(pkts, n_bytes);
  } else {
    return data_block(pkts, n_bytes);
  }

}

long
fake_usrp::data_block(transport_pkt *pkts, long n_bytes)
{
  std::cout << "[fake_usrp] Entering data block\n";

  // Infer the number of packets from the byte count to do logical tests
  long n_pkts = static_cast<long>(std::ceil(n_bytes / (double)transport_pkt::max_pkt_size()));
  
  std::cout << "[fake_usrp] Number of packets: " << n_pkts << "\n";

  // The first packet should have the start of burst, and the last packet should have end of burst
  if(pkts[0].start_of_burst() && pkts[n_pkts-1].end_of_burst()) {
    std::cout << "[fake_usrp] Correct burst flags set\n";
  } else {
    std::cout << "[fake_usrp] Incorrect burst flags set!\n";
    return 0;
  }

  // All other flags should be set to 0 (e.g., overrun should not be set yet) on ALL packets
  for(int i=0; i < n_pkts; i++) {
    if(pkts[i].overrun()) {
      std::cout << "[fake_usrp] Incorrect set of overrun flag on transmit\n";
      return 0;
    } else if(pkts[i].underrun()) {
      std::cout << "[fake_usrp] Incorrect set of underrun flag on transmit\n";
      return 0;
    } else if(pkts[i].dropped()) {
      std::cout << "[fake_usrp] Incorrect set of drop flag on transmit\n";
      return 0;
    } 
  }
  std::cout << "[fake_usrp] Correct overrun, underrun, and drop flags on transmit (initialized to 0)\n";
  
  // The first packet should have a timestamp, other packets should have "NOW"
  if(pkts[0].timestamp() != 0xffffffff) {
    std::cout << "[fake_usrp] Correct timestamp on first packet\n";
  } else {
    std::cout << "[fake_usrp] Initial packet should not have the 0xffffffff timestamp\n";
    return 0;
  }

  // Check that all of the other packets include the NOW timestamp
  int check_stamps=1;
  for(int i=1; i < n_pkts; i++)           // start at 1 to skip the first packet
    if(pkts[i].timestamp() != 0xffffffff) 
      check_stamps=0;

  if(check_stamps) {
    std::cout << "[fake_usrp] Correct NOW timestamps (0xffffffff) on intermediate burst packets\n";
  } else {
    std::cout << "[fake_usrp] Incorrect timestamps on intermediate burst packets\n";
    return 0;
  }

  // Since we are being transparent about samples, we do not ensure the payload is correct, however
  // it should be the case that if there are >1 packets, all packets except the last packet should
  // have a full payload size
  if(n_pkts > 1) {
    int check_size=1;
    for(int i=0; i < n_pkts-1; i++)
      if(pkts[i].payload_len() != transport_pkt::max_payload())
        check_size=0;

    if(check_size) {
      std::cout << "[fake_usrp] Correct payload size sanity check on packets\n";
    } else {
      std::cout << "[fake_usrp] Failed payload size sanity check\n";
      return 0;
    }
  }

  return 1;
}

long
fake_usrp::control_block(transport_pkt *pkts, long n_bytes)
{
  std::cout << "[fake_usrp] Entering control block\n";

  return 1;
}
