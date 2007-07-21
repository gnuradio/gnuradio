/* -*- c++ -*- */
/*
 * Copyright 2003 Free Software Foundation, Inc.
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

#include <gr_vmcircbuf.h>
#include <stdio.h>

int 
main (int argc, char **argv)
{
  int	verbose = 1;		// summary

  if (argc > 1)
    verbose = 2;		// extra chatty

  bool ok = gr_vmcircbuf_sysconfig::test_all_factories (verbose);

  if (ok){
    fprintf (stdout, "test_vmcircbuf: OK.  We've got at least one workable solution\n");
    return 0;
  }
  else {
    fprintf (stdout, "test_vmcircbuf: NOT OK.  We don't have a workable solution\n");
    return 1;
  }
}
