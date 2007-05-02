/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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
#include <mb_runtime_base.h>

/*
 * Default nop implementations...
 */

void
mb_runtime_base::request_shutdown(pmt_t result)
{
}

pmt_t
mb_runtime_base::schedule_one_shot_timeout(const mb_time &abs_time,
					   pmt_t user_data,
					   mb_msg_accepter_sptr accepter)
{
  return PMT_F;
}

pmt_t
mb_runtime_base::schedule_periodic_timeout(const mb_time &first_abs_time,
					   const mb_time &delta_time,
					   pmt_t user_data,
					   mb_msg_accepter_sptr accepter)
{
  return PMT_F;
}

void
mb_runtime_base::cancel_timeout(pmt_t handle)
{
}

