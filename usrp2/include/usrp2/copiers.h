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

#ifndef INCLUDED_USRP2_COPIERS_H
#define INCLUDED_USRP2_COPIERS_H

#include <complex>
#include <stdint.h>

namespace usrp2 {

  // FIXME we may want to rework this, but this will get us on the air

  /*
   * ----------------------------------------------------------------
   * Copy and convert from USRP2 wire format to host format
   * ----------------------------------------------------------------
   */

  void 
  copy_u2_16sc_to_host_16sc(size_t nitems,
			    const uint32_t *items,
			    std::complex<int16_t> *host_items);

  void 
  copy_u2_16sc_to_host_32fc(size_t nitems,
			    const uint32_t *items,
			    std::complex<float> *host_items);

  /*
   * ----------------------------------------------------------------
   * Copy and convert from host format to USRP2 wire format
   * ----------------------------------------------------------------
   */

  void 
  copy_host_16sc_to_u2_16sc(size_t nitems,
			    const std::complex<int16_t> *host_items,
			    uint32_t *items);

  void 
  copy_host_32fc_to_u2_16sc(size_t nitems,
			    const std::complex<float> *host_items,
			    uint32_t *items);
}


#endif /* INCLUDED_USRP2_COPIERS_H */
