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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "gc_job_manager.h"
#include "gc_job_manager_impl.h"
#include <boost/weak_ptr.hpp>
#include <stdio.h>


static boost::weak_ptr<gc_job_manager> s_singleton;


// custom deleter of gc_job_desc allocated via alloc_job_desc_sptr
class job_desc_deleter {
  gc_job_manager_sptr	d_mgr;
public:
  job_desc_deleter(gc_job_manager_sptr mgr) : d_mgr(mgr) {}

  void operator()(gc_job_desc *jd) {
    d_mgr->free_job_desc(jd);
  }
};



gc_job_manager_sptr
gc_make_job_manager(const gc_jm_options *options)
{
  return gc_job_manager_sptr(new gc_job_manager_impl(options));
}

gc_job_manager::gc_job_manager(const gc_jm_options *options)
{
  // nop
}

gc_job_manager::~gc_job_manager()
{
  // nop
}

void
gc_job_manager::set_debug(int debug)
{
  // nop
}

int
gc_job_manager::debug()
{
  return 0;
}

void 
gc_job_manager::set_singleton(gc_job_manager_sptr mgr)
{
  s_singleton = mgr;
}

gc_job_manager_sptr 
gc_job_manager::singleton()
{
  return gc_job_manager_sptr(s_singleton);
}

gc_job_desc_sptr 
gc_job_manager::make_jd_sptr(gc_job_manager_sptr mgr, gc_job_desc *jd)
{
  return gc_job_desc_sptr(jd, job_desc_deleter(mgr));
}

gc_job_desc_sptr 
gc_job_manager::alloc_job_desc(gc_job_manager_sptr mgr)
{
  return make_jd_sptr(mgr, mgr->alloc_job_desc());
}


// ------------------------------------------------------------------------


// custom deleter
class spe_program_handle_deleter {
public:
  void operator()(spe_program_handle_t *program) {
    if (program){
      int r = spe_image_close(program);
      if (r != 0){
	perror("spe_image_close");
      }
    }
  }
};

// nop custom deleter
class nop_spe_program_handle_deleter {
public:
  void operator()(spe_program_handle_t *program) {
  }
};

spe_program_handle_sptr 
gc_program_handle_from_filename(const std::string &filename)
{
  return spe_program_handle_sptr(spe_image_open(filename.c_str()),
				 spe_program_handle_deleter());
}


spe_program_handle_sptr 
gc_program_handle_from_address(spe_program_handle_t *handle)
{
  return spe_program_handle_sptr(handle, nop_spe_program_handle_deleter());
}

const std::string
gc_job_status_string(gc_job_status_t status)
{
  switch(status){
  case JS_OK:			return "JS_OK";
  case JS_SHUTTING_DOWN:	return "JS_SHUTTING_DOWN";
  case JS_TOO_MANY_CLIENTS:	return "JS_TOO_MANY_CLIENTS";
  case JS_UNKNOWN_PROC:		return "JS_UNKNOWN_PROC";
  case JS_BAD_DIRECTION:	return "JS_BAD_DIRECTION";
  case JS_BAD_EAH:		return "JS_BAD_EAH";
  case JS_BAD_N_DIRECT:		return "JS_BAD_N_DIRECT";
  case JS_BAD_N_EA:		return "JS_BAD_N_EA";
  case JS_ARGS_TOO_LONG:	return "JS_ARGS_TOO_LONG";
  case JS_BAD_JUJU:		return "JS_BAD_JUJU";
  case JS_BAD_JOB_DESC:		return "JS_BAD_JOB_DESC";
  default:
    char buf[100];
    snprintf(buf, sizeof(buf), "unknown gc_job_status_t (%d)\n", status);
    return buf;
  }
}

/*
 * exception classes
 */

gc_exception::gc_exception(const std::string &msg)
  : runtime_error(msg)
{
}

gc_unknown_proc::gc_unknown_proc(const std::string &msg)
  : gc_exception("gc_unknown_proc: " + msg)
{
}

gc_bad_alloc::gc_bad_alloc(const std::string &msg)
  : gc_exception("gc_bad_alloc: " + msg)
{
}

gc_bad_align::gc_bad_align(const std::string &msg)
  : gc_exception("gc_bad_align: " + msg)
{
}

gc_bad_submit::gc_bad_submit(const std::string &name, gc_job_status_t status)
  : gc_exception("gc_bad_submit(" + name + "): " + gc_job_status_string(status))
{
}
