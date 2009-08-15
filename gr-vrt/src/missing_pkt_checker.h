/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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
#ifndef INCLUDED_MISSING_PKT_CHECKER_H
#define INCLUDED_MISSING_PKT_CHECKER_H

#include <vrt/expanded_header.h>
#include <stdint.h>

/*!
 * \brief Check for missing packets
 */
class missing_pkt_checker
{
  // FIXME assumes we're inspecting only a single stream

  bool		d_resync;
  uint64_t	d_npackets;		//< total number of packets
  int		d_last_pkt_cnt;		//< the last pkt_cnt we saw
  uint64_t	d_nwrong_pkt_cnt; 	//< number of incorrect pkt_cnt
  uint64_t	d_nmissing_pkt_est;    	//< estimate of total number of missing pkts
  
public:
  missing_pkt_checker()
    : d_resync(true), d_npackets(0), d_last_pkt_cnt(0xf), d_nwrong_pkt_cnt(0),
      d_nmissing_pkt_est(0) {}
    
  /*!
   * \brief check header pkt_cnt and return 0 if OK, else estimate of number of missing packets.
   */
  int check(const vrt::expanded_header *hdr);

  void resync() { d_resync = true; }
  uint64_t npackets() const { return d_npackets; }
  uint64_t nwrong_pkt_cnt() const { return d_nwrong_pkt_cnt; }
  uint64_t nmissing_pkt_est() const { return d_nmissing_pkt_est; }
};


#endif /* INCLUDED_MISSING_PKT_CHECKER_H */
