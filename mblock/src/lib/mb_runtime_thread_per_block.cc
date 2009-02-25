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
#include <mb_runtime_thread_per_block.h>
#include <mblock/mblock.h>
#include <mb_mblock_impl.h>
#include <mblock/class_registry.h>
#include <mblock/exception.h>
#include <mb_worker.h>
#include <gnuradio/omnithread.h>
#include <iostream>
#include <mb_msg_accepter_msgq.h>


static pmt_t s_halt = pmt_intern("%halt");
static pmt_t s_sys_port = pmt_intern("%sys-port");
static pmt_t s_shutdown = pmt_intern("%shutdown");
static pmt_t s_request_shutdown = pmt_intern("%request-shutdown");
static pmt_t s_worker_state_changed = pmt_intern("%worker-state-changed");
static pmt_t s_timeout = pmt_intern("%timeout");
static pmt_t s_request_timeout = pmt_intern("%request-timeout");
static pmt_t s_cancel_timeout = pmt_intern("%cancel-timeout");
static pmt_t s_send_halt = pmt_intern("send-halt");
static pmt_t s_exit_now = pmt_intern("exit-now");

static void
send_sys_msg(mb_msg_queue &msgq, pmt_t signal,
	     pmt_t data = PMT_F, pmt_t metadata = PMT_F,
	     mb_pri_t priority = MB_PRI_BEST)
{
  mb_message_sptr msg = mb_make_message(signal, data, metadata, priority);
  msg->set_port_id(s_sys_port);
  msgq.insert(msg);
}


mb_runtime_thread_per_block::mb_runtime_thread_per_block()
  : d_shutdown_in_progress(false),
    d_shutdown_result(PMT_T)
{
  d_accepter = mb_msg_accepter_sptr(new mb_msg_accepter_msgq(&d_msgq));
}

mb_runtime_thread_per_block::~mb_runtime_thread_per_block()
{
  // FIXME iterate over workers and ensure that they are dead.

  if (!d_workers.empty())
    std::cerr << "\nmb_runtime_thread_per_block: dtor (# workers = "
	      << d_workers.size() << ")\n";
}

void
mb_runtime_thread_per_block::request_shutdown(pmt_t result)
{
  (*accepter())(s_request_shutdown, result, PMT_F, MB_PRI_BEST);
}

bool
mb_runtime_thread_per_block::run(const std::string &instance_name,
				 const std::string &class_name,
				 pmt_t user_arg, pmt_t *result)
{
  if (result)		// set it to something now, in case we throw
    *result = PMT_F;
  
  // reset the shutdown state
  d_shutdown_in_progress = false;
  d_shutdown_result = PMT_T;

  assert(d_workers.empty());

  while (!d_timer_queue.empty())	// ensure timer queue is empty
    d_timer_queue.pop();

  /*
   * Create the top-level component, and recursively all of its
   * subcomponents.
   */
  d_top = create_component(instance_name, class_name, user_arg);

  try {
    run_loop();
  }
  catch (...){
    d_top.reset();
    throw;
  }

  if (result)
    *result = d_shutdown_result;
  
  d_top.reset();
  return true;
}

void
mb_runtime_thread_per_block::run_loop()
{
  while (1){
    mb_message_sptr msg;

    if (d_timer_queue.empty())			  // Any timeouts pending?
      msg = d_msgq.get_highest_pri_msg();	  // Nope.  Block forever.

    else {
      mb_timeout_sptr to = d_timer_queue.top();	  // Yep.  Get earliest timeout.

      // wait for a msg or the timeout...
      msg = d_msgq.get_highest_pri_msg_timedwait(to->d_when);

      if (!msg){		// We timed out.
	d_timer_queue.pop();	// Remove timeout from timer queue.

	// send the %timeout msg
	(*to->d_accepter)(s_timeout, to->d_user_data, to->handle(), MB_PRI_BEST);

	if (to->d_is_periodic){
	  to->d_when = to->d_when + to->d_delta; 	// update time of next firing
	  d_timer_queue.push(to);			// push it back into the queue
	}
	continue;
      }
    }

    pmt_t signal = msg->signal();

    if (pmt_eq(signal, s_worker_state_changed)){	// %worker-state-changed
      omni_mutex_lock l1(d_workers_mutex);
      reap_dead_workers();
      if (d_workers.empty())	// no work left to do...
	return;
    }
    else if (pmt_eq(signal, s_request_shutdown)){	// %request-shutdown
      if (!d_shutdown_in_progress){
	d_shutdown_in_progress = true;
	d_shutdown_result = msg->data();

	// schedule a timeout for ourselves...
	schedule_one_shot_timeout(mb_time::time(0.100), s_send_halt, d_accepter);
	send_all_sys_msg(s_shutdown);
      }
    }
    else if (pmt_eq(signal, s_request_timeout)){	// %request-timeout
      mb_timeout_sptr to =
	boost::any_cast<mb_timeout_sptr>(pmt_any_ref(msg->data()));
      d_timer_queue.push(to);
    }
    else if (pmt_eq(signal, s_cancel_timeout)){		// %cancel-timeout
      d_timer_queue.cancel(msg->data());
    }
    else if (pmt_eq(signal, s_timeout)
	     && pmt_eq(msg->data(), s_send_halt)){	// %timeout, send-halt

      // schedule another timeout for ourselves...
      schedule_one_shot_timeout(mb_time::time(0.100), s_exit_now, d_accepter);
      send_all_sys_msg(s_halt);
    }
    else if (pmt_eq(signal, s_timeout)
	     && pmt_eq(msg->data(), s_exit_now)){	// %timeout, exit-now

      // We only get here if we've sent all workers %shutdown followed
      // by %halt, and one or more of them is still alive.  They must
      // be blocked in the kernel.  FIXME We could add one more step:
      // pthread_kill(...) but for now, we'll just ignore them...
      return;
    }
    else {
      std::cerr << "mb_runtime_thread_per_block: unhandled msg: " << msg << std::endl;
    }
  }
}

void
mb_runtime_thread_per_block::reap_dead_workers()
{
  // Already holding mutex
  // omni_mutex_lock l1(d_workers_mutex);

  for (worker_iter_t wi = d_workers.begin(); wi != d_workers.end(); ){
    bool is_dead;

    // We can't join while holding the worker mutex, since that would
    // attempt to destroy the mutex we're holding (omnithread's join
    // deletes the omni_thread object after the pthread_join
    // completes) Instead, we lock just long enough to figure out if
    // the worker is dead.
    {
      omni_mutex_lock l2((*wi)->d_mutex);
      is_dead = (*wi)->d_state == mb_worker::TS_DEAD;
    }

    if (is_dead){
      if (0)
	std::cerr << "\nruntime: "
		  << "(" << (*wi)->id() << ") "
		  << (*wi)->d_mblock->instance_name() << " is TS_DEAD\n";
      void *ignore;
      (*wi)->join(&ignore);
      wi = d_workers.erase(wi);
      continue;
    }
    ++wi;
  }
}

//
// Create the thread, then create the component in the thread.
// Return a pointer to the created mblock.
//
// Can be invoked from any thread
//
mb_mblock_sptr
mb_runtime_thread_per_block::create_component(const std::string &instance_name,
					      const std::string &class_name,
					      pmt_t user_arg)
{
  mb_mblock_maker_t maker;
  if (!mb_class_registry::lookup_maker(class_name, &maker))
    throw mbe_no_such_class(0, class_name + " (in " + instance_name + ")");

  // FIXME here's where we'd lookup NUMA placement requests & mblock
  // priorities and communicate them to the worker we're creating...

  // Create the worker thread
  mb_worker *w =
    new mb_worker(this, maker, instance_name, user_arg);

  w->start_undetached();  // start it

  // Wait for it to reach TS_RUNNING or TS_DEAD

  bool 				is_dead;
  mb_worker::cause_of_death_t	why_dead;
  {
    omni_mutex_lock l(w->d_mutex);
    while (!(w->d_state == mb_worker::TS_RUNNING
	     || w->d_state == mb_worker::TS_DEAD))
      w->d_state_cond.wait();

    is_dead = w->d_state == mb_worker::TS_DEAD;
    why_dead = w->d_why_dead;
  }

  // If the worker failed to init (constructor or initial_transition
  // raised an exception), reap the worker now and raise an exception.

  if (is_dead && why_dead != mb_worker::RIP_EXIT){

    void *ignore;
    w->join(&ignore);

    // FIXME with some work we ought to be able to propagate the
    // exception from the worker.
    throw mbe_mblock_failed(0, instance_name);
  }

  assert(w->d_mblock);

  // Add w to the vector of workers, and return the mblock.
  {
    omni_mutex_lock l(d_workers_mutex);
    d_workers.push_back(w);
  }

  if (0)
    std::cerr << "\nruntime: created "
	      << "(" << w->id() << ") "
	      << w->d_mblock->instance_name() << "\n";

  return w->d_mblock;
}

void
mb_runtime_thread_per_block::send_all_sys_msg(pmt_t signal,
					      pmt_t data,
					      pmt_t metadata,
					      mb_pri_t priority)
{
  omni_mutex_lock l1(d_workers_mutex);

  for (worker_iter_t wi = d_workers.begin(); wi != d_workers.end(); ++wi){
    send_sys_msg((*wi)->d_mblock->impl()->msgq(),
		 signal, data, metadata, priority);
  }
}

//
// Can be invoked from any thread.
// Sends a message to the runtime.
//
pmt_t
mb_runtime_thread_per_block::schedule_one_shot_timeout
  (const mb_time &abs_time,
   pmt_t user_data,
   mb_msg_accepter_sptr accepter)
{
  mb_timeout_sptr to(new mb_timeout(abs_time, user_data, accepter));
  (*d_accepter)(s_request_timeout, pmt_make_any(to), PMT_F, MB_PRI_BEST);
  return to->handle();
}

//
// Can be invoked from any thread.
// Sends a message to the runtime.
//
pmt_t
mb_runtime_thread_per_block::schedule_periodic_timeout
  (const mb_time &first_abs_time,
   const mb_time &delta_time,
   pmt_t user_data,
   mb_msg_accepter_sptr accepter)
{
  mb_timeout_sptr to(new mb_timeout(first_abs_time, delta_time,
				    user_data, accepter));
  (*d_accepter)(s_request_timeout, pmt_make_any(to), PMT_F, MB_PRI_BEST);
  return to->handle();
}

//
// Can be invoked from any thread.
// Sends a message to the runtime.
//
void
mb_runtime_thread_per_block::cancel_timeout(pmt_t handle)
{
  (*d_accepter)(s_cancel_timeout, handle, PMT_F, MB_PRI_BEST);
}
