/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <qa_gr_io_signature.h>
#include <gr_io_signature.h>

void
qa_gr_io_signature::t0 ()
{
  gr_make_io_signature (1, 1, sizeof (int));
}

void
qa_gr_io_signature::t1 ()
{
  gr_make_io_signature (3, 1, sizeof (int));  // throws std::invalid_argument
}

void
qa_gr_io_signature::t2 ()
{
  gr_io_signature_sptr p =
    gr_make_io_signature (3, gr_io_signature::IO_INFINITE, sizeof (int));

  CPPUNIT_ASSERT_EQUAL (p->min_streams (), 3);
  CPPUNIT_ASSERT_EQUAL (p->sizeof_stream_item (0), sizeof (int));
}

void
qa_gr_io_signature::t3 ()
{
}

