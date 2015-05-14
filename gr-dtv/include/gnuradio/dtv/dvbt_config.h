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

#ifndef INCLUDED_DTV_DVBT_CONFIG_H
#define INCLUDED_DTV_DVBT_CONFIG_H

namespace gr {
  namespace dtv {
    enum dvbt_hierarchy_t {
      NH = 0,
      ALPHA1,
      ALPHA2,
      ALPHA4,
    };

    enum dvbt_transmission_mode_t {
      T2k = 0,
      T8k = 1,
    };

  } // namespace dtv
} // namespace gr

typedef gr::dtv::dvbt_hierarchy_t dvbt_hierarchy_t;
typedef gr::dtv::dvbt_transmission_mode_t dvbt_transmission_mode_t;

#endif /* INCLUDED_DTV_DVBT_CONFIG_H */

