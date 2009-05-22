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
#ifndef INCLUDED_USRP2_METADATA_H
#define INCLUDED_USRP2_METADATA_H

#include <stdint.h>

namespace usrp2 {

  //! type of the timestamp returned from the USRP2 FPGA
  typedef uint32_t	fpga_timestamp;

  /*!
   * \brief metadata associated with received frames
   * \ingroup usrp2
   */
  struct rx_metadata {
    uint32_t		word0;			//< debugging, extensions
    fpga_timestamp	timestamp;		//< time that first sample of frame was received
    unsigned int	start_of_burst : 1;	//< this frame is the start of a burst
    unsigned int	end_of_burst   : 1;	//< this frame is the end of a burst
    unsigned int	rx_overrun     : 1;	//< An Rx overrun occurred in the FPGA
    // rssi
    // agc_mode

    rx_metadata() :
      word0(0), timestamp(0), start_of_burst(0), end_of_burst(0), rx_overrun(0) {}
  };

  /*!
   * \brief metadata associated with transmitted frames
   * \ingroup usrp2
   */
  struct tx_metadata {
    fpga_timestamp	timestamp;		//< time to transmit first sample of frame
    unsigned int	send_now       : 1;	//< ignore timestamp, send now
    unsigned int	start_of_burst : 1;	//< this frame is the start of a burst
    unsigned int	end_of_burst   : 1;	//< this frame is the end of a burst
    // ...

    tx_metadata() :
      timestamp(0), send_now(0), start_of_burst(0), end_of_burst(0) {}
  };

}; // usrp2

#endif /* INCLUDED_USRP2_METADATA_H */
