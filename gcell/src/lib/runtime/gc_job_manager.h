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

#ifndef INCLUDED_GC_JOB_MANAGER_H
#define INCLUDED_GC_JOB_MANAGER_H

#include <boost/utility.hpp>
#include <vector>
#include <string>
#include <libspe2.h>
#include "gc_job_desc.h"

class gc_job_manager;

enum gc_wait_mode {
  GC_WAIT_ANY,
  GC_WAIT_ALL,
};

/*
 * \brief Options that configure the job_manager.
 * The default values are reasonable.
 */
struct gc_jm_options {
  unsigned int max_jobs;	    // max # of job descriptors in system
  unsigned int max_client_threads;  // max # of client threads of job manager
  unsigned int nspes;		    // how many SPEs shall we use? 0 -> all of them
  bool gang_schedule;		    // shall we gang schedule?
  bool use_affinity;		    // shall we try for affinity (FIXME not implmented)
  bool enable_logging;		    // shall we log SPE events?
  uint32_t log2_nlog_entries;    	 // log2 of number of log entries (default is 12 == 4k)
  spe_program_handle_t *program_handle;  // program to load into SPEs

  gc_jm_options() :
    max_jobs(0), max_client_threads(0), nspes(0),
    gang_schedule(true), use_affinity(false),
    enable_logging(false), log2_nlog_entries(12),
    program_handle(0)
  {
  }
};


/*
 * \brief Create an instance of the job manager
 */
gc_job_manager *
gc_make_job_manager(const gc_jm_options *options = 0);


/*!
 * \brief Abstract class that manages SPE jobs.
 *
 * There is typically a single instance derived from this class.
 * It is safe to call its methods from any thread.
 */
class gc_job_manager : boost::noncopyable
{
public:
  gc_job_manager(const gc_jm_options *options = 0); 

  virtual ~gc_job_manager();

  /*!
   * Stop accepting new jobs.  Wait for existing jobs to complete.
   * Return all managed SPE's to the system.
   */
  virtual bool shutdown() = 0;

  /*!
   * \brief Return number of SPE's currently allocated to job manager.
   */
  virtual int nspes() const = 0;

  /*!
   * \brief Return a pointer to a properly aligned job descriptor,
   * or zero if none are available.
   */
  virtual gc_job_desc *alloc_job_desc() = 0;

  /*
   *! Free a job descriptor previously allocated with alloc_job_desc()
   *
   * \param[in] jd pointer to job descriptor to free.
   */
  virtual void free_job_desc(gc_job_desc *jd) = 0;

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
  virtual bool submit_job(gc_job_desc *jd) = 0;

  /*!
   * \brief Wait for job to complete.
   *
   * A thread may only wait for jobs which it submitted.
   *
   * \returns true if sucessful, else false.
   */
  virtual bool 
  wait_job(gc_job_desc *jd) = 0;

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
	    gc_job_desc *jd[], bool done[], gc_wait_mode mode) = 0;

  /*!
   * Return the maximum number of bytes of EA arguments that may be
   * copied to or from the SPE in a single job.  The limit applies
   * independently to the "get" and "put" args.  
   * \sa gc_job_desc_t, gc_job_ea_args_t
   */
  virtual int ea_args_maxsize() = 0;

  /*!
   * Return gc_proc_id_t associated with spu procedure \p proc_name if one
   * exists, otherwise return GCP_UNKNOWN_PROC.
   */
  virtual gc_proc_id_t lookup_proc(const std::string &proc_name) = 0;
  
  /*!
   * Return a vector of all known spu procedure names.
   */
  virtual std::vector<std::string> proc_names() = 0;

  virtual void set_debug(int debug);
  virtual int debug();
};


#endif /* INCLUDED_GC_JOB_MANAGER_H */
