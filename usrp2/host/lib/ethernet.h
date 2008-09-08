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

#ifndef INCLUDED_USRP2_ETHERNET_H
#define INCLUDED_USRP2_ETHERNET_H

#include <string>
#include <vector>
#include <eth_common.h>

namespace usrp2 {

  class pktfilter;
  
  /*!
   * \brief Read and write ethernet frames.
   *
   * This provides a low level interface to hardware that communicates
   * via raw (non-IP) ethernet frames.
   */
  class ethernet {
    int	  d_fd;
    uint8_t d_mac[6];
    
  public:
    ethernet ();
    ~ethernet ();
    
    static const int MAX_PKTLEN = 1512;
    static const int MIN_PKTLEN = 64;
    
    /*!
     * \param ifname ethernet interface name, e.g., "eth0"
     * \param protocol is the ethertype protocol number in network order.
     *    Use 0 to receive all protocols.
     */
    bool open (std::string ifname, int protocol);
    
    bool close ();
    
    /*!
     * \brief attach packet filter to socket to restrict which packets read sees.
     * \param pf	the packet filter
     */
    bool attach_pktfilter (pktfilter *pf);
    
    /*!
     * \brief return 6 byte string containing our MAC address
     */
    const uint8_t *mac () const { return d_mac; }
    
    /*!
     * \brief Return file descriptor associated with socket.
     */
    int fd () const { return d_fd; }
    
    /*!
     * \brief Read packet from interface.
     *
     * \param buf		where to put the packet
     * \param buflen	maximum length of packet in bytes (should be >= 1528)
     *
     * \returns number of bytes read or -1 if trouble.
     *
     * Returned packet includes 14-byte ethhdr
     */
    int read_packet (void *buf, int buflen);
    
    /*!
     * \brief Read packet from interface, but don't block waiting
     *
     * \param buf		where to put the packet
     * \param buflen	maximum length of packet in bytes (should be >= 1528)
     *
     * \returns number of bytes read, -1 if trouble or 0 if nothing available.
     *
     * Returned packet includes 14-byte ethhdr
     */
    int read_packet_dont_block (void *buf, int buflen);
    
    /*
     * \brief Write ethernet packet to interface.
     *
     * \param buf		the packet to write
     * \param buflen	length of packet in bytes
     *
     * \returns number of bytes written or -1 if trouble.
     *
     * Packet must begin with 14-byte ethhdr, but does not include the FCS.
     */
    int write_packet (const void *buf, int buflen);

    /*
     * \brief Write ethernet packet to interface.
     *
     * \param iov	scatter/gather array
     * \param iovlen	number of elements in iov
     *
     * \returns number of bytes written or -1 if trouble.
     *
     * Packet must begin with 14-byte ethhdr, but does not include the FCS.
     */
    int write_packetv (const eth_iovec *iov, size_t iovlen);

  };
  
} // namespace usrp2

#endif /* INCLUDED_USRP2_ETHERNET_H */
