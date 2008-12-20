/* -*- c++ -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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

#include <mblock/runtime.h>
#include <iostream>

int
main(int argc, char **argv)
{
  mb_runtime_sptr rt = mb_make_runtime();
  pmt_t result = PMT_NIL;

  long nmsgs =      1000000;
  long batch_size =     100;

  pmt_t arg = pmt_list2(pmt_from_long(nmsgs),	// # of messages to send through pipe
			pmt_from_long(batch_size));

  rt->run("top", "qa_bitset_top", arg, &result);

  if (!pmt_equal(PMT_T, result)){
    std::cerr << "benchmark_send: incorrect result";
    return 1;
  }

  return 0;
}
