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
#include <mb_runtime_nop.h>
#include <mb_mblock.h>
#include <mb_class_registry.h>
#include <mb_exception.h>

mb_runtime_sptr 
mb_make_runtime_nop()
{
  return mb_runtime_sptr(new mb_runtime_nop());
}


mb_runtime_nop::mb_runtime_nop()
{
  // nop for now
}

mb_runtime_nop::~mb_runtime_nop()
{
  // nop for now
}


bool
mb_runtime_nop::run(const std::string &instance_name,
		    const std::string &class_name,
		    pmt_t user_arg, pmt_t *result)
{
  class initial_visitor : public mb_visitor
  {
  public:
    bool operator()(mb_mblock *mblock)
    {
      mblock->initial_transition();
      return true;
    }
  };

  initial_visitor visitor;

  if (result)
    *result = PMT_T;

  d_top = create_component(instance_name, class_name, user_arg);
  d_top->walk_tree(&visitor);

  return true;
}

mb_mblock_sptr
mb_runtime_nop::create_component(const std::string &instance_name,
				 const std::string &class_name,
				 pmt_t user_arg)
{
  mb_mblock_maker_t maker;
  if (!mb_class_registry::lookup_maker(class_name, &maker))
    throw mbe_no_such_class(0, class_name + " (in " + instance_name + ")");

  return maker(this, instance_name, user_arg);
}
