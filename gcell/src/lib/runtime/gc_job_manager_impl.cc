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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gc_job_manager_impl.h>
#include <gc_mbox.h>
#include <gc_proc_def_utils.h>

#include <stdio.h>
#include <stdexcept>
#include <stdlib.h>
#include <atomic_dec_if_positive.h>
#include <memory_barrier.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


static const size_t CACHE_LINE_SIZE = 128;

static const unsigned int DEFAULT_MAX_JOBS = 128;
static const unsigned int DEFAULT_MAX_CLIENT_THREADS = 64;

// FIXME this really depends on the SPU code...
static const unsigned int MAX_TOTAL_INDIRECT_LENGTH = 16 * 1024;


static bool          s_key_initialized = false;
static pthread_key_t s_client_key;

static int s_worker_debug = 0;

// custom deleter of gang_contexts for use with boost::shared_ptr
class gang_deleter {
public:
  void operator()(spe_gang_context_ptr_t ctx) {
    if (ctx){
      int r = spe_gang_context_destroy(ctx);
      if (r != 0){
	perror("spe_gang_context_destroy");
      }
    }
  }
};

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


// custom deleter of anything that can be freed with "free"
class free_deleter {
public:
  void operator()(void *p) {
    free(p);
  }
};


/*
 * Called when client thread is destroyed.
 * We mark our client info free.
 */
static void
client_key_destructor(void *p)
{
  ((gc_client_thread_info *) p)->d_free = 1;
}

/*
 * Return pointer to cache-aligned chunk of storage of size size bytes.
 * Throw if can't allocate memory.  The storage should be freed
 * with "free" when done.  The memory is initialized to zero.
 */
static void *
aligned_alloc(size_t size, size_t alignment = CACHE_LINE_SIZE)
{
  void *p = 0;
  if (posix_memalign(&p, alignment, size) != 0){
    perror("posix_memalign");
    throw std::runtime_error("memory");
  }
  memset(p, 0, size);		// zero the memory
  return p;
}

static bool
is_power_of_2(uint32_t x)
{
  return (x != 0) && !(x & (x - 1));
}

////////////////////////////////////////////////////////////////////////


gc_job_manager_impl::gc_job_manager_impl(const gc_jm_options *options)
  : d_debug(0), d_spu_args(0),
    d_eh_cond(&d_eh_mutex), d_eh_thread(0), d_eh_state(EHS_INIT),
    d_shutdown_requested(false),
    d_client_thread(0), d_ea_args_maxsize(0),
    d_proc_def(0), d_proc_def_ls_addr(0), d_nproc_defs(0)
{
  if (!s_key_initialized){
    int r = pthread_key_create(&s_client_key, client_key_destructor);
    if (r != 0)
      throw std::runtime_error("pthread_key_create");
    s_key_initialized = true;
  }

  // ensure it's zero
  pthread_setspecific(s_client_key, 0);

  if (options != 0)
    d_options = *options;

  // provide the real default for those indicated with a zero
  if (d_options.max_jobs == 0)
    d_options.max_jobs = DEFAULT_MAX_JOBS;
  if (d_options.max_client_threads == 0)
    d_options.max_client_threads = DEFAULT_MAX_CLIENT_THREADS;

  if (d_options.program_handle == 0){
    fprintf(stderr, "gc_job_manager: options->program_handle must be non-zero\n");
    throw std::runtime_error("gc_job_manager: options->program_handle must be non-zero");
  }

  int ncpu_nodes = spe_cpu_info_get(SPE_COUNT_PHYSICAL_CPU_NODES, -1);
  int nusable_spes = spe_cpu_info_get(SPE_COUNT_USABLE_SPES, -1);

  if (debug()){
    printf("cpu_nodes = %d\n", ncpu_nodes);
    for (int i = 0; i < ncpu_nodes; i++){
      printf("node[%d].physical_spes = %2d\n", i,
	     spe_cpu_info_get(SPE_COUNT_PHYSICAL_SPES, i));
      printf("node[%d].usable_spes   = %2d\n", i,
	     spe_cpu_info_get(SPE_COUNT_USABLE_SPES, i));
    }
  }

  // clamp nspes
  d_options.nspes = std::min(d_options.nspes, (unsigned int) MAX_SPES);
  nusable_spes = std::min(nusable_spes, (int) MAX_SPES);

  //
  // sanity check requested number of spes.
  //
  if (d_options.nspes == 0)	// use all of them
    d_options.nspes = nusable_spes;
  else {
    if (d_options.nspes > (unsigned int) nusable_spes){
      fprintf(stderr,
	      "gc_job_manager: warning: caller requested %d spes.  There are only %d available.\n",
	      d_options.nspes, nusable_spes);
      if (d_options.gang_schedule){
	// If we're gang scheduling we'll never get scheduled if we
	// ask for more than are available.
	throw std::out_of_range("gang_scheduling: not enough spes available");
      }
      else {	// FIXME clamp to usable.  problem on PS3 when overcommited
	fprintf(stderr, "gc_job_manager: clamping nspes to %d\n", nusable_spes);
	d_options.nspes = nusable_spes;
      }
    }
  }

  if (d_options.use_affinity){
    printf("gc_job_manager: warning: affinity request was ignored\n");
  }

  if (d_options.gang_schedule){
    d_gang = spe_gang_context_sptr(spe_gang_context_create(0), gang_deleter());
    if (!d_gang){
      perror("gc_job_manager_impl[spe_gang_context_create]");
      throw std::runtime_error("spe_gang_context_create");
    }
  }

  // ----------------------------------------------------------------
  // initalize the job queue
  
  d_queue = (gc_jd_queue_t *) aligned_alloc(sizeof(gc_jd_queue_t));
  _d_queue_boost =
    boost::shared_ptr<void>((void *) d_queue, free_deleter());
  gc_jd_queue_init(d_queue);


  // ----------------------------------------------------------------
  // create the spe contexts

  // 1 spu_arg struct for each SPE
  assert(sizeof(gc_spu_args_t) % 16 == 0);
  d_spu_args =
    (gc_spu_args_t *) aligned_alloc(MAX_SPES * sizeof(gc_spu_args_t), 16);
  _d_spu_args_boost =
    boost::shared_ptr<void>((void *) d_spu_args, free_deleter());

  // 2 completion info structs for each SPE (we double buffer them)
  assert(sizeof(gc_comp_info_t) % CACHE_LINE_SIZE == 0);
  d_comp_info =
    (gc_comp_info_t *) aligned_alloc(2 * MAX_SPES * sizeof(gc_comp_info_t),
				     CACHE_LINE_SIZE);
  _d_comp_info_boost =
    boost::shared_ptr<void>((void *) d_comp_info, free_deleter());


  // get a handle to the spe program

  spe_program_handle_t *spe_image = d_options.program_handle;

  // fish proc_def table out of SPE ELF file

  if (!gcpd_find_table(spe_image, &d_proc_def, &d_nproc_defs, &d_proc_def_ls_addr)){
    fprintf(stderr, "gc_job_manager_impl: couldn't find gc_proc_defs in SPE ELF file.\n");
    throw std::runtime_error("no gc_proc_defs");
  }
  // fprintf(stderr, "d_proc_def_ls_addr = 0x%0x\n", d_proc_def_ls_addr);

  int spe_flags = (SPE_EVENTS_ENABLE
		   | SPE_CFG_SIGNOTIFY1_OR
		   | SPE_CFG_SIGNOTIFY2_OR);
  
  for (unsigned int i = 0; i < d_options.nspes; i++){
    // FIXME affinity stuff goes here
    d_worker[i].spe_ctx = spe_context_create(spe_flags, d_gang.get());;
    if (d_worker[i].spe_ctx == 0){
      perror("spe_context_create");
      throw std::runtime_error("spe_context_create");
    }
    d_worker[i].spe_idx = i;
    d_worker[i].spu_args = &d_spu_args[i];
    d_worker[i].spu_args->queue = ptr_to_ea(d_queue);
    d_worker[i].spu_args->comp_info[0] = ptr_to_ea(&d_comp_info[2*i+0]);
    d_worker[i].spu_args->comp_info[1] = ptr_to_ea(&d_comp_info[2*i+1]);
    d_worker[i].spu_args->spu_idx = i;
    d_worker[i].spu_args->nspus = d_options.nspes;
    d_worker[i].spu_args->proc_def_ls_addr = d_proc_def_ls_addr;
    d_worker[i].spu_args->nproc_defs = d_nproc_defs;
    d_worker[i].spu_args->log.base = 0;
    d_worker[i].spu_args->log.nentries = 0;
    d_worker[i].state = WS_INIT;

    int r = spe_program_load(d_worker[i].spe_ctx, spe_image);
    if (r != 0){
      perror("spe_program_load");
      throw std::runtime_error("spe_program_load");
    }
  }

  setup_logfiles();

  // ----------------------------------------------------------------
  // initalize the free list of job descriptors
  
  d_free_list = (gc_jd_stack_t *) aligned_alloc(sizeof(gc_jd_stack_t));
  // This ensures that the memory associated with d_free_list is
  // automatically freed in the destructor or if an exception occurs
  // here in the constructor.
  _d_free_list_boost =
    boost::shared_ptr<void>((void *) d_free_list, free_deleter());
  gc_jd_stack_init(d_free_list);

  if (debug()){
    printf("sizeof(d_jd[0]) = %d (0x%x)\n", sizeof(d_jd[0]), sizeof(d_jd[0]));
    printf("max_jobs = %u\n", d_options.max_jobs);
  }

  // Initialize the array of job descriptors.
  d_jd = (gc_job_desc_t *) aligned_alloc(sizeof(d_jd[0]) * d_options.max_jobs);
  _d_jd_boost = boost::shared_ptr<void>((void *) d_jd, free_deleter());


  // set unique job_id
  for (int i = 0; i < (int) d_options.max_jobs; i++)
    d_jd[i].sys.job_id = i;

  // push them onto the free list
  for (int i = d_options.max_jobs - 1; i >= 0; i--)
    free_job_desc(&d_jd[i]);

  // ----------------------------------------------------------------
  // initialize d_client_thread

  {
    gc_client_thread_info_sa cti(
         new gc_client_thread_info[d_options.max_client_threads]);

    d_client_thread.swap(cti);

    for (unsigned int i = 0; i < d_options.max_client_threads; i++)
      d_client_thread[i].d_client_id = i;
  }

  // ----------------------------------------------------------------
  // initialize bitvectors

  // initialize d_bvlen, the number of longs in job related bitvectors.
  int bits_per_long = sizeof(unsigned long) * 8;
  d_bvlen = (d_options.max_jobs + bits_per_long - 1) / bits_per_long;

  // allocate all bitvectors in a single cache-aligned chunk
  size_t nlongs = d_bvlen * d_options.max_client_threads;
  void *p = aligned_alloc(nlongs * sizeof(unsigned long));
  _d_all_bitvectors = boost::shared_ptr<void>(p, free_deleter());

  // Now point the gc_client_thread_info bitvectors into this storage
  unsigned long *v = (unsigned long *) p;

  for (unsigned int i = 0; i < d_options.max_client_threads; i++, v += d_bvlen)
    d_client_thread[i].d_jobs_done = v;


  // ----------------------------------------------------------------
  // create the spe event handler & worker (SPE) threads

  create_event_handler();

}

////////////////////////////////////////////////////////////////////////

gc_job_manager_impl::~gc_job_manager_impl()
{
  shutdown();

  d_jd = 0;		// handled via _d_jd_boost
  d_free_list = 0;	// handled via _d_free_list_boost
  d_queue = 0;		// handled via _d_queue_boost

  // clear cti, since we've deleted the underlying data
  pthread_setspecific(s_client_key, 0);

  unmap_logfiles();
}

bool
gc_job_manager_impl::shutdown()
{
  omni_mutex_lock	l(d_eh_mutex);

  d_shutdown_requested = true;		// set flag for event handler thread

  // should only happens during early QA code
  if (d_eh_thread == 0 && d_eh_state == EHS_INIT)
    return false;

  while (d_eh_state != EHS_DEAD)	// wait for it to finish
    d_eh_cond.wait();

  return true;
}

int
gc_job_manager_impl::nspes() const
{
  return d_options.nspes;
}

////////////////////////////////////////////////////////////////////////

void
gc_job_manager_impl::bv_zero(unsigned long *bv)
{
  memset(bv, 0, sizeof(unsigned long) * d_bvlen);
}

inline void
gc_job_manager_impl::bv_clr(unsigned long *bv, unsigned int bitno)
{
  unsigned int wi = bitno / (sizeof (unsigned long) * 8);
  unsigned int bi = bitno & ((sizeof (unsigned long) * 8) - 1);
  bv[wi] &= ~(1UL << bi);
}

inline void
gc_job_manager_impl::bv_set(unsigned long *bv, unsigned int bitno)
{
  unsigned int wi = bitno / (sizeof (unsigned long) * 8);
  unsigned int bi = bitno & ((sizeof (unsigned long) * 8) - 1);
  bv[wi] |= (1UL << bi);
}

inline bool
gc_job_manager_impl::bv_isset(unsigned long *bv, unsigned int bitno)
{
  unsigned int wi = bitno / (sizeof (unsigned long) * 8);
  unsigned int bi = bitno & ((sizeof (unsigned long) * 8) - 1);
  return (bv[wi] & (1UL << bi)) != 0;
}

inline bool
gc_job_manager_impl::bv_isclr(unsigned long *bv, unsigned int bitno)
{
  unsigned int wi = bitno / (sizeof (unsigned long) * 8);
  unsigned int bi = bitno & ((sizeof (unsigned long) * 8) - 1);
  return (bv[wi] & (1UL << bi)) == 0;
}

////////////////////////////////////////////////////////////////////////

gc_job_desc *
gc_job_manager_impl::alloc_job_desc()
{
  // stack is lock free, thus safe to call from any thread
  return gc_jd_stack_pop(d_free_list);
}

void
gc_job_manager_impl::free_job_desc(gc_job_desc *jd)
{
  // stack is lock free, thus safe to call from any thread
  if (jd != 0)
    gc_jd_stack_push(d_free_list, jd);
}

////////////////////////////////////////////////////////////////////////

/*
 * We check as much as we can here on the PPE side, so that the SPE
 * doesn't have to.
 */
static bool
check_direct_args(gc_job_desc *jd, gc_job_direct_args *args)
{
  if (args->nargs > MAX_ARGS_DIRECT){
    jd->status = JS_BAD_N_DIRECT;
    return false;
  }

  return true;
}

static bool
check_ea_args(gc_job_desc *jd, gc_job_ea_args *p)
{
  if (p->nargs > MAX_ARGS_EA){
    jd->status = JS_BAD_N_EA;
    return false;
  }

  uint32_t dir_union = 0;

  for (unsigned int i = 0; i < p->nargs; i++){
    dir_union |= p->arg[i].direction;
    switch(p->arg[i].direction){
    case GCJD_DMA_GET:
    case GCJD_DMA_PUT:
      break;

    default:
      jd->status = JS_BAD_DIRECTION;
      return false;
    }
  }

  if (p->nargs > 1){
    unsigned int common_eah = (p->arg[0].ea_addr) >> 32;
    for (unsigned int i = 1; i < p->nargs; i++){
      if ((p->arg[i].ea_addr >> 32) != common_eah){
	jd->status = JS_BAD_EAH;
	return false;
      }
    }
  }

  jd->sys.direction_union = dir_union;
  return true;
}

bool
gc_job_manager_impl::submit_job(gc_job_desc *jd)
{
  if (unlikely(d_shutdown_requested)){
    jd->status = JS_SHUTTING_DOWN;
    return false;
  }

  // Ensure it's one of our job descriptors

  if (jd < d_jd || jd >= &d_jd[d_options.max_jobs]){
    jd->status = JS_BAD_JOB_DESC;
    return false;
  }

  // Ensure we've got a client_thread_info assigned to this thread.
  
  gc_client_thread_info *cti =
    (gc_client_thread_info *) pthread_getspecific(s_client_key);
  if (unlikely(cti == 0)){
    if ((cti = alloc_cti()) == 0){
      fprintf(stderr, "gc_job_manager_impl::submit_job: Too many client threads.\n");
      jd->status = JS_TOO_MANY_CLIENTS;
      return false;
    }
    int r = pthread_setspecific(s_client_key, cti);
    if (r != 0){
      jd->status = JS_BAD_JUJU;
      fprintf(stderr, "pthread_setspecific failed (return = %d)\n", r);
      return false;
    }
  }

  if (jd->proc_id == GCP_UNKNOWN_PROC){
    jd->status = JS_UNKNOWN_PROC;
    return false;
  }

  if (!check_direct_args(jd, &jd->input))
    return false;

  if (!check_direct_args(jd, &jd->output))
    return false;

  if (!check_ea_args(jd, &jd->eaa))
    return false;

  jd->status = JS_OK;
  jd->sys.client_id = cti->d_client_id;

  // FIXME keep count of jobs in progress?
  
  gc_jd_queue_enqueue(d_queue, jd);
  return true;
}

bool
gc_job_manager_impl::wait_job(gc_job_desc *jd)
{
  bool done;
  return wait_jobs(1, &jd, &done, GC_WAIT_ANY) == 1;
}

int
gc_job_manager_impl::wait_jobs(unsigned int njobs,
			       gc_job_desc *jd[],
			       bool done[],
			       gc_wait_mode mode)
{
  unsigned int i;

  gc_client_thread_info *cti =
    (gc_client_thread_info *) pthread_getspecific(s_client_key);
  if (unlikely(cti == 0))
    return -1;

  for (i = 0; i < njobs; i++){
    done[i] = false;
    if (unlikely(jd[i]->sys.client_id != cti->d_client_id)){
      fprintf(stderr, "gc_job_manager_impl::wait_jobs: can't wait for a job you didn't submit\n");
      return -1;
    }
  }

  {
    omni_mutex_lock	l(cti->d_mutex);

    // setup info for event handler
    cti->d_state = (mode == GC_WAIT_ANY) ? CT_WAIT_ANY : CT_WAIT_ALL;
    cti->d_njobs_waiting_for = njobs;
    cti->d_jobs_waiting_for = jd;
    assert(cti->d_jobs_done != 0);

    unsigned int ndone = 0;

    // wait for jobs to complete
    
    while (1){
      ndone = 0;
      for (i= 0; i < njobs; i++){
	if (done[i])
	  ndone++;
	else if (bv_isset(cti->d_jobs_done, jd[i]->sys.job_id)){
	  bv_clr(cti->d_jobs_done, jd[i]->sys.job_id);
	  done[i] = true;
	  ndone++;
	}
      }

      if (mode == GC_WAIT_ANY && ndone > 0)
	break;

      if (mode == GC_WAIT_ALL && ndone == njobs)
	break;

      // FIXME what happens when somebody calls shutdown?

      cti->d_cond.wait();	// wait for event handler to wake us up
    }

    cti->d_state = CT_NOT_WAITING;  
    cti->d_njobs_waiting_for = 0;	// tidy up (not reqd)
    cti->d_jobs_waiting_for = 0;	// tidy up (not reqd)
    return ndone;
  }
}

////////////////////////////////////////////////////////////////////////

bool
gc_job_manager_impl::send_all_spes(uint32_t msg)
{
  bool ok = true;

  for (unsigned int i = 0; i < d_options.nspes; i++)
    ok &= send_spe(i, msg);

  return ok;
}

bool
gc_job_manager_impl::send_spe(unsigned int spe, uint32_t msg)
{
  if (spe >= d_options.nspes)
    return false;

  int r = spe_in_mbox_write(d_worker[spe].spe_ctx, &msg, 1,
			    SPE_MBOX_ALL_BLOCKING);
  if (r < 0){
    perror("spe_in_mbox_write");
    return false;
  }

  return r == 1;
}

////////////////////////////////////////////////////////////////////////

static void
pthread_create_failure_msg(int r, const char *which)
{
  char buf[256];
  char *s = 0;

  switch (r){
  case EAGAIN: s = "EAGAIN"; break;
  case EINVAL: s = "EINVAL"; break;
  case EPERM:  s = "EPERM";  break;
  default:
    snprintf(buf, sizeof(buf), "Unknown error %d", r);
    s = buf;
    break;
  }
  fprintf(stderr, "pthread_create[%s] failed: %s\n", which, s);
}


static bool
start_thread(pthread_t *thread,
	     void *(*start_routine)(void *),  void *arg,
	     const char *msg)
{
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  // FIXME save sigprocmask
  // FIXME set sigprocmask

  int r = pthread_create(thread, &attr, start_routine, arg);
    
  // FIXME restore sigprocmask

  if (r != 0){
    pthread_create_failure_msg(r, msg);
    return false;
  }
  return true;
}


////////////////////////////////////////////////////////////////////////

static void *start_worker(void *arg);

static void *
start_event_handler(void *arg)
{
  gc_job_manager_impl *p = (gc_job_manager_impl *) arg;
  p->event_handler_loop();
  return 0;
}

void
gc_job_manager_impl::create_event_handler()
{
  // create the SPE event handler and register our interest in events

  d_spe_event_handler.ptr = spe_event_handler_create();
  if (d_spe_event_handler.ptr == 0){
    perror("spe_event_handler_create");
    throw std::runtime_error("spe_event_handler_create");
  }

  for (unsigned int i = 0; i < d_options.nspes; i++){
    spe_event_unit_t	eu;
    memset(&eu, 0, sizeof(eu));
    eu.events = SPE_EVENT_OUT_INTR_MBOX | SPE_EVENT_SPE_STOPPED;
    eu.spe = d_worker[i].spe_ctx;
    eu.data.u32 = i;	// set in events returned by spe_event_wait

    if (spe_event_handler_register(d_spe_event_handler.ptr, &eu) != 0){
      perror("spe_event_handler_register");
      throw std::runtime_error("spe_event_handler_register");
    }
  }

  // create our event handling thread

  if (!start_thread(&d_eh_thread, start_event_handler, this, "event_handler")){
    throw std::runtime_error("pthread_create");
  }

  // create the SPE worker threads

  bool ok = true;
  for (unsigned int i = 0; ok && i < d_options.nspes; i++){
    char name[256];
    snprintf(name, sizeof(name), "worker[%d]", i);
    ok &= start_thread(&d_worker[i].thread, start_worker,
		       &d_worker[i], name);
  }

  if (!ok){
    //
    // FIXME Clean up the mess.  Need to terminate event handler and all workers.
    //
    // this should cause the workers to exit, unless they're seriously broken
    send_all_spes(MK_MBOX_MSG(OP_EXIT, 0));

    shutdown();

    throw std::runtime_error("pthread_create");
  }
}

////////////////////////////////////////////////////////////////////////

void
gc_job_manager_impl::set_eh_state(evt_handler_state s)
{
  omni_mutex_lock	l(d_eh_mutex);
  d_eh_state = s;
  d_eh_cond.broadcast();
}

void
gc_job_manager_impl::set_ea_args_maxsize(int maxsize)
{
  omni_mutex_lock	l(d_eh_mutex);
  d_ea_args_maxsize = maxsize;
  d_eh_cond.broadcast();
}

void
gc_job_manager_impl::print_event(spe_event_unit_t *evt)
{
  printf("evt: spe = %d events = (0x%x)", evt->data.u32, evt->events);

  if (evt->events & SPE_EVENT_OUT_INTR_MBOX)
    printf(" OUT_INTR_MBOX");
  
  if (evt->events & SPE_EVENT_IN_MBOX)
    printf(" IN_MBOX");
  
  if (evt->events & SPE_EVENT_TAG_GROUP)
    printf(" TAG_GROUP");
  
  if (evt->events & SPE_EVENT_SPE_STOPPED)
    printf(" SPE_STOPPED");

  printf("\n");
}

struct job_client_info {
  uint16_t	job_id;
  uint16_t	client_id;
};

static int
compare_jci_clients(const void *va, const void *vb)
{
  const job_client_info *a = (job_client_info *) va;
  const job_client_info *b = (job_client_info *) vb;

  return a->client_id - b->client_id;
}

void
gc_job_manager_impl::notify_clients_jobs_are_done(unsigned int spe_num,
						  unsigned int completion_info_idx)
{
  const char *msg = "gc_job_manager_impl::notify_client_job_is_done (INTERNAL ERROR)";

  smp_rmb();  // order reads so we know that data sent from SPE is here

  gc_comp_info_t *ci = &d_comp_info[2 * spe_num + (completion_info_idx & 0x1)];

  if (ci->ncomplete == 0){	// never happens, but ensures code below is correct
    ci->in_use = 0;
    return;
  }

  if (0){
    static int total_jobs;
    static int total_msgs;
    total_msgs++;
    total_jobs += ci->ncomplete;
    printf("ppe:     tj = %6d  tm = %6d\n", total_jobs, total_msgs);
  }

  job_client_info gci[GC_CI_NJOBS];

  /*
   * Make one pass through and sanity check everything while filling in gci
   */
  for (unsigned int i = 0; i < ci->ncomplete; i++){
    unsigned int job_id = ci->job_id[i];

    if (job_id >= d_options.max_jobs){
      // internal error, shouldn't happen
      fprintf(stderr,"%s: invalid job_id = %d\n", msg, job_id);
      ci->in_use = 0;		// clear flag so SPE knows we're done with it
      return;
    }
    gc_job_desc *jd = &d_jd[job_id];

    if (jd->sys.client_id >= d_options.max_client_threads){
      // internal error, shouldn't happen
      fprintf(stderr, "%s: invalid client_id = %d\n", msg, jd->sys.client_id);
      ci->in_use = 0;		// clear flag so SPE knows we're done with it
      return;
    }

    gci[i].job_id = job_id;
    gci[i].client_id = jd->sys.client_id;
  }

  // sort by client_id so we only have to lock & signal once / client

  if (ci->ncomplete > 1)
    qsort(gci, ci->ncomplete, sizeof(gci[0]), compare_jci_clients);

  // "wind-in" 

  gc_client_thread_info *last_cti = &d_client_thread[gci[0].client_id];
  last_cti->d_mutex.lock();
  bv_set(last_cti->d_jobs_done, gci[0].job_id);  // mark job done

  for (unsigned int i = 1; i < ci->ncomplete; i++){

    gc_client_thread_info *cti = &d_client_thread[gci[i].client_id];

    if (cti != last_cti){	// new client?

      // yes.  signal old client, unlock old, lock new

      // FIXME we could distinguish between CT_WAIT_ALL & CT_WAIT_ANY

      if (last_cti->d_state == CT_WAIT_ANY || last_cti->d_state == CT_WAIT_ALL)
	last_cti->d_cond.signal();	// wake client thread up

      last_cti->d_mutex.unlock();
      cti->d_mutex.lock();
      last_cti = cti;
    }

    // mark job done
    bv_set(cti->d_jobs_done, gci[i].job_id);
  }

  // "wind-out"

  if (last_cti->d_state == CT_WAIT_ANY || last_cti->d_state == CT_WAIT_ALL)
    last_cti->d_cond.signal();	// wake client thread up
  last_cti->d_mutex.unlock();

  ci->in_use = 0;		// clear flag so SPE knows we're done with it
}

void
gc_job_manager_impl::handle_event(spe_event_unit_t *evt)
{
  // print_event(evt);

  int spe_num = evt->data.u32;

  // only a single event type can be signaled at a time
  
  if (evt->events == SPE_EVENT_OUT_INTR_MBOX) { // SPE sent us 1 or more msgs
    static const int NMSGS = 32;
    unsigned int msg[NMSGS];
    int n = spe_out_intr_mbox_read(evt->spe, msg, NMSGS, SPE_MBOX_ANY_BLOCKING);
    // printf("spe_out_intr_mbox_read = %d\n", n);
    if (n < 0){
      perror("spe_out_intr_mbox_read");
    }
    else {
      for (int i = 0; i < n; i++){
	switch(MBOX_MSG_OP(msg[i])){
	case OP_JOBS_DONE:
	  if (debug())
	    printf("eh: job_done (0x%08x) from spu[%d]\n", msg[i], spe_num);
	  notify_clients_jobs_are_done(spe_num, MBOX_MSG_ARG(msg[i]));
	  break;

	case OP_SPU_BUFSIZE:
	  set_ea_args_maxsize(MBOX_MSG_ARG(msg[i]));
	  break;

	case OP_EXIT:
	default:
	  printf("eh: Unexpected msg (0x%08x) from spu[%d]\n", msg[i], spe_num);
	  break;
	}
      }
    }
  }
  else if (evt->events == SPE_EVENT_SPE_STOPPED){ // the SPE stopped
    spe_stop_info_t si;
    int r = spe_stop_info_read(evt->spe, &si);
    if (r < 0){
      perror("spe_stop_info_read");
    }
    else {
      switch (si.stop_reason){
      case SPE_EXIT:
	if (debug()){
	  printf("eh: spu[%d] SPE_EXIT w/ exit_code = %d\n",
		 spe_num, si.result.spe_exit_code);
	}
	break;
      case SPE_STOP_AND_SIGNAL:
	printf("eh: spu[%d] SPE_STOP_AND_SIGNAL w/ spe_signal_code = 0x%x\n",
	       spe_num, si.result.spe_signal_code);
	break;
      case SPE_RUNTIME_ERROR:
	printf("eh: spu[%d] SPE_RUNTIME_ERROR w/ spe_runtime_error = 0x%x\n",
	       spe_num, si.result.spe_runtime_error);
	break;
      case SPE_RUNTIME_EXCEPTION:
	printf("eh: spu[%d] SPE_RUNTIME_EXCEPTION w/ spe_runtime_exception = 0x%x\n",
	       spe_num, si.result.spe_runtime_exception);
	break;
      case SPE_RUNTIME_FATAL:
	printf("eh: spu[%d] SPE_RUNTIME_FATAL w/ spe_runtime_fatal = 0x%x\n",
	       spe_num, si.result.spe_runtime_fatal);
	break;
      case SPE_CALLBACK_ERROR:
	printf("eh: spu[%d] SPE_CALLBACK_ERROR w/ spe_callback_error = 0x%x\n",
	       spe_num, si.result.spe_callback_error);
	break;
      case SPE_ISOLATION_ERROR:
	printf("eh: spu[%d] SPE_ISOLATION_ERROR w/ spe_isolation_error = 0x%x\n",
	       spe_num, si.result.spe_isolation_error);
	break;
      default:
	printf("eh: spu[%d] UNKNOWN STOP REASON (%d) w/ spu_status = 0x%x\n",
	       spe_num, si.stop_reason, si.spu_status);
	break;
      }
    }
  }
#if 0 // not enabled
  else if (evt->events == SPE_EVENT_IN_MBOX){	 // there's room to write to SPE
    // spe_in_mbox_write (ignore)
  }
  else if (evt->events == SPE_EVENT_TAG_GROUP){	 // our DMA completed
    // spe_mfcio_tag_status_read
  }
#endif
  else {
    fprintf(stderr, "handle_event: unexpected evt->events = 0x%x\n", evt->events);
    return;
  }
}

//
// This is the "main program" of the event handling thread
//
void
gc_job_manager_impl::event_handler_loop()
{
  static const int MAX_EVENTS = 16;
  static const int TIMEOUT = 20;	// how long to block in milliseconds

  spe_event_unit_t events[MAX_EVENTS];

  if (d_debug)
    printf("event_handler_loop: starting\n");

  set_eh_state(EHS_RUNNING);

  // ask the first spe for its max bufsize
  send_spe(0, MK_MBOX_MSG(OP_GET_SPU_BUFSIZE, 0));

  while (1){
    switch(d_eh_state){

    case EHS_RUNNING:      // normal stuff
      if (d_shutdown_requested) {
	set_eh_state(EHS_SHUTTING_DOWN);
      }
      break;

    case EHS_SHUTTING_DOWN:

      // FIXME wait until job queue is empty, then tell them to exit

      send_all_spes(MK_MBOX_MSG(OP_EXIT, 0));
      set_eh_state(EHS_WAITING_FOR_WORKERS_TO_DIE);
      break;

    case EHS_WAITING_FOR_WORKERS_TO_DIE:
      {
	bool all_dead = true;
	for (unsigned int i = 0; i < d_options.nspes; i++)
	  all_dead &= d_worker[i].state == WS_DEAD;

	if (all_dead){
	  set_eh_state(EHS_DEAD);
	  if (d_debug)
	    printf("event_handler_loop: exiting\n");
	  return;
	}
      }
      break;

    default:
      set_eh_state(EHS_DEAD);
      printf("event_handler_loop(default): exiting\n");
      return;
    }

    // block waiting for events...
    int nevents = spe_event_wait(d_spe_event_handler.ptr,
				 events, MAX_EVENTS, TIMEOUT);
    if (nevents < 0){
      perror("spe_wait_event");
      // FIXME bail?
    }
    for (int i = 0; i < nevents; i++){
      handle_event(&events[i]);
    }
  }
}

////////////////////////////////////////////////////////////////////////
// This is the top of the SPE worker threads

static void *
start_worker(void *arg)
{
  worker_ctx *w = (worker_ctx *) arg;
  spe_stop_info_t	si;

  w->state = WS_RUNNING;
  if (s_worker_debug)
    printf("worker[%d]: WS_RUNNING\n", w->spe_idx);

  unsigned int entry = SPE_DEFAULT_ENTRY;
  int r = spe_context_run(w->spe_ctx,  &entry, 0, w->spu_args, 0, &si);

  if (r < 0){			// error
    char buf[64];
    snprintf(buf, sizeof(buf), "worker[%d]: spe_context_run", w->spe_idx);
    perror(buf);
  }
  else if (r == 0){
    // spe program called exit.
    if (s_worker_debug)
      printf("worker[%d]: SPE_EXIT w/ exit_code = %d\n",
	     w->spe_idx, si.result.spe_exit_code);
  }
  else {
    // called stop_and_signal
    //
    // I'm not sure we'll ever get here.  I think the event
    // handler will catch this...
    printf("worker[%d]: SPE_STOP_AND_SIGNAL w/ spe_signal_code = 0x%x\n",
	   w->spe_idx, si.result.spe_signal_code);
  }

  // in any event, we're committing suicide now ;)
  if (s_worker_debug)
    printf("worker[%d]: WS_DEAD\n", w->spe_idx);

  w->state = WS_DEAD;
  return 0;
}

////////////////////////////////////////////////////////////////////////

gc_client_thread_info *
gc_job_manager_impl::alloc_cti()
{
  for (unsigned int i = 0; i < d_options.max_client_threads; i++){
    if (d_client_thread[i].d_free){
      // try to atomically grab it
      if (_atomic_dec_if_positive(ptr_to_ea(&d_client_thread[i].d_free)) == 0){
	// got it...
	gc_client_thread_info *cti = &d_client_thread[i];
	cti->d_state = CT_NOT_WAITING;
	bv_zero(cti->d_jobs_done);
	cti->d_njobs_waiting_for = 0;
	cti->d_jobs_waiting_for = 0;
	
	return cti;
      }
    }
  }
  return 0;
}

void
gc_job_manager_impl::free_cti(gc_client_thread_info *cti)
{
  assert((size_t) (cti - d_client_thread.get()) < d_options.max_client_threads);
  cti->d_free = 1;
}

int
gc_job_manager_impl::ea_args_maxsize()
{
  omni_mutex_lock	l(d_eh_mutex);

  while (d_ea_args_maxsize == 0)	// wait for it to be initialized
    d_eh_cond.wait();

  return d_ea_args_maxsize;
}

void
gc_job_manager_impl::set_debug(int debug)
{
  d_debug = debug;
  s_worker_debug = debug;
}

int
gc_job_manager_impl::debug()
{
  return d_debug;
}

////////////////////////////////////////////////////////////////////////

void
gc_job_manager_impl::setup_logfiles()
{
  if (!d_options.enable_logging)
    return;

  if (d_options.log2_nlog_entries == 0)
    d_options.log2_nlog_entries = 12;

  // must end up a multiple of the page size

  size_t pagesize = getpagesize();
  size_t s = (1 << d_options.log2_nlog_entries) * sizeof(gc_log_entry_t);
  s = ((s + pagesize - 1) / pagesize) * pagesize;
  size_t nentries = s / sizeof(gc_log_entry_t);
  assert(is_power_of_2(nentries));

  for (unsigned int i = 0; i < d_options.nspes; i++){
    char filename[100];
    snprintf(filename, sizeof(filename), "spu_log.%02d", i);
    int fd = open(filename, O_CREAT|O_TRUNC|O_RDWR, 0664);
    if (fd == -1){
      perror(filename);
      return;
    }
    lseek(fd, s - 1, SEEK_SET);
    write(fd, "\0", 1);
    void *p = mmap(0, s, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED){
      perror("gc_job_manager_impl::setup_logfiles: mmap");
      close(fd);
      return;
    }
    close(fd);
    memset(p, 0, s);
    d_spu_args[i].log.base = ptr_to_ea(p);
    d_spu_args[i].log.nentries = nentries;
  }
}

void
gc_job_manager_impl::sync_logfiles()
{
  for (unsigned int i = 0; i < d_options.nspes; i++){
    if (d_spu_args[i].log.base)
      msync(ea_to_ptr(d_spu_args[i].log.base),
	    d_spu_args[i].log.nentries * sizeof(gc_log_entry_t),
	    MS_ASYNC);
  }
}

void
gc_job_manager_impl::unmap_logfiles()
{
  for (unsigned int i = 0; i < d_options.nspes; i++){
    if (d_spu_args[i].log.base)
      munmap(ea_to_ptr(d_spu_args[i].log.base),
	     d_spu_args[i].log.nentries * sizeof(gc_log_entry_t));
  }
}

////////////////////////////////////////////////////////////////////////
//
// lookup proc names in d_proc_def table

gc_proc_id_t 
gc_job_manager_impl::lookup_proc(const std::string &proc_name)
{
  for (int i = 0; i < d_nproc_defs; i++)
    if (proc_name == d_proc_def[i].name)
      return i;

  return GCP_UNKNOWN_PROC;
}

std::vector<std::string>
gc_job_manager_impl::proc_names()
{
  std::vector<std::string> r;
  for (int i = 0; i < d_nproc_defs; i++)
    r.push_back(d_proc_def[i].name);

  return r;
}

////////////////////////////////////////////////////////////////////////

worker_ctx::~worker_ctx()
{
  if (spe_ctx){
    int r = spe_context_destroy(spe_ctx);
    if (r != 0){
      perror("spe_context_destroy");
    }
    spe_ctx = 0;
  }
  state = WS_FREE;
}
