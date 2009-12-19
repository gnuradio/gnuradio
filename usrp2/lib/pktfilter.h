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

#ifndef INCLUDED_USRP2_PKTFILTER_H
#define INCLUDED_USRP2_PKTFILTER_H

struct sock_filter;

namespace usrp2 {

  /*
   * \brief Compile programs for the Berkeley Packet Filter
   */
  class pktfilter {
  public:
    
    unsigned		 d_len;		// # of instructions
    struct sock_filter	*d_inst;	// the instructions
    
    pktfilter ();
    ~pktfilter ();
    
    /*!
     * \brief Return a filter that harvests packets with the specified ethertype.
     * \param ethertype	the ethertype we're looking for.
     */
    static pktfilter *make_ethertype (unsigned short ethertype);
    
    /*!
     * \brief Return a filter that harvests inbound packets with the specified ethertype.
     * \param ethertype	the ethertype we're looking for
     * \param our_mac 	our MAC address so we can avoid pkts we sent
     */
    static pktfilter *make_ethertype_inbound (unsigned short ethertype,
					      const unsigned char *our_mac);

    /*!
     * \brief Return a filter that harvests inbound packets with the specified ethertype
     *        and source MAC address
     * \param ethertype	the ethertype we're looking for
     * \param usrp_mac 	the source MAC address
     */
    static pktfilter *make_ethertype_inbound_target (unsigned short ethertype,
						     const unsigned char *usrp_mac); 
  };
  
} // namespace usrp2

#endif /* INCLUDED_USRP2_PKTFILTER_H */
