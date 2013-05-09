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

#include <gr_block_detail.h>
#include <gr_buffer.h>
#include <iostream>

using namespace pmt;

static long s_ncurrently_allocated = 0;

long
gr_block_detail_ncurrently_allocated ()
{
  return s_ncurrently_allocated;
}

gr_block_detail::gr_block_detail (unsigned int ninputs, unsigned int noutputs)
  : d_produce_or(0),
    d_ninputs (ninputs), d_noutputs (noutputs),
    d_input (ninputs), d_output (noutputs),
    d_done (false),
    d_avg_noutput_items(0), 
    d_var_noutput_items(0),
    d_avg_nproduced(0),
    d_var_nproduced(0),
    d_avg_input_buffers_full(ninputs, 0),
    d_var_input_buffers_full(ninputs, 0),
    d_avg_output_buffers_full(noutputs, 0),
    d_var_output_buffers_full(noutputs, 0),
    d_avg_work_time(0),
    d_var_work_time(0),
    d_pc_counter(0)
{
  s_ncurrently_allocated++;
}

gr_block_detail::~gr_block_detail ()
{
  // should take care of itself
  s_ncurrently_allocated--;
}

void
gr_block_detail::set_input (unsigned int which, gr_buffer_reader_sptr reader)
{
  if (which >= d_ninputs)
    throw std::invalid_argument ("gr_block_detail::set_input");

  d_input[which] = reader;
}

void
gr_block_detail::set_output (unsigned int which, gr_buffer_sptr buffer)
{
  if (which >= d_noutputs)
    throw std::invalid_argument ("gr_block_detail::set_output");

  d_output[which] = buffer;
}

gr_block_detail_sptr
gr_make_block_detail (unsigned int ninputs, unsigned int noutputs)
{
  return gr_block_detail_sptr (new gr_block_detail (ninputs, noutputs));
}

void
gr_block_detail::set_done (bool done)
{
  d_done = done;
  for (unsigned int i = 0; i < d_noutputs; i++)
    d_output[i]->set_done (done);

  for (unsigned int i = 0; i < d_ninputs; i++)
    d_input[i]->set_done (done);
}

void
gr_block_detail::consume (int which_input, int how_many_items)
{
  if (how_many_items > 0) {
    input (which_input)->update_read_pointer (how_many_items);
  }
}


void
gr_block_detail::consume_each (int how_many_items)
{
  if (how_many_items > 0) {
    for (int i = 0; i < ninputs (); i++) {
      d_input[i]->update_read_pointer (how_many_items);
    }
  }
}

void
gr_block_detail::produce (int which_output, int how_many_items)
{
  if (how_many_items > 0){
    d_output[which_output]->update_write_pointer (how_many_items);
    d_produce_or |= how_many_items;
  }
}

void
gr_block_detail::produce_each (int how_many_items)
{
  if (how_many_items > 0) {
    for (int i = 0; i < noutputs (); i++) {
      d_output[i]->update_write_pointer (how_many_items);
    }
    d_produce_or |= how_many_items;
  }
}


uint64_t
gr_block_detail::nitems_read(unsigned int which_input)
{
  if(which_input >= d_ninputs)
    throw std::invalid_argument ("gr_block_detail::n_input_items");
  return d_input[which_input]->nitems_read();
}

uint64_t
gr_block_detail::nitems_written(unsigned int which_output)
{
  if(which_output >= d_noutputs)
    throw std::invalid_argument ("gr_block_detail::n_output_items");
  return d_output[which_output]->nitems_written();
}

void
gr_block_detail::add_item_tag(unsigned int which_output, const gr_tag_t &tag)
{
  if(!pmt_is_symbol(tag.key)) {
    throw pmt_wrong_type("gr_block_detail::add_item_tag key", tag.key);
  }
  else {
    // Add tag to gr_buffer's deque tags
    d_output[which_output]->add_item_tag(tag);
  }
}

void
gr_block_detail::remove_item_tag(unsigned int which_input, const gr_tag_t &tag)
{
  if(!pmt_is_symbol(tag.key)) {
    throw pmt_wrong_type("gr_block_detail::add_item_tag key", tag.key);
  }
  else {
    // Add tag to gr_buffer's deque tags
    d_input[which_input]->buffer()->remove_item_tag(tag);
  }
}

void
gr_block_detail::get_tags_in_range(std::vector<gr_tag_t> &v,
				   unsigned int which_input,
				   uint64_t abs_start,
				   uint64_t abs_end)
{
  // get from gr_buffer_reader's deque of tags
  d_input[which_input]->get_tags_in_range(v, abs_start, abs_end);
}

void
gr_block_detail::get_tags_in_range(std::vector<gr_tag_t> &v,
				   unsigned int which_input,
				   uint64_t abs_start,
				   uint64_t abs_end,
				   const pmt_t &key)
{
  std::vector<gr_tag_t> found_items;

  v.resize(0);

  // get from gr_buffer_reader's deque of tags
  d_input[which_input]->get_tags_in_range(found_items, abs_start, abs_end);

  // Filter further by key name
  pmt_t itemkey;
  std::vector<gr_tag_t>::iterator itr;
  for(itr = found_items.begin(); itr != found_items.end(); itr++) {
    itemkey = (*itr).key;
    if(pmt_eqv(key, itemkey)) {
      v.push_back(*itr);
    }
  }
}

void
gr_block_detail::set_processor_affinity(const std::vector<int> &mask)
{
  if(threaded) {
    try {
      gruel::thread_bind_to_processor(thread, mask);
    }
    catch (std::runtime_error e) {
      std::cerr << "set_processor_affinity: invalid mask."  << std::endl;;
    }
  }
}

void
gr_block_detail::unset_processor_affinity()
{
  if(threaded) {
    gruel::thread_unbind(thread);
  }
}

void
gr_block_detail::start_perf_counters()
{
  d_start_of_work = gruel::high_res_timer_now();
}

void
gr_block_detail::stop_perf_counters(int noutput_items, int nproduced)
{
  d_end_of_work = gruel::high_res_timer_now();
  gruel::high_res_timer_type diff = d_end_of_work - d_start_of_work;

  if(d_pc_counter == 0) {
    d_avg_work_time = diff;
    d_var_work_time = 0;
    d_avg_nproduced = nproduced;
    d_var_nproduced = 0;
    d_avg_noutput_items = noutput_items;
    d_var_noutput_items = 0;
    for(size_t i=0; i < d_input.size(); i++) {
      float pfull = static_cast<float>(d_input[i]->items_available()) /
	static_cast<float>(d_input[i]->max_possible_items_available());
      d_avg_input_buffers_full[i] = pfull;
      d_var_input_buffers_full[i] = 0;
    }
    for(size_t i=0; i < d_output.size(); i++) {
      gruel::scoped_lock guard(*d_output[i]->mutex());
      float pfull = 1.0f - static_cast<float>(d_output[i]->space_available()) /
	static_cast<float>(d_output[i]->bufsize());
      d_avg_output_buffers_full[i] = pfull;
      d_var_output_buffers_full[i] = 0;
    }
  }
  else {
    float d = diff - d_avg_work_time;
    d_avg_work_time = d_avg_work_time + d/d_pc_counter;
    d_var_work_time = d_var_work_time + d*d;

    d = nproduced - d_avg_nproduced;
    d_avg_nproduced = d_avg_nproduced +	d/d_pc_counter;
    d_var_nproduced = d_var_nproduced +	d*d;

    d = noutput_items - d_avg_noutput_items;
    d_avg_noutput_items = d_avg_noutput_items + d/d_pc_counter;
    d_var_noutput_items = d_var_noutput_items + d*d;

    for(size_t i=0; i < d_input.size(); i++) {
      float pfull = static_cast<float>(d_input[i]->items_available()) /
	static_cast<float>(d_input[i]->max_possible_items_available());
      
      d = pfull - d_avg_input_buffers_full[i];
      d_avg_input_buffers_full[i] = d_avg_input_buffers_full[i] + d/d_pc_counter;
      d_var_input_buffers_full[i] = d_var_input_buffers_full[i] + d*d;
    }

    for(size_t i=0; i < d_output.size(); i++) {
      gruel::scoped_lock guard(*d_output[i]->mutex());
      float pfull = 1.0f - static_cast<float>(d_output[i]->space_available()) /
	static_cast<float>(d_output[i]->bufsize());

      d = pfull - d_avg_output_buffers_full[i];
      d_avg_output_buffers_full[i] = d_avg_output_buffers_full[i] + d/d_pc_counter;
      d_var_output_buffers_full[i] = d_var_output_buffers_full[i] + d*d;
    }
  }

  d_pc_counter++;
}

void
gr_block_detail::reset_perf_counters()
{
  d_pc_counter = 0;
}

float
gr_block_detail::pc_noutput_items()
{
  return d_avg_noutput_items;
}

float
gr_block_detail::pc_nproduced()
{
  return d_avg_nproduced;
}

float
gr_block_detail::pc_input_buffers_full(size_t which)
{
  if(which < d_avg_input_buffers_full.size())
    return d_avg_input_buffers_full[which];
  else
    return 0;
}

std::vector<float>
gr_block_detail::pc_input_buffers_full()
{
  return d_avg_input_buffers_full;
}

float
gr_block_detail::pc_output_buffers_full(size_t which)
{
  if(which < d_avg_output_buffers_full.size())
    return d_avg_output_buffers_full[which];
  else
    return 0;
}

std::vector<float>
gr_block_detail::pc_output_buffers_full()
{
  return d_avg_output_buffers_full;
}

float
gr_block_detail::pc_work_time()
{
  return d_avg_work_time;
}


float
gr_block_detail::pc_noutput_items_var()
{
  return d_var_noutput_items/(d_pc_counter-1);
}

float
gr_block_detail::pc_nproduced_var()
{
  return d_var_nproduced/(d_pc_counter-1);
}

float
gr_block_detail::pc_input_buffers_full_var(size_t which)
{
  if(which < d_avg_input_buffers_full.size())
    return d_var_input_buffers_full[which]/(d_pc_counter-1);
  else
    return 0;
}

std::vector<float>
gr_block_detail::pc_input_buffers_full_var()
{
  std::vector<float> var(d_avg_input_buffers_full.size(), 0);
  for(size_t i = 0; i < d_avg_input_buffers_full.size(); i++)
    var[i] = d_avg_input_buffers_full[i]/(d_pc_counter-1);
  return var;
}

float
gr_block_detail::pc_output_buffers_full_var(size_t which)
{
  if(which < d_avg_output_buffers_full.size())
    return d_var_output_buffers_full[which]/(d_pc_counter-1);
  else
    return 0;
}

std::vector<float>
gr_block_detail::pc_output_buffers_full_var()
{
  std::vector<float> var(d_avg_output_buffers_full.size(), 0);
  for(size_t i = 0; i < d_avg_output_buffers_full.size(); i++)
    var[i] = d_avg_output_buffers_full[i]/(d_pc_counter-1);
  return var;
}

float
gr_block_detail::pc_work_time_var()
{
  return d_var_work_time/(d_pc_counter-1);
}
