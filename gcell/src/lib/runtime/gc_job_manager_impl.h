/* -*- c++ -*- */
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

#ifndef INCLUDED_GC_JOB_MANAGER_IMPL_H
#define INCLUDED_GC_JOB_MANAGER_IMPL_H

#include "gc_job_manager.h"
#include "gc_client_thread_info.h"
#include "gc_jd_stack.h"
#include "gc_jd_queue.h"
#include "gc_spu_args.h"
#include <libspe2.h>
#include <vector>
#include <boost/scoped_array.hpp>

typedef boost::shared_ptr<spe_gang_context> spe_gang_context_sptr;
typedef boost::shared_ptr<spe_program_handle_t> spe_program_handle_sptr;
typedef boost::scoped_array<gc_client_thread_info> gc_client_thread_info_sa;


enum worker_state {
  WS_FREE,	// not in use
  WS_INIT,	// allocated and being initialized
  WS_RUNNING,	// the thread is running
  WS_DEAD,	// the thread is dead
};

struct worker_ctx {
  volatile worker_state	state;
  unsigned int		spe_idx;  	// [0, nspes-1]
  spe_context_ptr_t	spe_ctx;
  pthread_t		thread;
  gc_spu_args_t		*spu_args;	// pointer to 16-byte aligned struct

  worker_ctx()
    : state(WS_FREE), spe_idx(0), spe_ctx(0),
      thread(0), spu_args(0) {}
  ~worker_ctx();
};

enum evt_handler_state {
  EHS_INIT,		// being initialized
  EHS_RUNNING,		// thread is running
  EHS_SHUTTING_DOWN,	// in process of shutting down everything
  EHS_WAITING_FOR_WORKERS_TO_DIE,
  EHS_DEAD,		// thread is dead
};

struct spe_event_handler {
  spe_event_handler_ptr_t	ptr;

  spe_event_handler() : ptr(0) {}
  ~spe_event_handler(){
    if (ptr){
      if (spe_event_handler_destroy(ptr) != 0){
	perror("spe_event_handler_destroy");
      }
    }
  }
};


/*!
 * \brief Concrete class that manages SPE jobs.
 *
 * This class contains all the implementation details.
 */
class gc_job_manager_impl : public gc_job_manager
{
  enum { MAX_SPES =  16 };

  int			  d_debug;
  gc_jm_options		  d_options;
  spe_program_handle_sptr d_spe_image;
  spe_gang_context_sptr   d_gang;		// boost::shared_ptr

  worker_ctx 		 d_worker[MAX_SPES];	// SPE ctx, thread, etc
  gc_spu_args_t		*d_spu_args;		// 16-byte aligned structs
  boost::shared_ptr<void> _d_spu_args_boost;	// hack for automatic storage mgmt

  gc_comp_info_t	*d_comp_info;		// 128-byte aligned structs
  boost::shared_ptr<void> _d_comp_info_boost;	// hack for automatic storage mgmt

  // used to coordinate communication w/ the event handling thread
  omni_mutex		 d_eh_mutex;
  omni_condition	 d_eh_cond;
  pthread_t		 d_eh_thread;		// the event handler thread
  volatile evt_handler_state	d_eh_state;
  volatile bool		 	d_shutdown_requested;
  spe_event_handler	 d_spe_event_handler;
  

  // All of the job descriptors are hung off of here.
  // We allocate them all in a single cache aligned chunk.
  gc_job_desc_t		*d_jd;			// [options.max_jobs]
  boost::shared_ptr<void> _d_jd_boost;		// hack for automatic storage mgmt

  gc_client_thread_info_sa d_client_thread;	// [options.max_client_threads]

  // We use bitvectors to represent the completing state of a job.  Each
  // bitvector is d_bvlen longs in length.
  int			 d_bvlen;		// bit vector length in longs

  // This contains the storage for all the bitvectors used by the job
  // manager.  There's 1 for each client thread, in the d_jobs_done
  // field.  We allocate them all in a single cache aligned chunk.
  boost::shared_ptr<void> _d_all_bitvectors;	// hack for automatic storage mgmt

  // Lock free stack where we keep track of the free job descriptors.
  gc_jd_stack_t		*d_free_list;		// stack of free job descriptors
  boost::shared_ptr<void> _d_free_list_boost;	// hack for automatic storage mgmt

  // The PPE inserts jobs here; SPEs pull jobs from here.
  gc_jd_queue_t	 	*d_queue;		// job queue
  boost::shared_ptr<void> _d_queue_boost;	// hack for automatic storage mgmt

  int			 d_ea_args_maxsize;

  struct gc_proc_def	*d_proc_def;		// the SPE procedure table
  uint32_t		 d_proc_def_ls_addr;	// the LS address of the table
  int			 d_nproc_defs;		// number of proc_defs in table

  gc_client_thread_info *alloc_cti();
  void free_cti(gc_client_thread_info *cti);

  void create_event_handler();
  void set_eh_state(evt_handler_state s);
  void set_ea_args_maxsize(int maxsize);

  void notify_clients_jobs_are_done(unsigned int spe_num,
				    unsigned int completion_info_idx);

public:
  void event_handler_loop();	// really private

private:
  bool send_all_spes(uint32_t msg);
  bool send_spe(unsigned int spe, uint32_t msg);
  void print_event(spe_event_unit_t *evt);
  void handle_event(spe_event_unit_t *evt);

  // bitvector ops
  void bv_zero(unsigned long *bv);
  void bv_clr(unsigned long *bv, unsigned int bitno);
  void bv_set(unsigned long *bv, unsigned int bitno);
  bool bv_isset(unsigned long *bv, unsigned int bitno);
  bool bv_isclr(unsigned long *bv, unsigned int bitno);

  void setup_logfiles();
  void sync_logfiles();
  void unmap_logfiles();

  friend gc_job_manager_sptr gc_make_job_manager(const gc_jm_options *options);
  
  gc_job_manager_impl(const gc_jm_options *options = 0);

public:
  virtual ~gc_job_manager_impl();

  /*!
   * Stop accepting new jobs.  Wait for existing jobs to complete.
   * Return all managed SPE's to the system.
   */
  virtual bool shutdown();

  /*!
   * \brief Return number of SPE's currently allocated to job manager.
   */
  virtual int nspes() const;

  /*!
   * \brief Return a pointer to a properly aligned job descriptor,
   * or zero if none are available.
   */
  virtual gc_job_desc *alloc_job_desc();

  /*
   *! Return a job descriptor previously allocated with alloc_job_desc()
   *
   * \param[in] jd pointer to job descriptor to free.
   */
  virtual void free_job_desc(gc_job_desc *jd);

  /*!
   * \brief Submit a job for asynchronous processing on an SPE.
   *
   * \param[in] jd pointer to job description
   *
   * The caller must not read or write the job description
   * or any of the memory associated with any indirect arguments
   * until after calling wait_job.
   *
   * \returns true iff the job was successfully enqueued.
   * If submit_job returns false, check jd->status for additional info.
   */
  virtual bool submit_job(gc_job_desc *jd);

  /*!
   * \brief Wait for job to complete.
   *
   * A thread may only wait for jobs which it submitted.
   *
   * \returns true if sucessful, else false.
   */
  virtual bool 
  wait_job(gc_job_desc *jd);

  /*!
   * \brief wait for 1 or more jobs to complete.
   *
   * \param[input] njobs is the length of arrays \p jd and \p done.
   * \param[input] jd are the jobs that are to be waited for.
   * \param[output] done indicates whether the corresponding job is complete.
   * \param[input] mode indicates whether to wait for ALL or ANY of the jobs
   *   in \p jd to complete.
   *
   * A thread may only wait for jobs which it submitted.
   *
   * \returns number of jobs completed, or -1 if error.
   */
  virtual int
  wait_jobs(unsigned int njobs,
	    gc_job_desc *jd[], bool done[], gc_wait_mode mode);

  virtual int ea_args_maxsize();

  virtual gc_proc_id_t lookup_proc(const std::string &name);
  virtual std::vector<std::string> proc_names();

  virtual void set_debug(int debug);
  virtual int debug();
};

#endif /* INCLUDED_GC_JOB_MANAGER_IMPL_H */
