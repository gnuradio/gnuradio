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
#ifndef INCLUDED_MB_RUNTIME_NOP_H
#define INCLUDED_MB_RUNTIME_NOP_H

#include <mb_runtime_base.h>

/*!
 * \brief Public constructor (factory) for mb_runtime_nop objects.
 */
mb_runtime_sptr mb_make_runtime_nop();

/*!
 * \brief Concrete runtime that does nothing.  Used only during early QA tests.
 */
class mb_runtime_nop : public mb_runtime_base
{
public:
  mb_runtime_nop();
  ~mb_runtime_nop();

  bool run(const std::string &instance_name,
	   const std::string &class_name,
	   pmt_t user_arg,
	   pmt_t *result);

protected:
  mb_mblock_sptr
  create_component(const std::string &instance_name,
		   const std::string &class_name,
		   pmt_t user_arg);
};

#endif /* INCLUDED_MB_RUNTIME_NOP_H */
