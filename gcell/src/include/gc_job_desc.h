/* -*- c -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GC_JOB_DESC_H
#define INCLUDED_GC_JOB_DESC_H

/*!
 * This file contains the structures that are used to describe how to
 * call "jobs" that execute on the SPEs.  A "job" is a task, or piece of
 * work that you want to run on an SPE.
 *
 * There is code running in the SPE that knows how to interpret
 * these job descriptions. Thus, in most cases, the overhead
 * of invoking these is very low.
 *
 * The whole "job idea" is SPE centric.  At first pass,
 * the PPE will be constructing jobs and enqueing them.
 * However, there is nothing in the implementation that
 * prohibits SPEs from creating their own jobs in the
 * future.  Also, there is nothing prohibiting SPE-to-SPE
 * DMA's.
 *
 * SPE's dequeue and "pull" jobs to themselves, do the work, then
 * notify the entity that submitted the job.
 */

#include "gc_types.h"
#include "gc_job_desc_private.h"

/*
 * This is C, not C++ code...
 *
 * ...and is used by both PPE and SPE code
 */
__GC_BEGIN_DECLS


//! opaque ID that specifies which code to invoke on the SPE
typedef uint32_t gc_proc_id_t;
#define GCP_UNKNOWN_PROC ((gc_proc_id_t) -1)


//! final job status
typedef enum {
  JS_OK,
  JS_SHUTTING_DOWN,	    // job mananger is shutting down
  JS_TOO_MANY_CLIENTS,	    // too many client threads
  JS_UNKNOWN_PROC,	    // didn't recognize the procedure ID
  JS_BAD_DIRECTION,	    // EA arg has invalid direction
  JS_BAD_EAH,		    // not all EA args have the same high 32 address bits
  JS_BAD_N_DIRECT,	    // too many direct args
  JS_BAD_N_EA,		    // too many EA args
  JS_ARGS_TOO_LONG,  	    // total length of EA args exceeds limit
  JS_BAD_JUJU,	    	    // misc problem: you're having a bad day
  JS_BAD_JOB_DESC,	    // gc_job_desc was not allocated using mgr->alloc_job_desc()

} gc_job_status_t;

#define MAX_ARGS_DIRECT  8  // maximum number of args passed using "direct" method
#define MAX_ARGS_EA	 8  // maximum number of args passed via EA memory (dma)

/*
 * We support two classes of arguments,
 *   "direct", which are contained in the gc_job_desc_args and
 *   "EA", which are copied in/out according to info in gc_job_desc_args
 */

/*!
 * \brief Tag type of "direct" argument
 */
typedef enum {
  GCT_S32,
  GCT_U32,
  GCT_S64,
  GCT_U64,
  GCT_FLOAT,
  GCT_DOUBLE,
  GCT_FLT_CMPLX,
  GCT_DBL_CMPLX,
  GCT_EADDR,

} gc_tag_t;


/*!
 * \brief union for passing "direct" argument
 */
typedef union gc_arg_union
{
  int32_t	 s32;
  uint32_t	 u32;
  int64_t	 s64;
  uint64_t	 u64;
  float		 f;
  double	 d;
  //float complex  cf;	//  64-bits (C99)
  //double complex cd;  // 128-bits (C99)
  gc_eaddr_t	 ea;	//  64-bits
} _AL8 gc_arg_union_t;


/*!
 * \brief "direct" input or output arguments
 */
typedef struct gc_job_direct_args
{
  uint32_t	  nargs;		        // # of "direct" args
  gc_tag_t	  tag[MAX_ARGS_DIRECT] _AL16;   // type of direct arg[i]
  gc_arg_union_t  arg[MAX_ARGS_DIRECT] _AL16;   // direct argument values

} _AL16 gc_job_direct_args_t;


// specifies direction for args passed in EA memory

#define GCJD_DMA_GET		0x01		// in to SPE
#define	GCJD_DMA_PUT		0x02		// out from SPE

/*!
 * \brief Description of args passed in EA memory.
 * These are DMA'd between EA and LS as specified.
 */
typedef struct gc_job_ea_arg {
  //! EA address of buffer
  gc_eaddr_t     ea_addr;	

  //! GC_JD_DMA_* get arg or put arg
  uint32_t	 direction;

  //! number of bytes to get
  uint32_t	 get_size;	   

  //! number of bytes to put
  uint32_t	 put_size;

#if defined(__SPU__)
  //! local store address (filled in by SPU runtime)
  void		*ls_addr;
  uint32_t	 _pad[2];
#else
  uint32_t       _pad[3];
#endif

} _AL16 gc_job_ea_arg_t;


typedef struct gc_job_ea_args {
  uint32_t		nargs;
  gc_job_ea_arg_t	arg[MAX_ARGS_EA];

} _AL16 gc_job_ea_args_t;


/*!
 * \brief "job description" that is DMA'd to/from the SPE.
 */
typedef struct gc_job_desc
{
  gc_job_desc_private_t sys;	  // internals
  gc_job_status_t   	status;	  // what happened (output)
  gc_proc_id_t          proc_id;  // specifies which procedure to run
  gc_job_direct_args_t	input;	  // direct args to SPE
  gc_job_direct_args_t	output;	  // direct args from SPE
  gc_job_ea_args_t	eaa;	  // args passed via EA memory

} _AL128 gc_job_desc_t;


/*!
 * type of procedure invoked on spu
 */
typedef void (*gc_spu_proc_t)(const gc_job_direct_args_t *input,
			      gc_job_direct_args_t *output,
			      const gc_job_ea_args_t *eaa);

#if !defined(__SPU__)

static inline gc_job_desc_t *
ea_to_jdp(gc_eaddr_t ea)
{
  return (gc_job_desc_t *) ea_to_ptr(ea);
}

static inline gc_eaddr_t
jdp_to_ea(gc_job_desc_t *item)
{
  return ptr_to_ea(item);
}

#endif


__GC_END_DECLS

#endif /* INCLUDED_GC_JOB_DESC_H */
