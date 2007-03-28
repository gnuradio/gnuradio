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
#include <mb_runtime_nop.h>
#include <mb_mblock.h>

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
mb_runtime_nop::run(mb_mblock_sptr top)
{
  class initial_visitor : public mb_visitor
  {
  public:
    bool operator()(mb_mblock *mblock, const std::string &path)
    {
      mblock->set_instance_name(path);
      mblock->init_fsm();
      return true;
    }
  };

  initial_visitor	visitor;

  top->walk_tree(&visitor);

  return true;
}
