/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
#ifndef INCLUDED_USRP2_ETH_BUFFER_H
#define INCLUDED_USRP2_ETH_BUFFER_H

#include "pktfilter.h"
#include <eth_common.h>
#include <boost/utility.hpp>
#include <vector>
#include <memory>
#include <stdint.h>

namespace usrp2 {

  class ethernet;
  class data_handler;

  /*!
   * \brief high-performance interface to send and receive raw
   * ethernet frames with out-of-order retirement of received frames.
   *
   * On many systems it should be possible to implement this on top of libpcap
   *
   * \internal
   */
  class eth_buffer : boost::noncopyable 
  {
    
    int		  d_fd;		        // socket file descriptor
    uint8_t	  d_mac[6];	        // our mac address
    bool          d_using_tpring;       // using kernel mapped packet ring
    size_t        d_buflen;             // length of our buffer
    uint8_t      *d_buf;                // packet ring
    unsigned int  d_frame_nr;           // max frames on ring
    size_t        d_frame_size;         // frame storage size
    unsigned int  d_head;               // pointer to next frame

    std::vector<uint8_t *>  d_ring;     // pointers into buffer
    std::auto_ptr<ethernet> d_ethernet; // our underlying interface
  
    bool frame_available();

    void inc_head()
    {
      if (d_head + 1 >= d_frame_nr)
	d_head = 0;
      else
	d_head = d_head + 1;
    }


  public:

    enum result {
      EB_OK,		//< everything's fine
      EB_ERROR,		//< A non-recoverable error occurred
      EB_WOULD_BLOCK,	//< A timeout of 0 was specified and nothing was ready
      EB_TIMED_OUT,	//< The timeout expired before anything was ready
    };

    static const unsigned int MAX_PKTLEN = 1512;
    static const unsigned int MIN_PKTLEN = 64;

    /*!
     * \param rx_bufsize is a hint as to the number of bytes of memory
     * to allocate for received ethernet frames (0 -> reasonable default)
     */
    eth_buffer(size_t rx_bufsize = 0);
    ~eth_buffer();
    
    /*!
     * \brief open the specified interface
     *
     * \param ifname ethernet interface name, e.g., "eth0"
     * \param protocol is the ethertype protocol number in network order.
     *    Use 0 to receive all protocols.
     */
    bool open(const std::string &ifname, int protocol);

    /*!
     * \brief close the interface
     */
    bool close();

    /*!
     * \brief attach packet filter to socket to restrict which packets read sees.
     * \param pf	the packet filter
     */
    bool attach_pktfilter(pktfilter *pf);

    /*!
     * \brief return 6 byte string containing our MAC address
     */
    const uint8_t *mac() const { return d_mac; }

    /*!
     * \brief Call \p f for each frame in the receive buffer.
     * \param f is the frame data handler
     * \param timeout (in ms) controls behavior when there are no frames to read
     *
     * If \p timeout is 0, rx_frames will not wait for frames if none are 
     * available, and f will not be invoked.  If \p timeout is -1 (the 
     * default), rx_frames will block indefinitely until frames are 
     * available.  If \p timeout is positive, it indicates the number of
     * milliseconds to wait for a frame to become available.  Once the
     * timeout has expired, rx_frames will return, f never having been 
     * invoked.
     *
     * \p f will be called on each ethernet frame that is available.
     * \p f returns a bit mask with one of the following set or cleared:
     * 
     * data_handler::KEEP  - hold onto the frame and present it again during 
     *                       the next call to rx_frames, otherwise discard it
     *
     * data_handler::DONE -  return from rx_frames now even though more frames
     *                       might be available; otherwise continue if more 
     *                       frames are ready.
     *
     * The idea of holding onto a frame for the next iteration allows
     * the caller to scan the received packet stream for particular
     * classes of frames (such as command replies) leaving the rest
     * intact.  On the next call all kept frames, followed by any new
     * frames received, will be presented in order to \p f.  
     * See usrp2.cc for an example of the pattern.
     *
     * \returns EB_OK if at least one frame was received
     * \returns EB_WOULD_BLOCK if \p timeout is 0 and the call would have blocked
     * \returns EB_TIMED_OUT if timeout occurred
     * \returns EB_ERROR if there was an unrecoverable error.
     */
    result rx_frames(data_handler *f, int timeout=-1);

    /*
     * \brief Release frame from buffer
     *
     * Call to release a frame previously held by a data_handler::KEEP.
     * The pointer may be offset from the base of the frame up to its length.
     */
    void release_frame(void *p);

    /*
     * \brief Write an ethernet frame to the interface.
     *
     * \param base points to the beginning of the frame (the 14-byte ethernet header).
     * \param len is the length of the frame in bytes.
     * \param flags is 0 or the bitwise-or of values from eth_flags
     *
     * The frame must begin with a 14-byte ethernet header.
     *
     * \returns EB_OK if the frame was successfully enqueued.
     * \returns EB_WOULD_BLOCK if flags contains EF_DONT_WAIT and the call would have blocked.
     * \returns EB_ERROR if there was an unrecoverable error.
     */
    result tx_frame(const void *base, size_t len, int flags=0);

    /*
     * \brief Write an ethernet frame to the interface using scatter/gather.
     *
     * \param iov points to an array of iovec structs
     * \param iovcnt is the number of entries
     * \param flags is 0 or the bitwise-or of values from eth_flags
     *
     * The frame must begin with a 14-byte ethernet header.
     *
     * \returns EB_OK if the frame was successfully enqueued.
     * \returns EB_WOULD_BLOCK if flags contains EF_DONT_WAIT and the call would have blocked.
     * \returns EB_ERROR if there was an unrecoverable error.
     */
    result tx_framev(const eth_iovec *iov, int iovcnt, int flags=0);

    /*
     * \brief Returns maximum possible number of frames in buffer
     */
    unsigned int max_frames() const { return d_frame_nr; }

  };

};  // namespace usrp2

#endif /* INCLUDED_USRP2_ETH_BUFFER_H */
