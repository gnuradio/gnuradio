/* -*- c -*- */
/*
 * Copyright 2009,2010 Free Software Foundation, Inc.
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
#ifndef INCLUDED_VRT_BITS_H
#define INCLUDED_VRT_BITS_H

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Definitions of bit and fields in VRT packets
 *
 * See "ANSI/VITA 49.0, VRT Standard" for details
 */

#include <stdint.h>
#include <vrt/types.h>

// ------------------------------------------------------------------------
// 		     Common VRT header info
// ------------------------------------------------------------------------

/* VRT Header bits */

#define	VRTH_PT_MASK		  (0xf << 28)
#define	VRTH_PT_IF_DATA_NO_SID	  (0x0 << 28)	// IF-Data, no stream id
#define	VRTH_PT_IF_DATA_WITH_SID  (0x1 << 28)	// IF-Data, w/ stream id
#define VRTH_PT_EXT_DATA_NO_SID	  (0x2 << 28)
#define	VRTH_PT_EXT_DATA_WITH_SID (0x3 << 28)
#define	VRTH_PT_IF_CONTEXT	  (0x4 << 28)
#define	VRTH_PT_EXT_CONTEXT	  (0x5 << 28)

#define	VRTH_HAS_CLASSID	  (1 << 27)
#define	VRTH_HAS_TRAILER	  (1 << 26)	// Data pkts only
#define	VRTH_START_OF_BURST	  (1 << 25)	// Data (Tx) pkts only
#define	VRTH_END_OF_BURST	  (1 << 24)	// Data (Tx) pkts only
#define	VRTH_TSM		  (1 << 24)	// Context pkts only

#define	VRTH_TSI_MASK		  (0x3 << 22)
#define	VRTH_TSI_NONE		  (0x0 << 22)
#define	VRTH_TSI_UTC		  (0x1 << 22)
#define	VRTH_TSI_GPS		  (0x2 << 22)
#define VRTH_TSI_OTHER		  (0x3 << 22)

#define	VRTH_TSF_MASK		  (0x3 << 20)
#define	VRTH_TSF_NONE		  (0x0 << 20)
#define	VRTH_TSF_SAMPLE_CNT	  (0x1 << 20)
#define	VRTH_TSF_REAL_TIME_PS	  (0x2 << 20)
#define	VRTH_TSF_FREE_RUNNING	  (0x3 << 20)

#define	VRTH_PKT_CNT_SHIFT	  16
#define	VRTH_PKT_CNT_MASK	  (0xf << 16)

#define	VRTH_PKT_SIZE_MASK	  0xffff


static inline int
vrth_pkt_cnt(uint32_t h)
{
  return (h & VRTH_PKT_CNT_MASK) >> 16;
}

static inline int
vrth_pkt_size(uint32_t h)
{
  return h & VRTH_PKT_SIZE_MASK;
}

/*
 * Trailer bits
 */
#define	TR_E		      (1 << 8)

#define TR_ENABLE(x) ((x) << 20)
#define	TR_STATE(x)  ((x) <<  8)

// Use these with TR_ENABLE and TR_STATE
#define	TR_CAL_TIME	      (1 << 11)
#define	TR_VALID_DATA	      (1 << 10)
#define TR_REF_LOCK	      (1 <<  9)
#define	TR_AGC		      (1 <<  8)
#define TR_DETECTED_SIG	      (1 <<  7)
#define	TR_SPECTRAL_INVERSION (1 <<  6)
#define	TR_OVER_RANGE	      (1 <<  5)
#define	TR_SAMPLE_LOSS	      (1 <<  4)
#define TR_USER_3	      (1 <<  3)
#define TR_USER_2	      (1 <<  2)
#define TR_USER_1	      (1 <<  1)
#define TR_USER_0	      (1 <<  0)

// ------------------------------------------------------------------------
//		      IF-Context packets
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
// Context Indicator field bits (Table 7.1.5.1-1)

#define CI_CHANGE_INDICATOR	(1 << 31)
#define	CI_REF_POINT_ID		(1 << 30)
#define	CI_BANDWIDTH	       	(1 << 29)
#define	CI_IF_REF_FREQ		(1 << 28)
#define	CI_RF_REF_FREQ		(1 << 27)
#define	CI_RF_REF_FREQ_OFFSET	(1 << 26)
#define CI_IF_BAND_OFFSET	(1 << 25)
#define	CI_REF_LEVEL		(1 << 24)
#define CI_GAIN			(1 << 23)
#define	CI_OVER_RANGE_COUNT	(1 << 22)
#define	CI_SAMPLE_RATE		(1 << 21)
#define	CI_TIMESTAMP_ADJ	(1 << 20)
#define	CI_TIMESTAMP_CAL_TIME	(1 << 19)
#define	CI_TEMPERATURE		(1 << 18)
#define CI_DEVICE_ID		(1 << 17)
#define	CI_STATE_AND_EVENT_IND	(1 << 16)
#define	CI_PAYLOAD_FMT		(1 << 15)
#define	CI_FORMATTED_GPS	(1 << 14)
#define	CI_FORMATTED_INS	(1 << 13)
#define	CI_ECEF_EPHEMERIS	(1 << 12)
#define	CI_REL_EPHEMERIS	(1 << 11)
#define	CI_EPHEMERIS_REF_ID	(1 << 10)
#define	CI_GPS_ASCII		(1 <<  9)
#define	CI_CNTX_ASSOC_LISTS	(1 <<  8)
#define CI_RESERVED_7		(1 <<  7)
#define CI_RESERVED_6		(1 <<  6)
#define CI_RESERVED_5		(1 <<  5)
#define CI_RESERVED_4		(1 <<  4)
#define CI_RESERVED_3		(1 <<  3)
#define CI_RESERVED_2		(1 <<  2)
#define CI_RESERVED_1		(1 <<  1)
#define CI_RESERVED_0		(1 <<  0)


// ------------------------------------------------------------------------
// Types that may be in the Context Section


typedef struct {
  uint32_t	hi;	// most significant 32-bits
  uint32_t	lo;	// least significant 32-bits
} vrt_frac_secs_t;

static inline uint64_t
get_frac_secs(const vrt_frac_secs_t *fs)
{
  return (((uint64_t)fs->hi) << 32) | fs->lo;
}

static inline void
put_frac_secs(vrt_frac_secs_t *fs, uint64_t x)
{
  fs->hi = (x >> 32);
  fs->lo = x;
}

//! See VITA-49.0 section 7.1.5.19

typedef struct vrt_formatted_gps_tag {
  uint32_t	  tsi_tsf_manuf_oui;
  uint32_t	  integer_secs;		// timestamp of position fix
  vrt_frac_secs_t fractional_secs;	// timestamp of position fix
  vrt_angle_t	  latitude;		// degrees  Q10.22
  vrt_angle_t	  longitude;		// degrees  Q10.22
  vrt_distance_t  altitude;		// meters   Q27.5
  vrt_velocity_t  speed_over_ground;	// meters/s Q16.16
  vrt_angle_t	  heading_angle;	// degrees  Q10.22
  vrt_angle_t	  track_angle;		// degrees  Q10.22
  vrt_angle_t	  magnetic_variation;	// degrees  Q10.22
} vrt_formatted_gps_t;

/*!
 * \brief GPS Unknown value indicator
 *
 * May be used in: latitude, longitude, altitude, speed_over_ground,
 * heading_angle, track_angle, and magnetic_variation fields.
 */
#define	VRT_GPS_UNKNOWN_VALUE	0x7fffffff



//! Data Packet Payload Format Field 7.1.5.18

typedef struct {
  uint32_t	word0;
  uint32_t	word1;
} vrt_payload_fmt_t;

#define DF0_PACKED				(1 << 31)
#define	DF0_REAL_CMPLX_TYPE_MASK		(0x3 << 29)
#define	  DF0_REAL_CMPLX_TYPE_REAL		(0 << 29)
#define	  DF0_REAL_CMPLX_TYPE_CMPLX_CART	(1 << 29)
#define	  DF0_REAL_CMPLX_TYPE_CMPLX_POLAR	(2 << 29)
#define	  DF0_REAL_CMPLX_TYPE_RESERVED		(3 << 29)
#define DF0_ITEM_FMT_MASK			(0x1f << 24)
#define   DF0_ITEM_FMT_SIGNED_FIXED_POINT	(0x00 << 24)
#define   DF0_ITEM_FMT_UNSIGNED_FIXED_POINT	(0x10 << 24)
// lots more we're not using
#define	DF0_SAMPLE_COMPONENT_REPEATING		(1 << 23)
#define	DF0_EVENT_TAG_SIZE_SHIFT		20
#define DF0_EVENT_TAG_SIZE_MASK			(0x7 << 20)
#define	DF0_CHANNEL_TAG_SIZE_SHIFT		16
#define	DF0_CHANNEL_TAG_SIZE_MASK		(0xf << 16)
#define	DF0_RESERVED_SHIFT			12
#define	DF0_RESERVED_MASK			(0xf << 12)
#define DF0_ITEM_PACKING_FIELD_SIZE_SHIFT	6
#define DF0_ITEM_PACKING_FIELD_SIZE_MASK	(0x3f << 6)
#define	DF0_DATA_ITEM_SIZE_SHIFT		0
#define	DF0_DATA_ITEM_SIZE_MASK			(0x3f << 0)

#define	DF1_REPEAT_COUNT_SHIFT			16
#define	DF1_REPEAT_COUNT_MASK			(0xffff << 16)
#define	DF1_VECTOR_SIZE_SHIFT			0
#define	DF1_VECTOR_SIZE_MASK			(0xffff <<  0)


// Composite values

// 16-bit I & Q, no events, no channel tags
#define DF0_16_BIT_IQ  \
  (0 \
   | DF0_REAL_CMPLX_TYPE_CMPLX_CART  \
   | DF0_ITEM_FMT_SIGNED_FIXED_POINT \
   | (15 << DF0_ITEM_PACKING_FIELD_SIZE_SHIFT) \
   | (15 << DF0_DATA_ITEM_SIZE_SHIFT))

// Vector of length 1
#define DF1_VECTOR_1 (0 << DF1_VECTOR_SIZE_SHIFT)

// Vector of length 4
#define DF1_VECTOR_4 (3 << DF1_VECTOR_SIZE_SHIFT)


//! ECEF Ephemeris Field 7.1.5.21

typedef struct {
  uint32_t	  tsi_tsf_manuf_oui;
  uint32_t	  integer_secs;		// timestamp of position fix
  vrt_frac_secs_t fractional_secs;	// timestamp of position fix
  vrt_distance_t  position_x;		// meters   Q27.5
  vrt_distance_t  position_y;		// meters   Q27.5
  vrt_distance_t  position_z;		// meters   Q27.5
  vrt_angle_t	  attitude_alpha;	// degrees  Q10.22
  vrt_angle_t	  attitude_beta;	// degrees  Q10.22
  vrt_angle_t	  attitude_phi;		// degrees  Q10.22
  vrt_velocity_t  velocity_dx;		// meters/s Q16.16
  vrt_velocity_t  velocity_dy;		// meters/s Q16.16
  vrt_velocity_t  velocity_dz;		// meters/s Q16.16
} vrt_ephemeris_t;

#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_VRT_BITS_H */
