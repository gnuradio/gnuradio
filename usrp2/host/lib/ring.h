/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
#ifndef INCLUDED_RING_H
#define INCLUDED_RING_H

#include <stddef.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <gruel/thread.h>
#include <vrt/expanded_header.h>
#include "sbuff.h"

namespace usrp2 {

  /* container class for data held by the ring */
  class ring_data
  {
    public:
        sbuff::sptr sb;
        vrt::expanded_header hdr;
        const uint32_t *payload;
        size_t n32_bit_words_payload;
  };

  class ring
  {
  public:
    typedef boost::shared_ptr<ring> sptr;

  private:
 
    size_t d_max;
    size_t d_read_ind;
    size_t d_write_ind;

    std::vector<ring_data> d_ring;

    gruel::mutex d_mutex;
    gruel::condition_variable d_not_empty;

    void inc_read_ind()
    {
      if (d_read_ind + 1 >= d_max)
	d_read_ind = 0;
      else
	d_read_ind = d_read_ind + 1;
    }

    void inc_write_ind()
    {
      if (d_write_ind + 1 >= d_max)
	d_write_ind = 0;
      else
	d_write_ind = d_write_ind + 1;
    }
        
    bool empty() const { return d_read_ind == d_write_ind; }
    bool full() const { return (d_write_ind+1)%d_max == d_read_ind; }

  public:

    ring(unsigned int entries);

    void wait_for_not_empty();

    bool enqueue(const ring_data &rd);
    bool dequeue(ring_data &rd);
  };

}  // namespace usrp2

#endif /* INCLUDED_RING_H */
