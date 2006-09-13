/*
 * Copyright 2002 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/*
 * This class gathers together all the test cases for the gr
 * directory into a single test suite.  As you create new test cases,
 * add them here.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <qa_runtime.h>
#include <qa_gr_vmcircbuf.h>
#include <qa_gr_io_signature.h>
#include <qa_gr_block.h>
#include <qa_gr_buffer.h>

CppUnit::TestSuite *
qa_runtime::suite ()
{
  CppUnit::TestSuite	*s = new CppUnit::TestSuite ("runtime");

  s->addTest (qa_gr_vmcircbuf::suite ());
  s->addTest (qa_gr_io_signature::suite ());
  s->addTest (qa_gr_block::suite ());
  s->addTest (qa_gr_buffer::suite ());
  
  return s;
}
