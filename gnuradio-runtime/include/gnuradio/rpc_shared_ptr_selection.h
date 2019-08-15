/* -*- c++ -*- */
/*
 * Copyright 2019 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef RPC_SHARED_PTR_SELECTION_H
#define RPC_SHARED_PTR_SELECTION_H

// select a "shared_ptr" type to use: std:: or boost:: . The selection
// will happen in gnuradio-runtime/lib/controlport/CMakeLists.txt .
// CTRLPORT_USE_STD_SHARED_PTR will be defined here to 0 or 1.

#if CTRLPORT_USE_STD_SHARED_PTR

// c++11 std::shared_ptr
#include <memory>
#define GR_RPC_SHARED_PTR std::shared_ptr

#else /* !CTRLPORT_USE_STD_SHARED_PTR */

// boost::shared_ptr
#include <gnuradio/types.h>
#define GR_RPC_SHARED_PTR boost::shared_ptr

#endif /* CTRLPORT_USE_STD_SHARED_PTR */

#endif /* RPC_SHARED_PTR_SELECTION_H */
