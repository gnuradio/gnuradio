/* -*- c++ -*- */
/*
 * Copyright 2007,2008,2009 Free Software Foundation, Inc.
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

// #define ENABLE_GC_LOGGING 	// define to enable logging

#include <spu_intrinsics.h>
#include <spu_mfcio.h>
#include <sync_utils.h>
#include "gc_spu_config.h"
#include "spu_buffers.h"
#include <gcell/gc_spu_args.h>
#include <gcell/gc_job_desc.h>
#include <gcell/gc_mbox.h>
#include <gcell/gc_declare_proc.h>
#include <gcell/spu/gc_jd_queue.h>
#include <gcell/spu/gc_random.h>
#include <gcell/spu/gc_delay.h>

#include <string.h>
#include <assert.h>
#include <stdio.h>


#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

//! round x down to p2 boundary (p2 must be a power-of-2)
#define ROUND_DN(x, p2) ((x) & ~((p2)-1))

//! round x up to p2 boundary (p2 must be a power-of-2)
#define ROUND_UP(x, p2) (((x)+((p2)-1)) & ~((p2)-1))


//#define OUT_MBOX_CHANNEL SPU_WrOutIntrMbox
#define OUT_MBOX_CHANNEL SPU_WrOutMbox

#define	CHECK_QUEUE_ON_MSG	0	// define to 0 or 1
#define USE_LLR_LOST_EVENT	0	// define to 0 or 1

int			gc_sys_tag;	// tag for misc DMA operations
static gc_spu_args_t	spu_args;

static struct gc_proc_def *gc_proc_def;	// procedure entry points

// ------------------------------------------------------------------------

// state for DMA'ing arguments in and out

static int get_tag;		// 1 tag for job arg gets
static int put_tags;		// 2 tags for job arg puts

static int pb_idx = 0;		// current put buffer index (0 or 1)

// bitmask (bit per put buffer): bit is set if DMA is started but not complete
static int put_in_progress = 0;
#define PBI_MASK(_pbi_) (1 << (_pbi_))

// ------------------------------------------------------------------------

// our working copy of the completion info
static gc_comp_info_t	comp_info = {  
  .in_use = 1,
  .ncomplete = 0
};

static int ci_idx = 0;		// index of current comp_info
static int ci_tags;		// two consecutive dma tags

// ------------------------------------------------------------------------

/*
 * Wait until EA copy of comp_info[idx].in_use is 0
 */
static void
wait_for_ppe_to_be_done_with_comp_info(int idx)
{
  char _tmp[256];
  char *buf = (char *) ALIGN(_tmp, 128);	// get cache-aligned buffer
  gc_comp_info_t *p = (gc_comp_info_t *) buf;

  assert(sizeof(gc_comp_info_t) == 128);

  do {
    mfc_get(buf, spu_args.comp_info[idx], 128, gc_sys_tag, 0, 0);
    mfc_write_tag_mask(1 << gc_sys_tag);
    mfc_read_tag_status_all();
    if (p->in_use == 0)
      return;

    gc_udelay(1);

  } while (1);
}

static void
flush_completion_info(void)
{
  // events: 0x3X

  static int total_complete = 0;

  if (comp_info.ncomplete == 0)
    return;
  
  // ensure that PPE is done with the buffer we're about to overwrite
  wait_for_ppe_to_be_done_with_comp_info(ci_idx);

  // dma the comp_info out to PPE
  int tag = ci_tags + ci_idx;
  mfc_put(&comp_info, spu_args.comp_info[ci_idx], sizeof(gc_comp_info_t), tag, 0, 0);

  // we need to wait for the completion info to finish, as well as
  // any EA argument puts.

  int tag_mask = 1 << tag;		// the comp_info tag
  if (put_in_progress & PBI_MASK(0))
    tag_mask |= (1 << (put_tags + 0));
  if (put_in_progress & PBI_MASK(1))
    tag_mask |= (1 << (put_tags + 1));

  gc_log_write2(GCL_SS_SYS, 0x30, put_in_progress, tag_mask);

  mfc_write_tag_mask(tag_mask);		// the tags we're interested in
  mfc_read_tag_status_all();		// wait for DMA to complete
  put_in_progress = 0;			// mark them all complete

  total_complete += comp_info.ncomplete;
  gc_log_write4(GCL_SS_SYS, 0x31,
		put_in_progress, ci_idx, comp_info.ncomplete, total_complete);

  // send PPE a message
  spu_writech(OUT_MBOX_CHANNEL, MK_MBOX_MSG(OP_JOBS_DONE, ci_idx));

  ci_idx ^= 0x1;	// switch buffers
  comp_info.in_use = 1;
  comp_info.ncomplete = 0;
}

// ------------------------------------------------------------------------


static unsigned int backoff;		// current backoff value in clock cycles
static unsigned int _backoff_start;
static unsigned int _backoff_cap;

/*
 * For 3.2 GHz SPE
 *
 * 10	 1023 cycles    320 ns
 * 11    2047 cycle     640 ns
 * 12    4095 cycles    1.3 us
 * 13    8191 cycles    2.6 us
 * 14   16383 cycles    5.1 us
 * 15   32767 cycles   10.2 us
 * 16                  20.4 us
 * 17                  40.8 us
 * 18                  81.9 us
 * 19                 163.8 us
 * 20                 327.7 us
 * 21                 655.4 us
 */
static unsigned char log2_backoff_start[16] = {
// 1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16
// -------------------------------------------------------------
//12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 16, 16
  11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11
};
  
static unsigned char log2_backoff_cap[16] = {
// 1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16
// -------------------------------------------------------------
//17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 21, 21
  13, 14, 14, 14, 14, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16
};
  
static void
backoff_init(void)
{
  _backoff_cap   = (1 << (log2_backoff_cap[(spu_args.nspus - 1) & 0xf])) - 1;
  _backoff_start = (1 << (log2_backoff_start[(spu_args.nspus - 1) & 0xf])) - 1;
  
  backoff = _backoff_start;
}

#if !CHECK_QUEUE_ON_MSG

static void 
backoff_reset(void)
{
  backoff = _backoff_start;
}


#define RANDOM_WEIGHT	0.2

static void
backoff_delay(void)
{
  gc_cdelay(backoff);

  // capped exponential backoff
  backoff = ((backoff << 1) + 1);
  if (backoff > _backoff_cap)
    backoff = _backoff_cap;

  // plus some randomness
  float r = (RANDOM_WEIGHT * (2.0 * (gc_uniform_deviate() - 0.5)));
  backoff = backoff * (1.0 + r);
}

#endif	// !CHECK_QUEUE_ON_MSG

// ------------------------------------------------------------------------

static inline unsigned int
make_mask(int nbits)
{
  return ~(~0 << nbits);
}

static unsigned int   dc_work;
static int            dc_put_tag;
static unsigned char *dc_ls_base;
static gc_eaddr_t     dc_ea_base;

// divide and conquer
static void
d_and_c(unsigned int offset, unsigned int len)
{
  unsigned int mask = make_mask(len) << offset;
  unsigned int t = mask & dc_work;
  if (t == 0)		// nothing to do
    return;
  if (t == mask){	// got a match, generate dma
    mfc_put(dc_ls_base + offset, dc_ea_base + offset, len, dc_put_tag, 0, 0);
  }
  else {		// bisect
    len >>= 1;
    d_and_c(offset, len);
    d_and_c(offset + len, len);
  }
}

// Handle the nasty case of a dma xfer that's less than 16 bytes long.
// len is guaranteed to be in [1, 15]

static void
handle_slow_and_tedious_dma(gc_eaddr_t ea, unsigned char *ls,
			    unsigned int len, int put_tag)
{
  // Set up for divide and conquer
  unsigned int alignment = ((uintptr_t) ls) & 0x7;
  dc_work = make_mask(len) << alignment;
  dc_ls_base = (unsigned char *) ROUND_DN((uintptr_t) ls, 8);
  dc_ea_base = ROUND_DN(ea, (gc_eaddr_t) 8);
  dc_put_tag = put_tag;

  d_and_c( 0, 8);
  d_and_c( 8, 8);
  d_and_c(16, 8);
}


static void
process_job(gc_eaddr_t jd_ea, gc_job_desc_t *jd)
{
  // events: 0x2X

  jd->status = JS_OK;	// assume success

  if (jd->proc_id >= spu_args.nproc_defs)
    jd->status = JS_UNKNOWN_PROC;
    
  else {
  
    if (jd->eaa.nargs == 0)
      (*gc_proc_def[jd->proc_id].proc)(&jd->input, &jd->output, &jd->eaa);

    else {	// handle EA args that must be DMA'd in/out

      gc_job_ea_args_t *eaa = &jd->eaa;

      int NELMS =
	MAX(MAX_ARGS_EA,
	    (GC_SPU_BUFSIZE + MFC_MAX_DMA_SIZE - 1) / MFC_MAX_DMA_SIZE);

      mfc_list_element_t  dma_get_list[NELMS];
      //mfc_list_element_t  dma_put_list[NELMS];
      
      memset(dma_get_list, 0, sizeof(dma_get_list));
      //memset(dma_put_list, 0, sizeof(dma_put_list));

      int gli = 0;	// get list index
      //int pli = 0;	// put list index

      unsigned char *get_base = _gci_getbuf[0];
      unsigned char *get_t = get_base;
      unsigned int   total_get_dma_len = 0;

      unsigned char *put_base = _gci_putbuf[pb_idx];
      unsigned char *put_t = put_base;
      unsigned int   total_put_alloc = 0;
      int	     put_tag = put_tags + pb_idx;

      // Do we have any "put" args?  If so ensure that previous
      // dma from this buffer is complete

      gc_log_write2(GCL_SS_SYS, 0x24, put_in_progress, jd->sys.direction_union);

      if ((jd->sys.direction_union & GCJD_DMA_PUT)
	  && (put_in_progress & PBI_MASK(pb_idx))){

	gc_log_write2(GCL_SS_SYS, 0x25, put_in_progress, 1 << put_tag);

	mfc_write_tag_mask(1 << put_tag);	// the tag we're interested in
	mfc_read_tag_status_all();		// wait for DMA to complete
	put_in_progress &= ~(PBI_MASK(pb_idx));

	gc_log_write1(GCL_SS_SYS, 0x26, put_in_progress);
      }


      // for now, all EA's must have the same high 32-bits
      gc_eaddr_t common_ea = eaa->arg[0].ea_addr;


      // assign LS addresses for buffers
      
      for (unsigned int i = 0; i < eaa->nargs; i++){

	gc_eaddr_t  	ea_base = 0;
	unsigned char  *ls_base;
	int	  	offset;
	unsigned int	dma_len;

	if (eaa->arg[i].direction == GCJD_DMA_GET){
	  ea_base = ROUND_DN(eaa->arg[i].ea_addr, (gc_eaddr_t) CACHE_LINE_SIZE);
	  offset = eaa->arg[i].ea_addr & (CACHE_LINE_SIZE-1);
	  dma_len = ROUND_UP(eaa->arg[i].get_size + offset, CACHE_LINE_SIZE);
	  total_get_dma_len += dma_len;

	  if (total_get_dma_len > GC_SPU_BUFSIZE){
	    jd->status = JS_ARGS_TOO_LONG;
	    goto wrap_up;
	  }

	  ls_base = get_t;
	  get_t += dma_len;
	  eaa->arg[i].ls_addr = ls_base + offset;

	  if (0){
	    assert((mfc_ea2l(eaa->arg[i].ea_addr) & 0x7f) == ((intptr_t)eaa->arg[i].ls_addr & 0x7f));
	    assert((ea_base & 0x7f) == 0);
	    assert(((intptr_t)ls_base & 0x7f) == 0);
	    assert((dma_len & 0x7f) == 0);
	    assert((eaa->arg[i].get_size <= dma_len)
		   && dma_len <= (eaa->arg[i].get_size + offset + CACHE_LINE_SIZE - 1));
	  }

	  // add to dma get list 
	  // FIXME (someday) the dma list is where the JS_BAD_EAH limitation comes from

	  while (dma_len != 0){
	    int n = MIN(dma_len, MFC_MAX_DMA_SIZE);
	    dma_get_list[gli].size = n;
	    dma_get_list[gli].eal = mfc_ea2l(ea_base);
	    dma_len -= n;
	    ea_base += n;
	    gli++;
	  }
	}

	else if (eaa->arg[i].direction == GCJD_DMA_PUT){
	  //
	  // This case is a trickier than the PUT case since we can't
	  // write outside of the bounds of the user provided buffer.
	  // We still align the buffers to 128-bytes for good performance
	  // in the middle portion of the xfers.
	  //
	  ea_base = ROUND_DN(eaa->arg[i].ea_addr, (gc_eaddr_t) CACHE_LINE_SIZE);
	  offset = eaa->arg[i].ea_addr & (CACHE_LINE_SIZE-1);

	  uint32_t ls_alloc_len =
	    ROUND_UP(eaa->arg[i].put_size + offset, CACHE_LINE_SIZE);

	  total_put_alloc += ls_alloc_len;

	  if (total_put_alloc > GC_SPU_BUFSIZE){
	    jd->status = JS_ARGS_TOO_LONG;
	    goto wrap_up;
	  }

	  ls_base = put_t;
	  put_t += ls_alloc_len;
	  eaa->arg[i].ls_addr = ls_base + offset;

	  if (1){
	    assert((mfc_ea2l(eaa->arg[i].ea_addr) & 0x7f)
		   == ((intptr_t)eaa->arg[i].ls_addr & 0x7f));
	    assert((ea_base & 0x7f) == 0);
	    assert(((intptr_t)ls_base & 0x7f) == 0);
	  }
	}

	else
	  assert(0);
      }

      // fire off the dma to fetch the args and wait for it to complete
      mfc_getl(get_base, common_ea, dma_get_list, gli*sizeof(dma_get_list[0]), get_tag, 0, 0);
      mfc_write_tag_mask(1 << get_tag);		// the tag we're interested in
      mfc_read_tag_status_all();		// wait for DMA to complete

      // do the work
      (*gc_proc_def[jd->proc_id].proc)(&jd->input, &jd->output, &jd->eaa);


      // Do we have any "put" args?  If so copy them out
      if (jd->sys.direction_union & GCJD_DMA_PUT){

	// Do the copy out using single DMA xfers.  The LS ranges
	// aren't generally contiguous.
	
	bool started_dma = false;

	for (unsigned int i = 0; i < eaa->nargs; i++){
	  if (eaa->arg[i].direction == GCJD_DMA_PUT && eaa->arg[i].put_size != 0){
	    
	    started_dma = true;

	    gc_eaddr_t       ea;
	    unsigned char   *ls;
	    unsigned int     len;

	    ea = eaa->arg[i].ea_addr;
	    ls = (unsigned char *) eaa->arg[i].ls_addr;
	    len = eaa->arg[i].put_size;

	    if (len < 16)
	      handle_slow_and_tedious_dma(ea, ls, len, put_tag);
	    
	    else {
	      if ((ea & 0xf) != 0){

		// printf("1:  ea = 0x%x  len = %5d\n", (int) ea, len);
		
		// handle the "pre-multiple-of-16" portion
		// do 1, 2, 4, or 8 byte xfers as required

		if (ea & 0x1){				// do a 1-byte xfer
		  mfc_put(ls, ea, 1, put_tag, 0, 0);
		  ea += 1;
		  ls += 1;
		  len -= 1;
		}
		if (ea & 0x2){				// do a 2-byte xfer
		  mfc_put(ls, ea, 2, put_tag, 0, 0);
		  ea += 2;
		  ls += 2;
		  len -= 2;
		}
		if (ea & 0x4){				// do a 4-byte xfer
		  mfc_put(ls, ea, 4, put_tag, 0, 0);
		  ea += 4;
		  ls += 4;
		  len -= 4;
		}
		if (ea & 0x8){				// do an 8-byte xfer
		  mfc_put(ls, ea, 8, put_tag, 0, 0);
		  ea += 8;
		  ls += 8;
		  len -= 8;
		}
	      }

	      if (1){
		// printf("2:  ea = 0x%x  len = %5d\n", (int) ea, len);
		assert((ea & 0xf) == 0);
		assert((((intptr_t) ls) & 0xf) == 0);
	      }

	      // handle the "multiple-of-16" portion

	      int aligned_len = ROUND_DN(len, 16);
	      len = len & (16 - 1);

	      while (aligned_len != 0){
		int dma_len = MIN(aligned_len, MFC_MAX_DMA_SIZE);
		mfc_put(ls, ea, dma_len, put_tag, 0, 0);
		ea += dma_len;
		ls += dma_len;
		aligned_len -= dma_len;
	      }

	      if (1){
		// printf("3:  ea = 0x%x  len = %5d\n", (int)ea, len);
		assert((ea & 0xf) == 0);
		assert((((intptr_t) ls) & 0xf) == 0);
	      }

	      // handle "post-multiple-of-16" portion

	      if (len != 0){

		if (len >= 8){				// do an 8-byte xfer
		  mfc_put(ls, ea, 8, put_tag, 0, 0);
		  ea += 8;
		  ls += 8;
		  len -= 8;
		}
		if (len >= 4){				// do a 4-byte xfer
		  mfc_put(ls, ea, 4, put_tag, 0, 0);
		  ea += 4;
		  ls += 4;
		  len -= 4;
		}
		if (len >= 2){				// do a 2-byte xfer
		  mfc_put(ls, ea, 2, put_tag, 0, 0);
		  ea += 2;
		  ls += 2;
		  len -= 2;
		}
		if (len >= 1){				// do a 1-byte xfer
		  mfc_put(ls, ea, 1, put_tag, 0, 0);
		  ea += 1;
		  ls += 1;
		  len -= 1;
		}
		if (1)
		  assert(len == 0);
	      }
	    }
	  }
	}
	if (started_dma){
	  put_in_progress |= PBI_MASK(pb_idx);		// note it's running
	  gc_log_write2(GCL_SS_SYS, 0x27, put_in_progress, pb_idx);
	  pb_idx ^= 1;					// toggle current buffer
	}
      }
    }
  }

 wrap_up:;	// semicolon creates null statement for C99 compliance

  // Copy job descriptor back out to EA.
  // (The dma will be waited on in flush_completion_info)
  int tag = ci_tags + ci_idx;			// use the current completion tag
  mfc_put(jd, jd_ea, sizeof(*jd), tag, 0, 0);

  // Tell PPE we're done with the job.
  //
  // We queue these up until we run out of room, or until we can send
  // the info to the PPE w/o blocking.  The blocking check is in
  // main_loop

  comp_info.job_id[comp_info.ncomplete++] = jd->sys.job_id;

  if (comp_info.ncomplete == GC_CI_NJOBS){
    gc_log_write0(GCL_SS_SYS, 0x28);
    flush_completion_info();
  }
}

static void
main_loop(void)
{
  // events: 0x1X

  static gc_job_desc_t	jd;	// static gets us proper alignment
  gc_eaddr_t		jd_ea;
  int			total_jobs = 0;

#if (USE_LLR_LOST_EVENT)
  // setup events
  spu_writech(SPU_WrEventMask, MFC_LLR_LOST_EVENT);

  // prime the pump
  while (gc_jd_queue_dequeue(spu_args.queue, &jd_ea, ci_tags + ci_idx, &jd))
    process_job(jd_ea, &jd);
  // we're now holding a lock-line reservation
#endif

  while (1){

#if !CHECK_QUEUE_ON_MSG
#if (USE_LLR_LOST_EVENT)

    if (unlikely(spu_readchcnt(SPU_RdEventStat))){
      //
      // execute standard event handling prologue
      //
      int status = spu_readch(SPU_RdEventStat);
      int mask = spu_readch(SPU_RdEventMask);
      spu_writech(SPU_WrEventMask, mask & ~status);	// disable active events
      spu_writech(SPU_WrEventAck, status);		// ack active events

      // execute per-event actions

      if (status & MFC_LLR_LOST_EVENT){
	//
	// We've lost a line reservation.  This is most likely caused
	// by somebody doing something to the queue.  Go look and see
	// if there's anything for us.
	//
	while (gc_jd_queue_dequeue(spu_args.queue, &jd_ea, ci_tags + ci_idx, &jd) == GCQ_OK)
	  process_job(jd_ea, &jd);
      }

      //
      // execute standard event handling epilogue
      //
      spu_writech(SPU_WrEventMask, mask);	// restore event mask
    }

#else

    // try to get a job from the job queue 
    if (gc_jd_queue_dequeue(spu_args.queue, &jd_ea, ci_tags + ci_idx, &jd) == GCQ_OK){
      total_jobs++;
      gc_log_write2(GCL_SS_SYS, 0x10, jd.sys.job_id, total_jobs);

      process_job(jd_ea, &jd); 

      gc_log_write2(GCL_SS_SYS, 0x11, jd.sys.job_id, total_jobs);
      backoff_reset(); 
    }
    else
      backoff_delay();

#endif
#endif

    // any msgs for us?

    if (unlikely(spu_readchcnt(SPU_RdInMbox))){
      int msg = spu_readch(SPU_RdInMbox);
      // printf("spu[%d] mbox_msg: 0x%08x\n", spu_args.spu_idx, msg);
#if CHECK_QUEUE_ON_MSG
      if (MBOX_MSG_OP(msg) == OP_CHECK_QUEUE){

	while (1){
	  //int delay = (int)(3200.0 * gc_uniform_deviate());	// uniformly in [0, 1.0us]
	  //gc_cdelay(delay);

	  gc_dequeue_status_t s =
	    gc_jd_queue_dequeue(spu_args.queue, &jd_ea, ci_tags + ci_idx, &jd);

	  if (s == GCQ_OK){
	    total_jobs++;
	    gc_log_write2(GCL_SS_SYS, 0x10, jd.sys.job_id, total_jobs);

	    process_job(jd_ea, &jd); 

	    gc_log_write2(GCL_SS_SYS, 0x11, jd.sys.job_id, total_jobs);
	  }
	  else if (s == GCQ_EMPTY){
	    break;
	  }
	  else {	// GCQ_LOCKED -- keep trying
	  }
	}
      }
      else 
#endif
      if (MBOX_MSG_OP(msg) == OP_EXIT){
	flush_completion_info();
	return;
      }
      else if (MBOX_MSG_OP(msg) == OP_GET_SPU_BUFSIZE){
	spu_writech(SPU_WrOutIntrMbox, MK_MBOX_MSG(OP_SPU_BUFSIZE, GC_SPU_BUFSIZE_BASE));
      }
    }

    // If we've got job completion info for the PPE and we can send a
    // message without blocking, do it.

    if (comp_info.ncomplete != 0 && spu_readchcnt(OUT_MBOX_CHANNEL) != 0){
      gc_log_write0(GCL_SS_SYS, 0x12);
      flush_completion_info();
    }
  }
}


int
main(unsigned long long spe_id __attribute__((unused)),
     unsigned long long argp,
     unsigned long long envp __attribute__((unused)))
{
  gc_sys_tag = mfc_tag_reserve();	// allocate a tag for our misc DMA operations
  get_tag  = mfc_tag_reserve();
  ci_tags  = mfc_multi_tag_reserve(2);
  put_tags = mfc_multi_tag_reserve(2);

  // dma the args in
  mfc_get(&spu_args, argp, sizeof(spu_args), gc_sys_tag, 0, 0);
  mfc_write_tag_mask(1 << gc_sys_tag);	// the tag we're interested in
  mfc_read_tag_status_all();		// wait for DMA to complete

  // initialize pointer to procedure entry table
  gc_proc_def = (gc_proc_def_t *) spu_args.proc_def_ls_addr;

  gc_set_seed(spu_args.spu_idx);

  // initialize logging
  _gc_log_init(spu_args.log);

  backoff_init();		// initialize backoff parameters

  main_loop();
  return 0;
}
