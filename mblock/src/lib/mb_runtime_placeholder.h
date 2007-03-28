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
#ifndef INCLUDED_MB_RUNTIME_PLACEHOLDER_H
#define INCLUDED_MB_RUNTIME_PLACEHOLDER_H

#include <mb_runtime.h>

/*!
 * \brief Concrete runtime that serves as a placeholder until the real
 * runtime is known.
 *
 * The singleton instance of this class is installed in the d_runtime
 * instance variable of each mb_mblock_impl at construction time.
 * Having a valid instance of runtime removes the "pre runtime::run"
 * corner case, and allows us to lock and unlock the big runtime lock
 * even though there's no "real runtime" yet.
 */
class mb_runtime_placeholder : public mb_runtime
{

public:
  mb_runtime_placeholder();
  ~mb_runtime_placeholder();

  //! throws mbe_not_implemented
  bool run(mb_mblock_sptr top);

  //! Return the placeholder singleton
  static mb_runtime *singleton();
};

#endif /* INCLUDED_MB_RUNTIME_PLACEHOLDER_H */
