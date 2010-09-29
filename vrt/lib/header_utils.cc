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

#include "header_utils.h"
#include <vrt/bits.h>
#include <ostream>
#include <time.h>
#include <boost/format.hpp>

using boost::format;
using boost::io::group;

namespace vrt
{
  namespace detail {

    void wr_header(std::ostream &os, uint32_t x)
    {
      os << format("%#10x") % x;

      uint32_t t = (x >> 22) & 0x3;
      switch(t){
      case 0x0:	os << " TSI=NONE";	break;
      case 0x1:	os << " TSI=UTC";	break;
      case 0x2:	os << " TSI=GPS";	break;
      case 0x3:	os << " TSI=OTHER";	break;
      }

      t = (x >> 20) & 0x3;
      switch(t){
      case 0x0:	os << " TSF=NONE";	break;
      case 0x1:	os << " TSF=SAMPLE_CNT";break;
      case 0x2:	os << " TSI=PICOSECS";	break;
      case 0x3:	os << " TSI=FREE_RUN";	break;
      }

      uint32_t pt = x & VRTH_PT_MASK;
      if (pt == VRTH_PT_IF_CONTEXT || pt == VRTH_PT_EXT_CONTEXT){
	if (x & VRTH_TSM)
	  os << " TSM=GENERAL";
	else
	  os << " TSM=EXACT";
      }
      else if (0
	       || pt == VRTH_PT_IF_DATA_WITH_SID
	       || pt == VRTH_PT_IF_DATA_NO_SID
	       || pt == VRTH_PT_EXT_DATA_WITH_SID
	       || pt == VRTH_PT_EXT_DATA_NO_SID){
	if (x & VRTH_START_OF_BURST)
	  os << " SOB";
	if (x & VRTH_END_OF_BURST)
	  os << " EOB";
      }

      os << std::endl;
    }

    void wr_int_secs(std::ostream &os, uint32_t secs)
    {
      os << format("%10d") % secs;

      time_t t = secs;
      struct tm r;

      if (gmtime_r(&t, &r)){
	// ISO 8601 date + time
	os << format("  %04d-%02d-%02d %02d:%02d:%02dZ")
	  % (r.tm_year + 1900) % (r.tm_mon + 1) % r.tm_mday
	  % r.tm_hour % r.tm_min % r.tm_sec;
      }

      os << std::endl;
    }

    void wr_name(std::ostream &os, const std::string &x)
    {
      os << format("  %-21s ") % (x + ":");
    }

    void wr_uint32_hex(std::ostream &os, uint32_t x)
    {
      os << format("%#10x") % x;
      os << std::endl;
    }

    void wr_uint32_dec(std::ostream &os, uint32_t x)
    {
      os << format("%12d") % x;
      os << std::endl;
    }

    void wr_hertz(std::ostream &os, vrt_hertz_t x)
    {
      os << format("%12g Hz") % vrt_hertz_to_double(x);
      os << std::endl;
    }

    void wr_dbm(std::ostream &os, vrt_db_t x)
    {
      os << format("%10g dBm") % vrt_db_to_double(x);
      os << std::endl;
    }

    void wr_db(std::ostream &os, vrt_db_t x)
    {
      os << format("%10g dB") % vrt_db_to_double(x);
      os << std::endl;
    }

    void wr_temp(std::ostream &os, vrt_temp_t x)
    {
      os << format("%10g C") % vrt_temp_to_double(x);
      os << std::endl;
    }

    void wr_angle(std::ostream &os, vrt_angle_t x)
    {
      if (x == VRT_GPS_UNKNOWN_VALUE)
	os << "   UNKNOWN";
      else
	os << format("%10g deg") % vrt_angle_to_double(x);

      os << std::endl;
    }

    void wr_distance(std::ostream &os, vrt_distance_t x)
    {
      if (x == VRT_GPS_UNKNOWN_VALUE)
	os << "   UNKNOWN";
      else
	os << format("%10g m") % vrt_distance_to_double(x);

      os << std::endl;
    }

    void wr_velocity(std::ostream &os, vrt_velocity_t x)
    {
      if (x == VRT_GPS_UNKNOWN_VALUE)
	os << "   UNKNOWN";
      else
	os << format("%10g m/s") % vrt_velocity_to_double(x);

      os << std::endl;
    }

    void wr_payload_fmt(std::ostream &os, vrt_payload_fmt_t f)
    {
      if (f.word0 & DF0_PACKED)
	os << " Packed";

      switch (f.word0 & DF0_REAL_CMPLX_TYPE_MASK){
      case DF0_REAL_CMPLX_TYPE_REAL:	    os << " Real";	break;
      case DF0_REAL_CMPLX_TYPE_CMPLX_CART:    os << " Cmplx";	break;
      case DF0_REAL_CMPLX_TYPE_CMPLX_POLAR:   os << " Polar";	break;
      case DF0_REAL_CMPLX_TYPE_RESERVED:	    os << " Reserved"; 	break;
      }

      switch(f.word0 & DF0_ITEM_FMT_MASK){
      case DF0_ITEM_FMT_SIGNED_FIXED_POINT:   os << " S-Fixed";	break;
      case DF0_ITEM_FMT_UNSIGNED_FIXED_POINT: os << " U-Fixed";	break;
      case DF0_ITEM_FMT_IEEE_FLOAT:           os << " float"; 	break;
      case DF0_ITEM_FMT_IEEE_DOUBLE:          os << " double"; 	break;
      default:
	os << format(" ItemFmt=%#x") % ((f.word0 & DF0_ITEM_FMT_MASK) >> 24);
	break;
      }

      if (f.word0 & DF0_SAMPLE_COMPONENT_REPEATING)
	os << " SampleCompRpt";

      uint32_t t = (f.word0 & DF0_EVENT_TAG_SIZE_MASK) >> DF0_EVENT_TAG_SIZE_SHIFT;
      if (t != 0)
	os << format(" EvtTagSize=%d") % t;

      t = (f.word0 & DF0_CHANNEL_TAG_SIZE_MASK) >> DF0_CHANNEL_TAG_SIZE_SHIFT;
      if (t != 0)
	os << format(" ChanTagSize=%d") % t;

      t = (f.word0 & DF0_ITEM_PACKING_FIELD_SIZE_MASK) >> DF0_ITEM_PACKING_FIELD_SIZE_SHIFT;
      os << format(" FieldSize=%d") % (t + 1);

      t = (f.word0 & DF0_DATA_ITEM_SIZE_MASK) >> DF0_DATA_ITEM_SIZE_SHIFT;
      os << format(" ItemSize=%d") % (t + 1);

      t = (f.word1 & DF1_REPEAT_COUNT_MASK) >> DF1_REPEAT_COUNT_SHIFT;
      os << format(" RptCnt=%d") % (t + 1);

      t = (f.word1 & DF1_VECTOR_SIZE_MASK) >> DF1_VECTOR_SIZE_SHIFT;
      os << format(" VectSize=%d") % (t + 1);

      os << std::endl;
    }

    void wr_formatted_gps(std::ostream &os,
			  const vrt_formatted_gps_t &x)
    {
      uint32_t t = (x.tsi_tsf_manuf_oui >> 26) & 0x3;
      switch(t){
      case 0x0:	os << " TSI=UNDEF";	break;
      case 0x1:	os << " TSI=UTC";	break;
      case 0x2:	os << " TSI=GPS";	break;
      case 0x3:	os << " TSI=OTHER";	break;
      }

      t = (x.tsi_tsf_manuf_oui >> 24) & 0x3;
      switch(t){
      case 0x0:	os << " TSF=UNDEF";	break;
      case 0x1:	os << " TSF=SAMPLE_CNT";break;
      case 0x2:	os << " TSI=PICOSECS";	break;
      case 0x3:	os << " TSI=FREE_RUN";	break;
      }

      t = x.tsi_tsf_manuf_oui & 0x00ffffff;
      os << format(" manuf_oui=%#x\n") % t;

      wr_name(os, "  fix int secs");
      //os << format("%10d\n") % x.integer_secs;
      wr_int_secs(os, x.integer_secs);

      wr_name(os, "  fix frac secs");
      os << format("%10d\n") % get_frac_secs(&x.fractional_secs);

      wr_name(os, "  latitude");
      wr_angle(os, x.latitude);
      wr_name(os, "  longitude");
      wr_angle(os, x.longitude);
      wr_name(os, "  altitude");
      wr_distance(os, x.altitude);
      wr_name(os, "  speed_over_ground");
      wr_velocity(os, x.speed_over_ground);
      wr_name(os, "  heading_angle");
      wr_angle(os, x.heading_angle);
      wr_name(os, "  track_angle");
      wr_angle(os, x.track_angle);
      wr_name(os, "  magnetic_variation");
      wr_angle(os, x.magnetic_variation);

      os << std::endl;
    }

  };
};
