/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2008 Free Software Foundation, Inc.
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
#ifndef INCLUDED_MB_COMMON_H
#define INCLUDED_MB_COMMON_H

#include <pmt.h>
#include <vector>
#include <stdexcept>
#include <boost/utility.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/weak_ptr.hpp>

/*
 * The priority type and valid range
 */
typedef unsigned int	mb_pri_t;
static const mb_pri_t	MB_PRI_BEST    = 0;
static const mb_pri_t	MB_PRI_DEFAULT = 4; 
static const mb_pri_t   MB_PRI_WORST   = 7;
static const mb_pri_t	MB_NPRI = MB_PRI_WORST + 1;	  // number of valid priorities 

/*!
 * \brief return true iff priority a is better than priority b
 */
inline static bool
mb_pri_better(mb_pri_t a, mb_pri_t b)
{
  return a < b;
}

/*!
 * \brief return true iff priority a is worse than priority b
 */
inline static bool
mb_pri_worse(mb_pri_t a, mb_pri_t b)
{
  return a > b;
}

/*!
 * \brief ensure that pri is valid
 */
inline static mb_pri_t
mb_pri_clamp(mb_pri_t p)
{
  return p < MB_NPRI ? p : MB_NPRI - 1;
}

class mb_runtime;
typedef boost::shared_ptr<mb_runtime> mb_runtime_sptr;

//class mb_runtime_impl;
//typedef boost::shared_ptr<mb_runtime_impl> mb_runtime_impl_sptr;

class mb_mblock;
typedef boost::shared_ptr<mb_mblock> mb_mblock_sptr;

class mb_mblock_impl;
typedef boost::shared_ptr<mb_mblock_impl> mb_mblock_impl_sptr;

class mb_port;
typedef boost::shared_ptr<mb_port> mb_port_sptr;

//class mb_port_detail;
//typedef boost::shared_ptr<mb_port_detail> mb_port_detail_sptr;

class mb_msg_accepter;
typedef boost::shared_ptr<mb_msg_accepter> mb_msg_accepter_sptr;

class mb_message;
typedef boost::shared_ptr<mb_message> mb_message_sptr;

class mb_msg_queue;
typedef boost::shared_ptr<mb_msg_queue> mb_msg_queue_sptr;

#endif /* INCLUDED_MB_COMMON_H */
