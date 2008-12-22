/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#include <gcell/gc_logging.h>
#include <gcell/gc_spu_args.h>
#include <spu_intrinsics.h>
#include <spu_mfcio.h>

static gc_eaddr_t     log_base_ea;	// base address of log entries in EA
static uint32_t	      log_idx_mask;	// nentries - 1
static uint32_t	      log_idx;		// current log entry index
static uint32_t	      log_seqno;

static int 	      log_tags;		// two consecutive tags
static int	      tmp_buffer_busy;	// bitmask: buffer busy state
static int	      tmp_buffer_idx;	// 0 or 1
static gc_log_entry_t tmp_buffer[2];

void
_gc_log_init(gc_log_t info)
{
  spu_write_decrementer(~0);

  log_base_ea = info.base;
  log_idx_mask = info.nentries - 1;
  log_idx = 0;
  log_seqno = 0;

  log_tags = mfc_multi_tag_reserve(2);
  tmp_buffer_busy = 0;
  tmp_buffer_idx = 0;

  gc_log_write0(GCL_SS_SYS, 0);
}

void
_gc_log_write(gc_log_entry_t entry)
{
  if (log_base_ea == 0)
    return;

  entry.seqno = log_seqno++;
  entry.timestamp = spu_read_decrementer();

  if (tmp_buffer_busy & (1 << tmp_buffer_idx)){
    mfc_write_tag_mask(1 << (log_tags + tmp_buffer_idx));
    mfc_read_tag_status_all();
  }

  tmp_buffer[tmp_buffer_idx] = entry;	// save local copy

  mfc_put(&tmp_buffer[tmp_buffer_idx],
	  log_base_ea + log_idx * sizeof(entry), sizeof(entry),
	  log_tags + tmp_buffer_idx, 0, 0);

  tmp_buffer_busy |= (1 << tmp_buffer_idx);
  tmp_buffer_idx ^= 0x1;
  log_idx = (log_idx + 1) & log_idx_mask;
}
