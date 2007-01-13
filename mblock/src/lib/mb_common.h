/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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
#ifndef INCLUDED_MB_COMMON_H
#define INCLUDED_MB_COMMON_H

#include <pmt.h>
#include <vector>
#include <stdexcept>
#include <boost/utility.hpp>


typedef unsigned int	mb_pri_t;
static const mb_pri_t	MB_PRI_DEFAULT = 5;


class mb_runtime;
typedef boost::shared_ptr<mb_runtime> mb_runtime_sptr;

class mb_runtime_impl;
typedef boost::shared_ptr<mb_runtime_impl> mb_runtime_impl_sptr;

class mb_mblock;
typedef boost::shared_ptr<mb_mblock> mb_mblock_sptr;

class mb_mblock_impl;
typedef boost::shared_ptr<mb_mblock_impl> mb_mblock_impl_sptr;

class mb_port;
typedef boost::shared_ptr<mb_port> mb_port_sptr;

class mb_port_detail;
typedef boost::shared_ptr<mb_port_detail> mb_port_detail_sptr;


#endif /* INCLUDED_MB_COMMON_H */
