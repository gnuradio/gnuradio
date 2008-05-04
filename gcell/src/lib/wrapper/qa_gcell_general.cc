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

#include "qa_gcell_general.h"
#include <cppunit/TestAssert.h>

#include <stdio.h>
#include <stdlib.h>	// random, posix_memalign
#include <algorithm>
#include <string.h>
#include <gc_job_manager.h>


// handle to embedded SPU executable
extern spe_program_handle_t gcell_general_qa;

gc_job_desc_sptr
gcp_qa_general_submit(gc_job_manager_sptr mgr, const std::string &proc_name)
{
  gc_proc_id_t proc_id = mgr->lookup_proc(proc_name);
  gc_job_desc_sptr jd = gc_job_manager::alloc_job_desc(mgr);

  jd->proc_id = proc_id;
  jd->input.nargs = 0;
  jd->output.nargs = 1;
  jd->eaa.nargs = 0;

  if (!mgr->submit_job(jd.get())){
    gc_job_status_t s = jd->status;
    throw gc_bad_submit(proc_name, s);
  }
  return jd;
}


bool
qa_gcell_general::generic_test_body(const std::string &proc_name)
{
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_general_qa);
  opts.nspes = 1;
  gc_job_manager_sptr mgr = gc_make_job_manager(&opts);

  gc_job_desc_sptr jd = gcp_qa_general_submit(mgr, proc_name);
  if (!mgr->wait_job(jd.get())){
    fprintf(stderr, "wait_job for %s failed: %s\n",
	    proc_name.c_str(),
	    gc_job_status_string(jd->status).c_str());
    CPPUNIT_ASSERT(0);
  }

  return jd->output.arg[0].u32;		// bool result from SPE code
}

/*
 * ------------------------------------------------------------------------
 *		    Add more calls to SPE QA code here...
 * ------------------------------------------------------------------------
 */
void
qa_gcell_general::test_memset()
{
  CPPUNIT_ASSERT(generic_test_body("qa_memset"));
}

