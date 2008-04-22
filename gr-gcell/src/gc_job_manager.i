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
    
struct spe_program_handle_t;
typedef boost::shared_ptr<spe_program_handle_t> spe_program_handle_sptr;
%template(spe_program_handle_sptr) boost::shared_ptr<spe_program_handle_t>;

class gc_job_manager;
typedef boost::shared_ptr<gc_job_manager> gc_job_manager_sptr;
%template(gc_job_manager_sptr) boost::shared_ptr<gc_job_manager>;


%rename(program_handle_from_filename) gc_program_handle_from_filename;
spe_program_handle_sptr 
gc_program_handle_from_filename(const std::string &filename);

%rename(program_handle_from_address) gc_program_handle_from_address;
spe_program_handle_sptr 
gc_program_handle_from_address(spe_program_handle_t *handle);


%rename(jm_options) gc_jm_options;
struct gc_jm_options {
  unsigned int max_jobs;	    // max # of job descriptors in system
  unsigned int max_client_threads;  // max # of client threads of job manager
  unsigned int nspes;		    // how many SPEs shall we use? 0 -> all of them
  bool gang_schedule;		    // shall we gang schedule?
  bool use_affinity;		    // shall we try for affinity (FIXME not implmented)
  bool enable_logging;		    // shall we log SPE events?
  uint32_t log2_nlog_entries;    	   // log2 of number of log entries (default is 12 == 4k)
  spe_program_handle_sptr program_handle;  // program to load into SPEs

  gc_jm_options(spe_program_handle_sptr program_handle_,
		unsigned int nspes_ = 0) :
    max_jobs(0), max_client_threads(0), nspes(nspes_),
    gang_schedule(false), use_affinity(false),
    enable_logging(false), log2_nlog_entries(12),
    program_handle(program_handle_)
  {
  }
};

%rename(job_manager) gc_make_job_manager;
gc_job_manager_sptr
gc_make_job_manager(const gc_jm_options *options);

%inline {
  void set_singleton(gc_job_manager_sptr mgr)
  {
    gc_job_manager::set_singleton(mgr);
  }

  gc_job_manager_sptr singleton()
  {
    return gc_job_manager::singleton();
  }
}
