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
#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <stdexcept>
#include <libspe2.h>
#include "gc_job_desc.h"

class gc_job_manager;
typedef boost::shared_ptr<gc_job_manager> gc_job_manager_sptr;
typedef boost::shared_ptr<spe_program_handle_t> spe_program_handle_sptr;
typedef boost::shared_ptr<gc_job_desc> gc_job_desc_sptr;

/*!
 * \brief Return a boost::shared_ptr to an spe_program_handle_t
 *
 * \param filename is the name of the SPE ELF executable to open.
 *
 * Calls spe_image_open to open the file.  If successful returns a
 * boost::shared_ptr that will call spe_image_close when it's time to
 * free the object.
 *
 * Returns the equivalent of the NULL pointer if the file cannot be
 * opened, or if it's not an SPE ELF object file.
 *
 * \sa gc_program_handle_from_address
 */
spe_program_handle_sptr 
gc_program_handle_from_filename(const std::string &filename);

/*!
 * \brief Return a boost::shared_ptr to an spe_program_handle_t
 *
 * \param handle is a non-zero pointer to an embedded SPE image.
 *
 * If successful returns a boost::shared_ptr that does nothing when
 * it's time to free the object.
 *
 * \sa gc_program_handle_from_filename
 */
spe_program_handle_sptr 
gc_program_handle_from_address(spe_program_handle_t *handle);

/*!
 * \brief map gc_job_status_t into a string
 */
const std::string
gc_job_status_string(gc_job_status_t status);

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
  uint32_t log2_nlog_entries;    	   // log2 of number of log entries (default is 12 == 4k)
  spe_program_handle_sptr program_handle;  // program to load into SPEs

  gc_jm_options() :
    max_jobs(0), max_client_threads(0), nspes(0),
    gang_schedule(false), use_affinity(false),
    enable_logging(false), log2_nlog_entries(12)
  {
  }

  gc_jm_options(spe_program_handle_sptr program_handle_,
		unsigned int nspes_ = 0) :
    max_jobs(0), max_client_threads(0), nspes(nspes_),
    gang_schedule(false), use_affinity(false),
    enable_logging(false), log2_nlog_entries(12),
    program_handle(program_handle_)
  {
  }
};

enum gc_wait_mode {
  GC_WAIT_ANY,
  GC_WAIT_ALL,
};

/*
 * exception classes
 */
class gc_exception : public std::runtime_error
{
public:
  gc_exception(const std::string &msg);
};

class gc_unknown_proc : public gc_exception
{
public:
  gc_unknown_proc(const std::string &msg);
};

class gc_bad_alloc : public gc_exception
{
public:
  gc_bad_alloc(const std::string &msg);
};

class gc_bad_align : public gc_exception
{
public:
  gc_bad_align(const std::string &msg);
};

class gc_bad_submit : public gc_exception
{
public:
  gc_bad_submit(const std::string &name, gc_job_status_t status);
};

/*
 * \brief Create an instance of the job manager
 */
gc_job_manager_sptr
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
   * or throws gc_bad_alloc if there are none available.
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
   * The caller must examine the status field of each job to confirm
   * successful completion of the job.
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
   * exists, otherwise throws gc_unknown_proc.
   */
  virtual gc_proc_id_t lookup_proc(const std::string &proc_name) = 0;
  
  /*!
   * Return a vector of all known spu procedure names.
   */
  virtual std::vector<std::string> proc_names() = 0;

  virtual void set_debug(int debug);
  virtual int debug();

  /* ----- static methods ----- */

  /*!
   * \brief Set the singleton gc_job_manager instance.
   * \param mgr is the job manager instance.
   *
   * The singleton is weakly held, thus the caller must maintain
   * a reference to the mgr for the duration.  (If we held the
   * manager strongly, the destructor would never be called, and the
   * resources (SPEs) would not be returned.)  Bottom line: the
   * caller is responsible for life-time management.
   */
  static void set_singleton(gc_job_manager_sptr mgr);

  /*!
   * \brief Retrieve the singleton gc_job_manager instance.
   *
   * Returns the singleton gc_job_manager instance or raises
   * boost::bad_weak_ptr if the singleton is empty.
   */
  static gc_job_manager_sptr singleton();

  /*!
   * \brief return a boost::shared_ptr to a job descriptor.
   */
  static gc_job_desc_sptr make_jd_sptr(gc_job_manager_sptr mgr, gc_job_desc *jd);

  /*!
   * \brief allocate a job descriptor and return a boost::shared_ptr to it.
   */
  static gc_job_desc_sptr alloc_job_desc(gc_job_manager_sptr mgr);
};


#endif /* INCLUDED_GC_JOB_MANAGER_H */
