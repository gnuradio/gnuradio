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

  /*!
   * \brief metadata associated with transmitted frames
   * \ingroup usrp2
   */
  struct tx_metadata {
    unsigned int	send_now       : 1;	//< ignore timestamp, send now
    unsigned int	start_of_burst : 1;	//< this frame is the start of a burst
    unsigned int	end_of_burst   : 1;	//< this frame is the end of a burst
    // ...

    tx_metadata() :
      send_now(0), start_of_burst(0), end_of_burst(0) {}
  };

}; // usrp2

#endif /* INCLUDED_USRP2_METADATA_H */
