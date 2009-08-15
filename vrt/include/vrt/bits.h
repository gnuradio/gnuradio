/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VRT_BITS_H
#define INCLUDED_VRT_BITS_H

#include <stdint.h>


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

#endif /* INCLUDED_VRT_BITS_H */
