/* -*- c++ -*- */
/*
 * Copyright 2007,2009 Free Software Foundation, Inc.
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
#ifndef INCLUDED_MB_RUNTIME_THREAD_PER_BLOCK_H
#define INCLUDED_MB_RUNTIME_THREAD_PER_BLOCK_H

#include <mb_runtime_base.h>
#include <mb_worker.h>
#include <mblock/msg_queue.h>
#include <mb_timer_queue.h>

/*!
 * \brief Concrete runtime that uses a thread per mblock
 * \internal
 *
 * These are all implementation details.
 */
class mb_runtime_thread_per_block : public mb_runtime_base
{
public:
  omni_mutex		      d_workers_mutex;	// hold while manipulating d_workers
  std::vector<mb_worker*>     d_workers;
  bool			      d_shutdown_in_progress;
  gruel::pmt_t		      d_shutdown_result;
  mb_msg_queue		      d_msgq;
  mb_timer_queue	      d_timer_queue;

  typedef std::vector<mb_worker*>::iterator  worker_iter_t;

  mb_runtime_thread_per_block();
  ~mb_runtime_thread_per_block();

  bool run(const std::string &instance_name,
	   const std::string &class_name,
	   gruel::pmt_t user_arg,
	   gruel::pmt_t *result);

  void request_shutdown(gruel::pmt_t result);

protected:
  mb_mblock_sptr
  create_component(const std::string &instance_name,
		   const std::string &class_name,
		   gruel::pmt_t user_arg);

  gruel::pmt_t
  schedule_one_shot_timeout(const mb_time &abs_time, gruel::pmt_t user_data,
			    mb_msg_accepter_sptr accepter);

  gruel::pmt_t
  schedule_periodic_timeout(const mb_time &first_abs_time,
			    const mb_time &delta_time,
			    gruel::pmt_t user_data,
			    mb_msg_accepter_sptr accepter);
  void
  cancel_timeout(gruel::pmt_t handle);

private:
  void reap_dead_workers();
  void run_loop();

  void send_all_sys_msg(gruel::pmt_t signal, gruel::pmt_t data = gruel::PMT_F,
			gruel::pmt_t metadata = gruel::PMT_F,
			mb_pri_t priority = MB_PRI_BEST);
};

#endif /* INCLUDED_MB_RUNTIME_THREAD_PER_BLOCK_H */
