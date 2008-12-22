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

#include "qa_job_manager.h"
#include <cppunit/TestAssert.h>
#include <gcell/gc_job_manager.h>
#include <stdexcept>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include <malloc.h>

// handle to embedded SPU executable w/ QA routines
extern spe_program_handle_t gcell_runtime_qa_spx;

#if 0
static void
gc_msleep(unsigned long millisecs)
{
  int r;
  struct timespec tv;
  tv.tv_sec = millisecs / 1000;
  tv.tv_nsec = (millisecs - (tv.tv_sec * 1000)) * 1000000;
  
  while (1){
    r = nanosleep(&tv, &tv);
    if (r == 0)
      return;
    if (r == -1 && errno == EINTR)
      continue;
    perror("nanosleep");
    return;
  }
}
#endif

void
qa_job_manager::leak_check(test_t t, const std::string &name)
{
  struct mallinfo before, after;

  before = mallinfo();
  (this->*t)();
  after = mallinfo();

  size_t delta = after.uordblks - before.uordblks;
  if (delta != 0){
    std::cout << name << " leaked memory\n";
    printf("  before.uordblks = %6d\n", before.uordblks);
    printf("  after.uordblks  = %6d\n",  after.uordblks);
    printf("  delta = %d\n", after.uordblks - before.uordblks);
  }
}

void
qa_job_manager::t0()
{
  //leak_check(&qa_job_manager::t1_body, "t1-0");
}

void
qa_job_manager::t1()
{
  t1_body();		// leaks 800 bytes first time, could be one-time inits
  leak_check(&qa_job_manager::t1_body, "t1");
}

void
qa_job_manager::t2()
{
  leak_check(&qa_job_manager::t2_body, "t2");
}

void
qa_job_manager::t3()
{
  t3_body();		// leaks first time only, could be cppunit
  leak_check(&qa_job_manager::t3_body, "t3");
}

void
qa_job_manager::t4()
{
  leak_check(&qa_job_manager::t4_body, "t4");
}

void
qa_job_manager::t5()
{
  leak_check(&qa_job_manager::t5_body, "t5");
}

void
qa_job_manager::t6()
{
  leak_check(&qa_job_manager::t6_body, "t6");
}

void
qa_job_manager::t7()
{
  leak_check(&qa_job_manager::t7_body, "t7");
}

void
qa_job_manager::t8()
{
  leak_check(&qa_job_manager::t8_body, "t8");
}

void
qa_job_manager::t9()
{
  leak_check(&qa_job_manager::t9_body, "t9");
}

void
qa_job_manager::t10()
{
  leak_check(&qa_job_manager::t10_body, "t10");
}

void
qa_job_manager::t11()
{
  leak_check(&qa_job_manager::t11_body, "t11");
}

void
qa_job_manager::t12()
{
  leak_check(&qa_job_manager::t12_body, "t12");
}

void
qa_job_manager::t13()
{
  leak_check(&qa_job_manager::t13_body, "t13");
}

void
qa_job_manager::t14()
{
  leak_check(&qa_job_manager::t14_body, "t14");
}

void
qa_job_manager::t15()
{
  leak_check(&qa_job_manager::t15_body, "t15");
}

// ----------------------------------------------------------------

void
qa_job_manager::t1_body()
{
  gc_job_manager_sptr mgr;
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_runtime_qa_spx);
  mgr = gc_make_job_manager(&opts);
}

void
qa_job_manager::t2_body()
{
  gc_job_manager_sptr mgr;
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_runtime_qa_spx);
  opts.nspes = 100;
  opts.gang_schedule = false;
  mgr = gc_make_job_manager(&opts);
}

void
qa_job_manager::t3_body()
{
  // This leaks memory the first time it's invoked, but I'm not sure
  // if it's us or the underlying exception handling mechanism, or
  // cppunit.  cppunit is the prime suspect.

#if 0
  gc_job_manager_sptr mgr;
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_runtime_qa_spx);
  opts.nspes = 100;
  opts.gang_schedule = true;
  CPPUNIT_ASSERT_THROW(mgr = gc_make_job_manager(&opts), std::out_of_range);
#endif
}

static void
init_jd(gc_job_desc *jd, gc_proc_id_t proc_id)
{
  jd->proc_id = proc_id;
  jd->input.nargs = 0;
  jd->output.nargs = 0;
  jd->eaa.nargs = 0;
}

void
qa_job_manager::t4_body()
{
  gc_job_manager_sptr mgr;
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_runtime_qa_spx);
  opts.nspes = 1;
  mgr = gc_make_job_manager(&opts);
  //mgr->set_debug(-1);
  static const int NJOBS = 32;
  gc_job_desc *jds[NJOBS];
  bool done[NJOBS];

  gc_proc_id_t gcp_no_such;
  CPPUNIT_ASSERT_THROW(gcp_no_such = mgr->lookup_proc("--no-such-proc-name--"), gc_unknown_proc);

  gc_proc_id_t gcp_qa_nop = mgr->lookup_proc("qa_nop");
  CPPUNIT_ASSERT(gcp_qa_nop != GCP_UNKNOWN_PROC);

  for (int i = 0; i < NJOBS; i++){
    jds[i] = mgr->alloc_job_desc();
    init_jd(jds[i], gcp_qa_nop);
  }

  for (int i = 0; i < NJOBS; i++){
    if (!mgr->submit_job(jds[i])){
      printf("%d: submit_job(jds[%d]) failed, status = %d\n",
	     __LINE__, i, jds[i]->status);
    }
  }

  int n = mgr->wait_jobs(NJOBS, jds, done, GC_WAIT_ALL);
  CPPUNIT_ASSERT_EQUAL(NJOBS, n);

  for (int i = 0; i < NJOBS; i++){
    mgr->free_job_desc(jds[i]);
  }
}

void
qa_job_manager::t5_body()
{
  gc_job_manager_sptr mgr;
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_runtime_qa_spx);
  opts.nspes = 0;	// use them all
  mgr = gc_make_job_manager(&opts);
  //mgr->set_debug(-1);
  static const int NJOBS = 32;
  gc_job_desc *jds[NJOBS];
  bool done[NJOBS];

  gc_proc_id_t gcp_qa_nop = mgr->lookup_proc("qa_nop");

  for (int i = 0; i < NJOBS; i++){
    jds[i] = mgr->alloc_job_desc();
    init_jd(jds[i], gcp_qa_nop);
  }

  for (int i = 0; i < NJOBS; i++){
    if (!mgr->submit_job(jds[i])){
      printf("%d: submit_job(jds[%d]) failed, status = %d\n",
	     __LINE__, i, jds[i]->status);
    }
  }

  int n = mgr->wait_jobs(NJOBS, jds, done, GC_WAIT_ALL);
  CPPUNIT_ASSERT_EQUAL(NJOBS, n);

  for (int i = 0; i < NJOBS; i++){
    mgr->free_job_desc(jds[i]);
  }
}

void
qa_job_manager::t6_body()
{
  gc_job_manager_sptr mgr;
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_runtime_qa_spx);
  opts.nspes = 1;	
  mgr = gc_make_job_manager(&opts);
  gc_proc_id_t gcp_qa_nop = mgr->lookup_proc("qa_nop");
  gc_job_desc *jd = mgr->alloc_job_desc();

  
  // test for success with gcp_qa_nop procedure
  init_jd(jd, gcp_qa_nop);
  if (!mgr->submit_job(jd)){
    printf("%d: submit_job(jd) failed, status = %d\n", __LINE__, jd->status);
  }
  else {
    mgr->wait_job(jd);
    CPPUNIT_ASSERT_EQUAL(JS_OK, jd->status);
  }

  // test for JS_UNKNOWN_PROC with bogus procedure
  init_jd(jd, -2);
  if (!mgr->submit_job(jd)){
    printf("%d: submit_job(jd) failed, status = %d\n", __LINE__, jd->status);
  }
  else {
    mgr->wait_job(jd);
    CPPUNIT_ASSERT_EQUAL(JS_UNKNOWN_PROC, jd->status);
  }

  mgr->free_job_desc(jd);
}

static int
sum_shorts(short *p, int nshorts)
{
  int total = 0;
  for (int i = 0; i < nshorts; i++)
    total += p[i];

  return total;
}

static void
test_sum_shorts(gc_job_manager_sptr mgr, short *buf, int nshorts)
{
  gc_job_desc *jd = mgr->alloc_job_desc();
  gc_proc_id_t gcp_qa_sum_shorts = mgr->lookup_proc("qa_sum_shorts");

  init_jd(jd, gcp_qa_sum_shorts);
  jd->eaa.nargs = 1;
  jd->eaa.arg[0].ea_addr = ptr_to_ea(buf);
  jd->eaa.arg[0].direction = GCJD_DMA_GET;
  jd->eaa.arg[0].get_size = nshorts * sizeof(short);
  

  if (!mgr->submit_job(jd)){
    printf("%d: submit_job(jd) failed, status = %d\n", __LINE__, jd->status);
  }
  else {
    mgr->wait_job(jd);
    CPPUNIT_ASSERT_EQUAL(JS_OK, jd->status);
    int expected = sum_shorts(buf, nshorts);
    int actual = jd->output.arg[0].s32;
    CPPUNIT_ASSERT_EQUAL(expected, actual);
  }

  mgr->free_job_desc(jd);
}

static const int NS = 32768;
static short short_buf[NS] _AL128;	// for known alignment

//
// test all "get" alignments and sizes
//
void
qa_job_manager::t7_body()
{
  gc_job_manager_sptr mgr;
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_runtime_qa_spx);
  opts.nspes = 1;
  mgr = gc_make_job_manager(&opts);

  int ea_args_maxsize = mgr->ea_args_maxsize();

  for (int i = 0; i < NS; i++)	// init buffer with known qty
    short_buf[i] = 0x1234 + i;
  
  for (int offset = 0; offset <= 128; offset++){
    for (int len = 0; len <= 128; len++){
      test_sum_shorts(mgr, &short_buf[offset], len);
    }
  }

  // confirm maximum length
  for (int offset = 0; offset <= 64; offset++){
    test_sum_shorts(mgr, &short_buf[offset], ea_args_maxsize/sizeof(short));
  }
}

//
// test "get" args too long
//
void
qa_job_manager::t8_body()
{
  gc_job_manager_sptr mgr;
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_runtime_qa_spx);
  opts.nspes = 1;
  mgr = gc_make_job_manager(&opts);
  gc_job_desc *jd = mgr->alloc_job_desc();
  gc_proc_id_t gcp_qa_sum_shorts = mgr->lookup_proc("qa_sum_shorts");

  init_jd(jd, gcp_qa_sum_shorts);
  jd->eaa.nargs = 1;
  jd->eaa.arg[0].ea_addr = 0;
  jd->eaa.arg[0].direction = GCJD_DMA_GET;
  jd->eaa.arg[0].get_size = 1 << 20;

  if (!mgr->submit_job(jd)){
    printf("%d: submit_job(jd) failed, status = %d\n", __LINE__, jd->status);
  }
  else {
    mgr->wait_job(jd);
    CPPUNIT_ASSERT_EQUAL(JS_ARGS_TOO_LONG, jd->status);
  }

  mgr->free_job_desc(jd);
}

//
// test MAX_ARGS_EA "get" case
//
void
qa_job_manager::t9_body()
{
  static const int N = 127;
  static const int M = 201;
  gc_job_manager_sptr mgr;
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_runtime_qa_spx);
  opts.nspes = 1;
  mgr = gc_make_job_manager(&opts);
  gc_job_desc *jd = mgr->alloc_job_desc();
  gc_proc_id_t gcp_qa_sum_shorts = mgr->lookup_proc("qa_sum_shorts");

  init_jd(jd, gcp_qa_sum_shorts);
  jd->eaa.nargs = MAX_ARGS_EA;
  for (int i = 0; i < MAX_ARGS_EA; i++){
    jd->eaa.arg[i].direction = GCJD_DMA_GET;
    jd->eaa.arg[i].ea_addr = ptr_to_ea(&short_buf[i * M]);
    jd->eaa.arg[i].get_size = N * sizeof(short);
  }

  if (!mgr->submit_job(jd)){
    printf("%d: submit_job(jd) failed, status = %d\n", __LINE__, jd->status);
  }
  else {
    mgr->wait_job(jd);
    CPPUNIT_ASSERT_EQUAL(JS_OK, jd->status);
    for (int i = 0; i < MAX_ARGS_EA; i++){
      int expected = sum_shorts(&short_buf[i * M], N);
      int actual = jd->output.arg[i].s32;
      CPPUNIT_ASSERT_EQUAL(expected, actual);
    }
  }

  mgr->free_job_desc(jd);
}

static bool
confirm_const(const unsigned char *buf, size_t len, unsigned char v)
{
  bool ok = true;

  for (size_t i = 0; i < len; i++){
    if (buf[i] != v){
      ok = false;
      printf("confirm_const: buf[%6d] = 0x%02x, expected = 0x%02x\n",
	     i, buf[i], v);
    }
  }

  return ok;
}

static bool
confirm_seq(const unsigned char *buf, size_t len, unsigned char v)
{
  bool ok = true;

  for (size_t i = 0; i < len; i++, v++){
    if (buf[i] != v){
      ok = false;
      printf("confirm_seq: buf[%6d] = 0x%02x, expected = 0x%02x\n",
	     i, buf[i], v);
    }
  }

  return ok;
}

static void
test_put_seq(gc_job_manager_sptr mgr, int offset, int len, int starting_val)
{
  gc_job_desc *jd = mgr->alloc_job_desc();
  gc_proc_id_t gcp_qa_put_seq = mgr->lookup_proc("qa_put_seq");

  unsigned char *buf = (unsigned char *) short_buf;
  size_t buf_len = sizeof(short_buf);
  memset(buf, 0xff, buf_len);

  // two cache lines into the buffer, so we can check before and after
  int fixed_offset = 256;

  init_jd(jd, gcp_qa_put_seq);
  jd->input.nargs = 1;
  jd->input.arg[0].s32 = starting_val;
  jd->eaa.nargs = 1;
  jd->eaa.arg[0].ea_addr = ptr_to_ea(buf + fixed_offset + offset);
  jd->eaa.arg[0].direction = GCJD_DMA_PUT;
  jd->eaa.arg[0].put_size = len;

  if (!mgr->submit_job(jd)){
    printf("%d: submit_job(jd) failed, status = %d\n", __LINE__, jd->status);
  }
  else {
    mgr->wait_job(jd);
    CPPUNIT_ASSERT_EQUAL(JS_OK, jd->status);
    
    // check before
    CPPUNIT_ASSERT(confirm_const(&buf[0], fixed_offset + offset, 0xff)); 

    // check sequence
    CPPUNIT_ASSERT(confirm_seq(&buf[fixed_offset + offset], len, starting_val));

    // check after
    CPPUNIT_ASSERT(confirm_const(&buf[fixed_offset + offset + len],
				 buf_len - fixed_offset - offset - len, 0xff));
  }
  mgr->free_job_desc(jd);
}

//
// Test all "put" alignments and sizes
//
void
qa_job_manager::t10_body()
{
  gc_job_manager_sptr mgr;
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_runtime_qa_spx);
  opts.nspes = 1;
  mgr = gc_make_job_manager(&opts);

  int starting_val = 13;

  for (int offset = 0; offset <= 128; offset++){
    for (int len = 0; len <= 128; len++){
      test_put_seq(mgr, offset, len, starting_val++);
    }
  }

  int ea_args_maxsize = mgr->ea_args_maxsize();

  // confirm maximum length
  for (int offset = 0; offset <= 64; offset++){
    test_put_seq(mgr, offset, ea_args_maxsize, starting_val++);
  }
}

//
// test "put" args too long
//
void
qa_job_manager::t11_body()
{
  gc_job_manager_sptr mgr;
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_runtime_qa_spx);
  opts.nspes = 1;
  mgr = gc_make_job_manager(&opts);
  gc_job_desc *jd = mgr->alloc_job_desc();
  gc_proc_id_t gcp_qa_put_seq = mgr->lookup_proc("qa_put_seq");

  init_jd(jd, gcp_qa_put_seq);
  jd->input.nargs = 1;
  jd->input.arg[0].s32 = 0;
  jd->eaa.nargs = 1;
  jd->eaa.arg[0].ea_addr = 0;
  jd->eaa.arg[0].direction = GCJD_DMA_PUT;
  jd->eaa.arg[0].put_size = 1 << 20;

  if (!mgr->submit_job(jd)){
    printf("%d: submit_job(jd) failed, status = %d\n", __LINE__, jd->status);
  }
  else {
    mgr->wait_job(jd);
    CPPUNIT_ASSERT_EQUAL(JS_ARGS_TOO_LONG, jd->status);
  }

  mgr->free_job_desc(jd);
}

//
// test MAX_ARGS_EA "put" case
//
void
qa_job_manager::t12_body()
{
  static const int N = 127;
  static const int M = 201;
  gc_job_manager_sptr mgr;
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_runtime_qa_spx);
  opts.nspes = 1;
  mgr = gc_make_job_manager(&opts);
  gc_job_desc *jd = mgr->alloc_job_desc();
  gc_proc_id_t gcp_qa_put_seq = mgr->lookup_proc("qa_put_seq");

  unsigned char *buf = (unsigned char *) short_buf;
  size_t buf_len = sizeof(short_buf);
  memset(buf, 0xff, buf_len);

  // two cache lines into the buffer, so we can check before and after
  int fixed_offset = 256;

  int starting_val = 13;

  init_jd(jd, gcp_qa_put_seq);
  jd->input.nargs = 1;
  jd->input.arg[0].s32 = starting_val;
  jd->eaa.nargs = MAX_ARGS_EA;
  for (int i = 0; i < MAX_ARGS_EA; i++){
    jd->eaa.arg[i].direction = GCJD_DMA_PUT;
    jd->eaa.arg[i].ea_addr = ptr_to_ea(&buf[i * M + fixed_offset]);
    jd->eaa.arg[i].put_size = N;
  }

  if (!mgr->submit_job(jd)){
    printf("%d: submit_job(jd) failed, status = %d\n", __LINE__, jd->status);
  }
  else {
    mgr->wait_job(jd);
    CPPUNIT_ASSERT_EQUAL(JS_OK, jd->status);
    for (int i = 0; i < MAX_ARGS_EA; i++){
      CPPUNIT_ASSERT(confirm_seq(&buf[i * M + fixed_offset], N, starting_val));
      starting_val += N;
    }
  }

  mgr->free_job_desc(jd);
}

//
// test qa_copy primitive
//
void
qa_job_manager::t13_body()
{
  gc_job_manager_sptr mgr;
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_runtime_qa_spx);
  opts.nspes = 1;
  mgr = gc_make_job_manager(&opts);

  memset(short_buf, 0, sizeof(short_buf));
  for (int i = 0; i < NS/2; i++)	// init buffer with known qty
    short_buf[i] = 0x1234 + i;

  int nshorts = NS/2;

  gc_job_desc *jd = mgr->alloc_job_desc();
  gc_proc_id_t gcp_qa_copy = mgr->lookup_proc("qa_copy");

#if 0
  printf("gcq_qa_copy = %d\n", gcp_qa_copy);
  std::vector<std::string> procs = mgr->proc_names();
  for (unsigned int i = 0; i < procs.size(); ++i)
    std::cout << procs[i] << std::endl;
#endif

  init_jd(jd, gcp_qa_copy);
  jd->eaa.nargs = 2;
  jd->eaa.arg[0].ea_addr = ptr_to_ea(&short_buf[nshorts]);
  jd->eaa.arg[0].direction = GCJD_DMA_PUT;
  jd->eaa.arg[0].put_size = nshorts * sizeof(short);
  
  jd->eaa.arg[1].ea_addr = ptr_to_ea(&short_buf[0]);
  jd->eaa.arg[1].direction = GCJD_DMA_GET;
  jd->eaa.arg[1].get_size = nshorts * sizeof(short);
  

  if (!mgr->submit_job(jd)){
    printf("%d: submit_job(jd) failed, status = %d\n", __LINE__, jd->status);
  }
  else {
    mgr->wait_job(jd);
    CPPUNIT_ASSERT_EQUAL(JS_OK, jd->status);
    CPPUNIT_ASSERT_EQUAL(0, jd->output.arg[0].s32);

    bool ok = true;
    for (int i = 0; i < nshorts; i++){
      if (short_buf[i] != short_buf[i + nshorts])
	ok = false;
    }
    CPPUNIT_ASSERT(ok);
  }
  mgr->free_job_desc(jd);
}

/*
 * Parallel submission of NJOBS "put" jobs will test double buffered puts.
 */
void
qa_job_manager::t14_body()
{
  //return;

  //static const int NJOBS = 64;
  static const int NJOBS = 128;
  static const int LEN_PER_JOB = 1021;
  unsigned char    buf[NJOBS * LEN_PER_JOB];
  gc_job_desc_t	  *jd[NJOBS];
  bool		   done[NJOBS];

  static const int STARTING_VAL = 13;

  memset(buf, 0xff, LEN_PER_JOB * NJOBS);

  gc_job_manager_sptr mgr;
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_runtime_qa_spx);
  opts.nspes = 1;
  mgr = gc_make_job_manager(&opts);


  gc_proc_id_t gcp_qa_put_seq = mgr->lookup_proc("qa_put_seq");

  // do all the initialization up front

  for (int i = 0, val = STARTING_VAL; i < NJOBS; i++, val += 3){
    jd[i] = mgr->alloc_job_desc();
    init_jd(jd[i], gcp_qa_put_seq);
    jd[i]->input.nargs = 1;
    jd[i]->input.arg[0].s32 = val;
    jd[i]->eaa.nargs = 1;
    jd[i]->eaa.arg[0].ea_addr = ptr_to_ea(&buf[i * LEN_PER_JOB]);
    jd[i]->eaa.arg[0].direction = GCJD_DMA_PUT;
    jd[i]->eaa.arg[0].put_size = LEN_PER_JOB;
  }

  // submit them all

  for (int i = 0; i < NJOBS; i++){
    if (!mgr->submit_job(jd[i])){
      printf("%d: submit_job(jd[%2d]) failed, status = %d\n", __LINE__, i, jd[i]->status);
    }
  }

  // wait for them all

  int n = mgr->wait_jobs(NJOBS, jd, done, GC_WAIT_ALL);
  CPPUNIT_ASSERT_EQUAL(NJOBS, n);

  // check results

  for (int i = 0, val = STARTING_VAL; i < NJOBS; i++, val += 3){
    CPPUNIT_ASSERT_EQUAL(JS_OK, jd[i]->status);
    CPPUNIT_ASSERT(confirm_seq(&buf[i * LEN_PER_JOB], LEN_PER_JOB, val));
  }
  
  // cleanup
  for (int i = 0; i < NJOBS; i++)
    mgr->free_job_desc(jd[i]);
}

void
qa_job_manager::t15_body()
{
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&gcell_runtime_qa_spx);
  opts.nspes = 1;
  gc_job_manager_sptr mgr = gc_make_job_manager(&opts);

  gc_job_manager::set_singleton(mgr);

  CPPUNIT_ASSERT(gc_job_manager::singleton());
  mgr.reset();
  CPPUNIT_ASSERT_THROW(gc_job_manager::singleton(), boost::bad_weak_ptr);
}
