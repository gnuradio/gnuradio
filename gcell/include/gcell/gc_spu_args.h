/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GCELL_GC_SPU_ARGS_H
#define INCLUDED_GCELL_GC_SPU_ARGS_H

#include <gcell/gc_types.h>
#include <gcell/gc_logging.h>

// args passed to SPE at initialization time

typedef struct gc_spu_args {
  gc_eaddr_t	queue;		// address of job queue (gc_jd_queue_t *)
  gc_eaddr_t	comp_info[2]; 	// completion info (gc_comp_info_t *)
  uint32_t	spu_idx;	// which spu we are: [0,nspus-1]
  uint32_t	nspus;		// number of spus we're using
  uint32_t	proc_def_ls_addr;  // LS addr of proc_def table
  uint32_t	nproc_defs;	   // number of proc_defs in table
  gc_log_t	log;		   // logging info
} _AL16 gc_spu_args_t;


#define	GC_CI_NJOBS	62	// makes gc_comp_info 1 cache line long

/*!
 * \brief Used to return info to PPE on which jobs are completed.
 *
 * When each SPE is initalized, it is passed EA pointers to two of
 * these structures.  The SPE uses these to communicate which jobs
 * that it has worked on are complete.  The SPE notifies the PPE by
 * sending an OP_JOBS_DONE message (see gc_mbox.h) with an argument of
 * 0 or 1, indicating which of the two comp_info's to examine.  The
 * SPE sets the in_use flag to 1 before DMA'ing to the PPE.  When the
 * PPE is done with the structure, it must clear the in_use field to
 * let the SPE know it can begin using it again.
 */
typedef struct gc_comp_info {
  uint16_t	in_use;		// set by SPE, cleared by PPE when it's finished
  uint16_t	ncomplete;	// number of valid job_id's
  uint16_t	job_id[GC_CI_NJOBS];	// job_id's of completed jobs
} _AL128 gc_comp_info_t;

#endif /* INCLUDED_GCELL_GC_SPU_ARGS_H */
