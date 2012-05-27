/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <gr_vector_map.h>
#include <gr_io_signature.h>
#include <string.h>

std::vector<int>
get_in_sizeofs(size_t item_size, std::vector<size_t> in_vlens)
{
  std::vector<int> in_sizeofs;
  for(unsigned int i = 0; i < in_vlens.size(); i++) {
    in_sizeofs.push_back(in_vlens[i]*item_size);
  }
  return in_sizeofs;
}

std::vector<int>
get_out_sizeofs(size_t item_size,
		std::vector< std::vector< std::vector<size_t> > > mapping)
{
  std::vector<int> out_sizeofs;
  for(unsigned int i = 0; i < mapping.size(); i++) {
    out_sizeofs.push_back(mapping[i].size()*item_size);
  }
  return out_sizeofs;
} 

gr_vector_map_sptr
gr_make_vector_map (size_t item_size, std::vector<size_t> in_vlens,
		    std::vector< std::vector< std::vector<size_t> > > mapping)
{
  return gnuradio::get_initial_sptr(new gr_vector_map(item_size,
						      in_vlens,
						      mapping));
}

gr_vector_map::gr_vector_map(size_t item_size, std::vector<size_t> in_vlens,
			     std::vector< std::vector< std::vector<size_t> > > mapping)
  : gr_sync_block("vector_map",
		  gr_make_io_signaturev(in_vlens.size(), in_vlens.size(),
					get_in_sizeofs(item_size, in_vlens)),
		  gr_make_io_signaturev(mapping.size(), mapping.size(),
					get_out_sizeofs(item_size, mapping))),
    d_item_size(item_size), d_in_vlens(in_vlens)
{
  set_mapping(mapping);
}

void
gr_vector_map::set_mapping(std::vector< std::vector< std::vector<size_t> > > mapping) {
  // Make sure the contents of the mapping vectors are possible.
  for(unsigned int i=0; i<mapping.size(); i++) {
    for(unsigned int j=0; j<mapping[i].size(); j++) {
      if(mapping[i][j].size() != 2) {
	throw std::runtime_error("Mapping must be of the form (out_mapping_stream1, out_mapping_stream2, ...), where out_mapping_stream1 is of the form (mapping_element1, mapping_element2, ...), where mapping_element1 is of the form (input_stream, input_element).  This error is raised because a mapping_element vector does not contain exactly 2 items.");
      }
      unsigned int s = mapping[i][j][0];
      unsigned int index = mapping[i][j][1];
      if(s >= d_in_vlens.size()) {
	throw std::runtime_error("Stream numbers in mapping must be less than the number of input streams.");
      }
      if((index < 0) || (index >= d_in_vlens[s])) {
	throw std::runtime_error ("Indices in mapping must be greater than 0 and less than the input vector lengths.");
      }
    }
  }
  gruel::scoped_lock guard(d_mutex);
  d_mapping = mapping;
}

int
gr_vector_map::work(int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items)
{
  const char **inv = (const char **) &input_items[0];
  char **outv = (char **) &output_items[0];

  for(unsigned int n = 0; n < (unsigned int)(noutput_items); n++) {
    for(unsigned int i = 0; i < d_mapping.size(); i++) {
      unsigned int out_vlen = d_mapping[i].size();
      for(unsigned int j = 0; j < out_vlen; j++) {
	unsigned int s = d_mapping[i][j][0];
	unsigned int k = d_mapping[i][j][1];
	memcpy(outv[i] + out_vlen*d_item_size*n +
	       d_item_size*j, inv[s] + d_in_vlens[s]*d_item_size*n +
	       k*d_item_size, d_item_size);
      }
    }
  }

  return noutput_items;
}
