/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <gr_skiphead.h>
#include <gr_io_signature.h>

gr_skiphead::gr_skiphead (size_t sizeof_stream_item, int nitems)
  : gr_sync_block ("skiphead",
       gr_make_io_signature (1, 1, sizeof_stream_item),
       gr_make_io_signature (1, 1, sizeof_stream_item)),
    d_nitems (nitems), d_nskipped_items (0)
{
}

gr_block_sptr
gr_make_skiphead (size_t sizeof_stream_item, int nitems)
{
  return gr_block_sptr (new gr_skiphead (sizeof_stream_item, nitems));
}

int
gr_skiphead::work (int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
  int items_to_skip = d_nitems - d_nskipped_items;
  if (items_to_skip <=0)
  {
     //Done with skipping, copy all input to the output;
     memcpy (output_items[0], input_items[0], noutput_items * input_signature()->sizeof_stream_item (0));
     return noutput_items;
  } else if (items_to_skip < noutput_items)
  {
     memcpy (output_items[0], &(((char *)input_items[0])[items_to_skip*input_signature()->sizeof_stream_item (0)]), (noutput_items -items_to_skip) * input_signature()->sizeof_stream_item (0));
    //memcpy (output_items[0], &((input_items[0])[items_to_skip]), (noutput_items -items_to_skip) * input_signature()->sizeof_stream_item (0));
     //memcpy (output_items[0], input_items[0]+items_to_skip*input_signature()->sizeof_stream_item (0), (noutput_items -items_to_skip) * input_signature()->sizeof_stream_item (0));
     d_nskipped_items += items_to_skip;
     consume_each (items_to_skip);
     return (noutput_items -items_to_skip);
  } else
  {
     d_nskipped_items += noutput_items;
     consume_each (items_to_skip);
     return 0;
  }

  return -1;//Should never get here
}
