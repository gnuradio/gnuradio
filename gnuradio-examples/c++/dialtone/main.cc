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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

// GNU Radio C++ application
//
// Instantiate a top block
// Instantiate a runtime, passing it the top block
// Tell the runtime to go...

#include <dialtone.h>
#include <gr_runtime.h>

int main()
{
    dialtone_sptr top_block = make_dialtone();
    gr_runtime_sptr runtime = gr_make_runtime(top_block);

    runtime->run();   
    return 0;
}
