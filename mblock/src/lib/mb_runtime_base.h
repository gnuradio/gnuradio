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

#ifndef INCLUDED_MB_RUNTIME_BASE_H
#define INCLUDED_MB_RUNTIME_BASE_H

#include <mblock/runtime.h>
#include <gnuradio/omnithread.h>
#include <mblock/time.h>

/*
 * \brief This is the runtime class used by the implementation.
 */
class mb_runtime_base : public mb_runtime
{
  omni_mutex		d_brl;	// big runtime lock (avoid using this if possible...)

protected:
  mb_msg_accepter_sptr  d_accepter;

public:

  /*!
   * \brief lock the big runtime lock
   * \internal
   */
  inline void lock() { d_brl.lock(); }

  /*!
   * \brief unlock the big runtime lock
   * \internal
   */
  inline void unlock() { d_brl.unlock(); }

  virtual void request_shutdown(gruel::pmt_t result);

  virtual mb_mblock_sptr
  create_component(const std::string &instance_name,
		   const std::string &class_name,
		   gruel::pmt_t user_arg) = 0;

  virtual gruel::pmt_t
  schedule_one_shot_timeout(const mb_time &abs_time, gruel::pmt_t user_data,
			    mb_msg_accepter_sptr accepter);

  virtual gruel::pmt_t
  schedule_periodic_timeout(const mb_time &first_abs_time,
			    const mb_time &delta_time,
			    gruel::pmt_t user_data,
			    mb_msg_accepter_sptr accepter);
  virtual void
  cancel_timeout(gruel::pmt_t handle);

  mb_msg_accepter_sptr
  accepter() { return d_accepter; }
  
};


#endif /* INCLUDED_MB_RUNTIME_BASE_H */
