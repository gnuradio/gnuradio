/* -*- c++ -*- */
/* 
 * Copyright 2017 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DTV_CATV_CONFIG_H
#define INCLUDED_DTV_CATV_CONFIG_H

namespace gr {
  namespace dtv {
    enum catv_constellation_t {
      CATV_MOD_64QAM = 0,
      CATV_MOD_256QAM,
    };

  } // namespace dtv
} // namespace gr

typedef gr::dtv::catv_constellation_t catv_constellation_t;

#endif /* INCLUDED_DTV_CATV_CONFIG_H */

