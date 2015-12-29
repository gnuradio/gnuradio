/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DTV_DVBS2_CONFIG_H
#define INCLUDED_DTV_DVBS2_CONFIG_H

namespace gr {
  namespace dtv {
    enum dvbs2_rolloff_factor_t {
      RO_0_35 = 0,
      RO_0_25,
      RO_0_20,
      RO_RESERVED,
      RO_0_15,
      RO_0_10,
      RO_0_05,
    };

    enum dvbs2_pilots_t {
      PILOTS_OFF = 0,
      PILOTS_ON,
    };

    enum dvbs2_interpolation_t {
      INTERPOLATION_OFF = 0,
      INTERPOLATION_ON,
    };

  } // namespace dtv
} // namespace gr

typedef gr::dtv::dvbs2_rolloff_factor_t dvbs2_rolloff_factor_t;
typedef gr::dtv::dvbs2_pilots_t dvbs2_pilots_t;
typedef gr::dtv::dvbs2_interpolation_t dvbs2_interpolation_t;

#endif /* INCLUDED_DTV_DVBS2_CONFIG_H */

