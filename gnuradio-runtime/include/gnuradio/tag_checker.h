/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_RUNTIME_TAG_CHECKER_H
#define INCLUDED_GR_RUNTIME_TAG_CHECKER_H

#include <vector>
#include <gnuradio/tags.h>

namespace gr {

  class tag_checker
  {
  private:
    std::vector<tag_t> d_tags;
    tag_t d_next_tag;
    bool d_has_next_tag;
    unsigned int d_next_tag_index;

  public:
    tag_checker(std::vector<tag_t> &tags):
      d_has_next_tag(false),
      d_next_tag_index(0)
    {
      d_tags = tags;
      std::sort(d_tags.begin(), d_tags.end(), &gr::tag_t::offset_compare);
      if(d_tags.size() > 0) {
        d_has_next_tag = true;
        d_next_tag = tags[0];
      }
    };

    ~tag_checker() {};

    void get_tags(std::vector<tag_t> &tag_list, unsigned int offset)
    {
      while(d_has_next_tag && (offset >= d_next_tag.offset)) {
        if(offset == d_next_tag.offset) {
          tag_list.push_back(d_next_tag);
        }
        d_next_tag_index += 1;
        if(d_next_tag_index >= d_tags.size()) {
          d_has_next_tag = false;
        }
        else {
          d_next_tag = d_tags[d_next_tag_index];
        }
      }
    };

  };
}

#endif /* INCLUDED_GR_RUNTIME_TAG_CHECKER_H */
