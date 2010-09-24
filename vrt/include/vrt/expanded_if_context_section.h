/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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
#ifndef INCLUDED_VRT_EXPANDED_IF_CONTEXT_SECTION_H
#define INCLUDED_VRT_EXPANDED_IF_CONTEXT_SECTION_H

#include <string>
#include <vector>

/*!
 * Expanded (unpacked) version of Context Section, defined in VRT section 7.1.5
 */

#include <stdint.h>
#include <stddef.h>
#include <vrt/bits.h>
#include <vrt/types.h>

namespace vrt {
  
  //! Expanded GPS ASCII field
  struct exp_gps_ascii {
    uint32_t	manuf_oui;	// GSP/INS Manufacturer OUI
    std::string	ascii;
  };

  //! Expanded Context Association Lists
  struct exp_context_assocs {
    std::vector<uint32_t> source;	// stream-ids
    std::vector<uint32_t> system;	// stream-ids
    std::vector<uint32_t> vector_comp;	// stream-ids
    std::vector<uint32_t> async_channel;// stream-ids
    std::vector<uint32_t> async_tag;	// channel tags
  };

  /*!
   * \brief Expanded (unpacked) version of the contents of an
   * IF-Context packet's Context Section, as defined in VRT section 7.1.5
   *
   * There are fields allocated for each possible field.  Their
   * content may or may not be valid.  Check the context_indicator
   * field to determinte their validity.  If the associated CI_* bit
   * is set the corresponding field is valid.
   *
   * All values are in host-endian format.
   */
  struct expanded_if_context_section {
    uint32_t	context_indicator;	//< 7.1.5.1
					//  7.1.5.2 (no field)
    uint32_t	ref_point_id;		//< 7.1.5.3
    vrt_hertz_t	bandwidth;		//< 7.1.5.4
    vrt_hertz_t	if_ref_freq;		//< 7.1.5.5
    vrt_hertz_t	rf_ref_freq;		//< 7.1.5.6
    vrt_hertz_t	rf_ref_freq_offset;	//< 7.1.5.7
    vrt_hertz_t	if_band_offset;		//< 7.1.5.8
    vrt_db_t	ref_level;		//< 7.1.5.9 (dBm)
    vrt_gain_t	gain;			//< 7.1.5.10
    uint32_t	over_range_count;	//< 7.1.5.11
    vrt_hertz_t	sample_rate;		//< 7.1.5.12
    int64_t	timestamp_adj;		//< 7.1.5.13 (picoseconds)
    uint32_t	timestamp_cal_time;	//< 7.1.5.14 (TSI seconds)
    vrt_temp_t	temperature;		//< 7.1.5.15
    uint32_t	device_id[2];		//< 7.1.5.16 (0: OUI; 1: dev code)
    uint32_t	state_and_event_ind;	//< 7.1.5.17
    vrt_payload_fmt_t payload_fmt;	//< 7.1.5.18
    vrt_formatted_gps_t formatted_gps;	//< 7.1.5.19
    vrt_formatted_gps_t	formatted_ins;	//< 7.1.5.20
    vrt_ephemeris_t ecef_ephemeris;	//< 7.1.5.21
    vrt_ephemeris_t rel_ephemeris;	//< 7.1.5.22
    int32_t	ephemeris_ref_id;	//< 7.1.5.23
    exp_gps_ascii  gps_ascii;		//< 7.1.5.24
    exp_context_assocs cntx_assoc_lists;//< 7.1.5.25
  };

}; // namespace vrt

#endif /* INCLUDED_VRT_EXPANDED_IF_CONTEXT_SECTION_H */
