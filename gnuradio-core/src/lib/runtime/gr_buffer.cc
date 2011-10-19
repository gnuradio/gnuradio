/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_buffer.h>
#include <gr_vmcircbuf.h>
#include <gr_math.h>
#include <stdexcept>
#include <iostream>
#include <assert.h>
#include <algorithm>
#include <boost/math/common_factor_rt.hpp>

static long s_buffer_count = 0;		// counts for debugging storage mgmt
static long s_buffer_reader_count = 0;

// ----------------------------------------------------------------------------
//			Notes on storage management
//
// Pretty much all the fundamental classes are now using the
// shared_ptr stuff for automatic reference counting.  To ensure that
// no mistakes are made, we make the constructors for classes private,
// and then provide a free factory function that returns a smart
// pointer to the desired class.
//
// gr_buffer and gr_buffer_reader are no exceptions.  However, they
// both want pointers to each other, and unless we do something, we'll
// never delete any of them because of the circular structure.
// They'll always have a reference count of at least one.  We could
// use boost::weak_ptr's from gr_buffer to gr_buffer_reader but that
// introduces it's own problems.  (gr_buffer_reader's destructor needs
// to call gr_buffer::drop_reader, but has no easy way to get a
// shared_ptr to itself.)
//
// Instead, we solve this problem by having gr_buffer hold a raw
// pointer to gr_buffer_reader in its d_reader vector.
// gr_buffer_reader's destructor calls gr_buffer::drop_reader, so
// we're never left with an dangling pointer.  gr_buffer_reader still
// has a shared_ptr to the buffer ensuring that the buffer doesn't go
// away under it.  However, when the reference count of a
// gr_buffer_reader goes to zero, we can successfully reclaim it.
// ----------------------------------------------------------------------------


/*
 * Compute the minimum number of buffer items that work (i.e.,
 * address space wrap-around works).  To work is to satisfy this
 * contraint for integer buffer_size and k:
 *
 *     type_size * nitems == k * page_size
 */
static long
minimum_buffer_items (long type_size, long page_size)
{
  return page_size / boost::math::gcd (type_size, page_size);
}


gr_buffer::gr_buffer (int nitems, size_t sizeof_item, gr_block_sptr link)
  : d_base (0), d_bufsize (0), d_vmcircbuf (0),
    d_sizeof_item (sizeof_item), d_link(link),
    d_write_index (0), d_abs_write_offset(0), d_done (false),
    d_last_min_items_read(0)
{
  if (!allocate_buffer (nitems, sizeof_item))
    throw std::bad_alloc ();

  s_buffer_count++;
}

gr_buffer_sptr 
gr_make_buffer (int nitems, size_t sizeof_item, gr_block_sptr link)
{
  return gr_buffer_sptr (new gr_buffer (nitems, sizeof_item, link));
}

gr_buffer::~gr_buffer ()
{
  delete d_vmcircbuf;
  assert (d_readers.size() == 0);
  s_buffer_count--;
}

/*!
 * sets d_vmcircbuf, d_base, d_bufsize.
 * returns true iff successful.
 */
bool
gr_buffer::allocate_buffer (int nitems, size_t sizeof_item)
{
  int	orig_nitems = nitems;
  
  // Any buffersize we come up with must be a multiple of min_nitems.

  int granularity = gr_vmcircbuf_sysconfig::granularity ();
  int min_nitems =  minimum_buffer_items (sizeof_item, granularity);

  // Round-up nitems to a multiple of min_nitems.

  if (nitems % min_nitems != 0)
    nitems = ((nitems / min_nitems) + 1) * min_nitems;

  // If we rounded-up a whole bunch, give the user a heads up.
  // This only happens if sizeof_item is not a power of two.

  if (nitems > 2 * orig_nitems && nitems * (int) sizeof_item > granularity){
    std::cerr << "gr_buffer::allocate_buffer: warning: tried to allocate\n"
	      << "   " << orig_nitems << " items of size "
	      << sizeof_item << ". Due to alignment requirements\n"
	      << "   " << nitems << " were allocated.  If this isn't OK, consider padding\n"
	      << "   your structure to a power-of-two bytes.\n"
	      << "   On this platform, our allocation granularity is " << granularity << " bytes.\n";
  }

  d_bufsize = nitems;
  d_vmcircbuf = gr_vmcircbuf_sysconfig::make (d_bufsize * d_sizeof_item);
  if (d_vmcircbuf == 0){
    std::cerr << "gr_buffer::allocate_buffer: failed to allocate buffer of size "
	      << d_bufsize * d_sizeof_item / 1024 << " KB\n";
    return false;
  }

  d_base = (char *) d_vmcircbuf->pointer_to_first_copy ();
  return true;
}


int
gr_buffer::space_available ()
{
  if (d_readers.empty ())
    return d_bufsize - 1;	// See comment below

  else {

    // Find out the maximum amount of data available to our readers

    int	most_data = d_readers[0]->items_available ();
    uint64_t min_items_read = d_readers[0]->nitems_read();
    for (size_t i = 1; i < d_readers.size (); i++) {
      most_data = std::max (most_data, d_readers[i]->items_available ());
      min_items_read = std::min(min_items_read, d_readers[i]->nitems_read());
    }

    if(min_items_read != d_last_min_items_read) {
      prune_tags(d_last_min_items_read);
      d_last_min_items_read = min_items_read;
    }

    // The -1 ensures that the case d_write_index == d_read_index is
    // unambiguous.  It indicates that there is no data for the reader

    return d_bufsize - most_data - 1;
  }
}

void *
gr_buffer::write_pointer ()
{
  return &d_base[d_write_index * d_sizeof_item];
}

void
gr_buffer::update_write_pointer (int nitems)
{
  gruel::scoped_lock guard(*mutex());
  d_write_index = index_add (d_write_index, nitems);
  d_abs_write_offset += nitems;
}

void
gr_buffer::set_done (bool done)
{
  gruel::scoped_lock guard(*mutex());
  d_done = done;
}

gr_buffer_reader_sptr
gr_buffer_add_reader (gr_buffer_sptr buf, int nzero_preload, gr_block_sptr link)
{
  if (nzero_preload < 0)
    throw std::invalid_argument("gr_buffer_add_reader: nzero_preload must be >= 0");

  gr_buffer_reader_sptr r (new gr_buffer_reader (buf,
						 buf->index_sub(buf->d_write_index,
								nzero_preload),
						 link));
  buf->d_readers.push_back (r.get ());

  return r;
}

void
gr_buffer::drop_reader (gr_buffer_reader *reader)
{
  // isn't C++ beautiful...    GAG!

  std::vector<gr_buffer_reader *>::iterator result =
    std::find (d_readers.begin (), d_readers.end (), reader);

  if (result == d_readers.end ())
    throw std::invalid_argument ("gr_buffer::drop_reader");    // we didn't find it...

  d_readers.erase (result);
}

void
gr_buffer::add_item_tag(const gr_tag_t &tag)
{
  gruel::scoped_lock guard(*mutex());
  d_item_tags.push_back(tag);
}

void
gr_buffer::prune_tags(uint64_t max_time)
{
  /* NOTE: this function _should_ lock the mutex before editing
     d_item_tags. In practice, this function is only called at
     runtime by min_available_space in gr_block_executor.cc,
     which locks the mutex itself.
     
     If this function is used elsewhere, remember to lock the
     buffer's mutex al la the scoped_lock line below.
  */
  //gruel::scoped_lock guard(*mutex());
  std::deque<gr_tag_t>::iterator itr = d_item_tags.begin();

  uint64_t item_time;

  // Since tags are not guarenteed to be in any particular order,
  // we need to erase here instead of pop_front. An erase in the
  // middle invalidates all iterators; so this resets the iterator
  // to find more. Mostly, we wil be erasing from the front and
  // therefore lose little time this way.
  while(itr != d_item_tags.end()) {
    item_time = (*itr).offset;
    if(item_time < max_time) {
      d_item_tags.erase(itr);
      itr = d_item_tags.begin();
    }
    else
      itr++;
  }
}

long
gr_buffer_ncurrently_allocated ()
{
  return s_buffer_count;
}

// ----------------------------------------------------------------------------

gr_buffer_reader::gr_buffer_reader(gr_buffer_sptr buffer, unsigned int read_index,
				   gr_block_sptr link)
  : d_buffer(buffer), d_read_index(read_index), d_abs_read_offset(0), d_link(link)
{
  s_buffer_reader_count++;
}

gr_buffer_reader::~gr_buffer_reader ()
{
  d_buffer->drop_reader(this);
  s_buffer_reader_count--;
}
   
int
gr_buffer_reader::items_available () const
{
  return d_buffer->index_sub (d_buffer->d_write_index, d_read_index);
}

const void *
gr_buffer_reader::read_pointer ()
{
  return &d_buffer->d_base[d_read_index * d_buffer->d_sizeof_item];
}

void
gr_buffer_reader::update_read_pointer (int nitems)
{
  gruel::scoped_lock guard(*mutex());
  d_read_index = d_buffer->index_add (d_read_index, nitems);
  d_abs_read_offset += nitems;
}

void
gr_buffer_reader::get_tags_in_range(std::vector<gr_tag_t> &v,
				    uint64_t abs_start,
				    uint64_t abs_end)
{
  gruel::scoped_lock guard(*mutex());

  v.resize(0);
  std::deque<gr_tag_t>::iterator itr = d_buffer->get_tags_begin();
  
  uint64_t item_time;
  while(itr != d_buffer->get_tags_end()) {
    item_time = (*itr).offset;

    if((item_time >= abs_start) && (item_time < abs_end)) {
      v.push_back(*itr);
    }

    itr++;
  }
}

long
gr_buffer_reader_ncurrently_allocated ()
{
  return s_buffer_reader_count;
}
