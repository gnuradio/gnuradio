/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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
#ifndef INCLUDED_VRT_IF_CONTEXT_H
#define INCLUDED_VRT_IF_CONTEXT_H

#include <stdint.h>

// Stream ID's for packet streams (low 8-bits)

#define QR_SID_BEAMFORMER_IF_DATA	0x00	// paired
#define QR_SID_BEAMFORMER_IF_CNTX	0x00	// paired
#define QR_SID_DBOARD_0_IF_CNTX		0x01	// src assoc
#define QR_SID_DBOARD_1_IF_CNTX		0x02	// src assoc
#define QR_SID_DBOARD_2_IF_CNTX		0x03	// src assoc
#define QR_SID_DBOARD_3_IF_CNTX		0x04	// src assoc
#define QR_SID_GPS_IF_CNTX		0x05	// system assoc
#define QR_SID_SYS_INFO_EXT_CNTX	0x06	// system assoc

// No class id, with 3 words of time.
// Used for all IF-Context packets
typedef struct if_context_hdr_tag {
  uint32_t	header;			// first word of all packets
  uint32_t	stream_id;		// mandatory in context packets
  uint32_t	integer_secs;		// integer seconds timestamp
  uint64_t	fractional_secs;	// fractional seconds timestamp
  uint32_t	context_indicator;	// which context fields follow
} __attribute__((packed)) if_context_hdr_t;


// With class id, with 3 words of time.
// Used only with Extension Context packets
typedef struct if_context_hdr_with_classid_tag {
  uint32_t	header;			// first word of all packets
  uint32_t	stream_id;		// mandatory in context packets
  uint64_t	class_id;		// class identifier
  uint32_t	integer_secs;		// integer seconds timestamp
  uint64_t	fractional_secs;	// fractional seconds timestamp
  uint32_t	context_indicator;	// which context fields follow
} __attribute__((packed)) if_context_hdr_with_classid_t;


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
#define	CI_ASCII_GPS		(1 <<  9)
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


// 7.1.5.10 Gain -- two Q10.6 dB fields

static inline uint32_t
vrt_make_gain_field(int16_t stage1, int16_t stage2)
{
  return (((uint32_t) stage2) << 16) | (stage1 & 0xffff);
}

static inline int16_t
vrt_gain_stage1(uint32_t field)
{
  return (int16_t)(field & 0xffff);
}

static inline int16_t
vrt_gain_stage2(uint32_t field)
{
  return (int16_t)((field >> 16) & 0xffff);
}

// See VITA-49.0 section 7.1.5.19

#define	VRT_GPS_UNKNOWN_VALUE	0x7fffffff	// for SOG, heading, track, and mag var

typedef struct vrt_formatted_gps_tag {
  uint32_t	tsi_tsf_manuf_oui;
  uint32_t	integer_secs;		// timestamp of position fix
  uint64_t	fractional_secs;	// timestamp of position fix
  uint32_t	latitude;		// degrees Q10.22
  uint32_t	longitude;		// degrees Q10.22
  uint32_t	altitude;		// meters  Q27.5
  uint32_t	speed_over_ground;
  uint32_t	heading_angle;
  uint32_t	track_angle;
  uint32_t	magnetic_variation;	
} vrt_formatted_gps_t;


// Data Packet Payload Format Field 7.1.5.18

typedef struct vrt_payload_format_tag {
  uint32_t	word0;
  uint32_t	word1;
} vrt_payload_format_t;

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


// ------------------------------------------------------------------------
// Our context packets

typedef struct beamformer_if_cntx_tag {
  if_context_hdr_t	cntx_hdr;
  uint64_t		rf_ref_freq;	// Q44.20 Hz
  uint64_t		sample_rate;	// Q44.20 Hz
  vrt_payload_format_t	format;
  // FIXME context association lists
} __attribute__((packed)) beamformer_if_cntx_t;

typedef struct gps_if_cntx_tag {
  if_context_hdr_t	cntx_hdr;
  vrt_formatted_gps_t	formatted_gps;
} __attribute__((packed)) gps_if_cntx_t;

typedef struct gain_if_cntx_tag {
  if_context_hdr_t	cntx_hdr;
  uint32_t		gain;
} __attribute__((packed)) gain_if_cntx_t;

typedef struct caldiv_if_cntx_tag {
  if_context_hdr_t	cntx_hdr;
  uint64_t lo_freq;
  uint32_t lo_locked;
  uint64_t cal_freq;
  uint32_t cal_locked;
  uint32_t cal_enabled;
  uint32_t temp;
  uint32_t rev;
  uint32_t ser;
} __attribute__((packed)) caldiv_if_cntx_t;

// FIXME the rest...

// ------------------------------------------------------------------------
// All of them concatenated

#define	NCONTEXT_PKTS	7	// number of context pkts aggregated here

typedef struct all_context_tag {
  beamformer_if_cntx_t	beamformer;
  gain_if_cntx_t	db[4];

  // FIXME add Extension Context for board types, revs, serial numbers, temps
  caldiv_if_cntx_t caldiv;

  gps_if_cntx_t		gps;		// must be last
} __attribute__((packed)) all_context_t;

#define SIZEOF_ALL_CONTEXT	   (sizeof(all_context_t))
#define SIZEOF_ALL_CONTEXT_BUT_GPS (SIZEOF_ALL_CONTEXT - sizeof(gps_if_cntx_t))


#endif /* INCLUDED_IF_CONTEXT_H */
