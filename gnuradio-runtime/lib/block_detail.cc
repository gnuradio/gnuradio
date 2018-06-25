/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2013 Free Software Foundation, Inc.
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

#include <gnuradio/block_detail.h>
#include <gnuradio/buffer.h>
#include <iostream>

namespace gr {

  static long s_ncurrently_allocated = 0;

  long
  block_detail_ncurrently_allocated()
  {
    return s_ncurrently_allocated;
  }

  block_detail::block_detail(unsigned int ninputs, unsigned int noutputs)
    : d_produce_or(0),
      d_ninputs(ninputs), d_noutputs(noutputs),
      d_input(ninputs), d_output(noutputs),
      d_done(false),
      d_ins_noutput_items(0),
      d_avg_noutput_items(0),
      d_var_noutput_items(0),
      d_total_noutput_items(0),
      d_ins_nproduced(0),
      d_avg_nproduced(0),
      d_var_nproduced(0),
      d_ins_input_buffers_full(ninputs, 0),
      d_avg_input_buffers_full(ninputs, 0),
      d_var_input_buffers_full(ninputs, 0),
      d_ins_output_buffers_full(noutputs, 0),
      d_avg_output_buffers_full(noutputs, 0),
      d_var_output_buffers_full(noutputs, 0),
      d_ins_work_time(0),
      d_avg_work_time(0),
      d_var_work_time(0),
      d_avg_throughput(0),
      d_pc_counter(0)
  {
    s_ncurrently_allocated++;
    d_pc_start_time = gr::high_res_timer_now();
  }

  block_detail::~block_detail()
  {
    // should take care of itself
    s_ncurrently_allocated--;
  }

  void
  block_detail::set_input(unsigned int which, buffer_reader_sptr reader)
  {
    if(which >= d_ninputs)
      throw std::invalid_argument("block_detail::set_input");

    d_input[which] = reader;
  }

  void
  block_detail::set_output(unsigned int which, buffer_sptr buffer)
  {
    if(which >= d_noutputs)
      throw std::invalid_argument("block_detail::set_output");

    d_output[which] = buffer;
  }

  block_detail_sptr
  make_block_detail(unsigned int ninputs, unsigned int noutputs)
  {
    return block_detail_sptr (new block_detail(ninputs, noutputs));
  }

  void
  block_detail::set_done(bool done)
  {
    d_done = done;
    for(unsigned int i = 0; i < d_noutputs; i++)
      d_output[i]->set_done(done);

    for(unsigned int i = 0; i < d_ninputs; i++)
      d_input[i]->set_done(done);
  }

  void
  block_detail::consume(int which_input, int how_many_items)
  {
    d_consumed = how_many_items;
    if(how_many_items > 0) {
      input(which_input)->update_read_pointer(how_many_items);
    }
  }

  int
  block_detail::consumed() const
  {
    return d_consumed;
  }

  void
  block_detail::consume_each(int how_many_items)
  {
    d_consumed = how_many_items;
    if(how_many_items > 0) {
      for(int i = 0; i < ninputs (); i++) {
        d_input[i]->update_read_pointer(how_many_items);
      }
    }
  }

  void
  block_detail::produce(int which_output, int how_many_items)
  {
    if(how_many_items > 0) {
      d_output[which_output]->update_write_pointer(how_many_items);
      d_produce_or |= how_many_items;
    }
  }

  void
  block_detail::produce_each(int how_many_items)
  {
    if(how_many_items > 0) {
      for(int i = 0; i < noutputs (); i++) {
        d_output[i]->update_write_pointer (how_many_items);
      }
      d_produce_or |= how_many_items;
    }
  }

  uint64_t
  block_detail::nitems_read(unsigned int which_input)
  {
    if(which_input >= d_ninputs)
      throw std::invalid_argument ("block_detail::n_input_items");
    return d_input[which_input]->nitems_read();
  }

  uint64_t
  block_detail::nitems_written(unsigned int which_output)
  {
    if(which_output >= d_noutputs)
      throw std::invalid_argument ("block_detail::n_output_items");
    return d_output[which_output]->nitems_written();
  }

  void
  block_detail::reset_nitem_counters()
  {
    for(unsigned int i = 0; i < d_ninputs; i++) {
      d_input[i]->reset_nitem_counter();
    }
    for(unsigned int o = 0; o < d_noutputs; o++) {
      d_output[o]->reset_nitem_counter();
    }
  }

  void
  block_detail::clear_tags()
  {
    for(unsigned int i = 0; i < d_ninputs; i++) {
      uint64_t max_time = 0xFFFFFFFFFFFFFFFF; // from now to the end of time
      d_input[i]->buffer()->prune_tags(max_time);
    }
  }

  void
  block_detail::add_item_tag(unsigned int which_output, const tag_t &tag)
  {
    if(!pmt::is_symbol(tag.key)) {
      throw pmt::wrong_type("block_detail::add_item_tag key", tag.key);
    }
    else {
      // Add tag to gr_buffer's deque tags
      d_output[which_output]->add_item_tag(tag);
    }
  }

  void
  block_detail::remove_item_tag(unsigned int which_input, const tag_t &tag, long id)
  {
    if(!pmt::is_symbol(tag.key)) {
      throw pmt::wrong_type("block_detail::add_item_tag key", tag.key);
    }
    else {
      // Add tag to gr_buffer's deque tags
      d_input[which_input]->buffer()->remove_item_tag(tag, id);
    }
  }

  void
  block_detail::get_tags_in_range(std::vector<tag_t> &v,
                                  unsigned int which_input,
                                  uint64_t abs_start,
                                  uint64_t abs_end,
				  long id)
  {
    // get from gr_buffer_reader's deque of tags
    d_input[which_input]->get_tags_in_range(v, abs_start, abs_end, id);
   }

  void
  block_detail::get_tags_in_range(std::vector<tag_t> &v,
                                  unsigned int which_input,
                                  uint64_t abs_start,
                                  uint64_t abs_end,
                                  const pmt::pmt_t &key,
				  long id)
  {
    std::vector<tag_t> found_items;

    v.resize(0);

    // get from gr_buffer_reader's deque of tags
    d_input[which_input]->get_tags_in_range(found_items, abs_start, abs_end, id);

    // Filter further by key name
    pmt::pmt_t itemkey;
    std::vector<tag_t>::iterator itr;
    for(itr = found_items.begin(); itr != found_items.end(); itr++) {
      itemkey = (*itr).key;
      if(pmt::eqv(key, itemkey)) {
        v.push_back(*itr);
      }
    }
  }

  void
  block_detail::set_processor_affinity(const std::vector<int> &mask)
  {
    if(threaded) {
      try {
        gr::thread::thread_bind_to_processor(thread, mask);
      }
      catch (std::runtime_error e) {
        std::cerr << "set_processor_affinity: invalid mask."  << std::endl;;
      }
    }
  }

  void
  block_detail::unset_processor_affinity()
  {
    if(threaded) {
      gr::thread::thread_unbind(thread);
    }
  }

  int
  block_detail::thread_priority(){
    if(threaded) {
      return gr::thread::thread_priority(thread);
    }
    return -1;
  }

  int
  block_detail::set_thread_priority(int priority){
    if(threaded) {
      return gr::thread::set_thread_priority(thread,priority);
    }
    return -1;
  }

  void
  block_detail::start_perf_counters()
  {
    d_start_of_work = gr::high_res_timer_now_perfmon();
  }

  void
  block_detail::stop_perf_counters(int noutput_items, int nproduced)
  {
    d_end_of_work = gr::high_res_timer_now_perfmon();
    gr::high_res_timer_type diff = d_end_of_work - d_start_of_work;

    if(d_pc_counter == 0) {
      d_ins_work_time = diff;
      d_avg_work_time = diff;
      d_var_work_time = 0;
      d_total_work_time = diff;
      d_ins_nproduced = nproduced;
      d_avg_nproduced = nproduced;
      d_var_nproduced = 0;
      d_ins_noutput_items = noutput_items;
      d_avg_noutput_items = noutput_items;
      d_var_noutput_items = 0;
      d_total_noutput_items = noutput_items;
      d_pc_start_time = (float)gr::high_res_timer_now();
      for(size_t i=0; i < d_input.size(); i++) {
        buffer_reader_sptr in_buf = d_input[i];
        gr::thread::scoped_lock guard(*in_buf->mutex());
        float pfull = static_cast<float>(in_buf->items_available()) /
          static_cast<float>(in_buf->max_possible_items_available());
        d_ins_input_buffers_full[i] = pfull;
        d_avg_input_buffers_full[i] = pfull;
        d_var_input_buffers_full[i] = 0;
      }
      for(size_t i=0; i < d_output.size(); i++) {
        buffer_sptr out_buf = d_output[i];
        gr::thread::scoped_lock guard(*out_buf->mutex());
        float pfull = 1.0f - static_cast<float>(out_buf->space_available()) /
          static_cast<float>(out_buf->bufsize());
        d_ins_output_buffers_full[i] = pfull;
        d_avg_output_buffers_full[i] = pfull;
        d_var_output_buffers_full[i] = 0;
      }
    }
    else {
      float d = diff - d_avg_work_time;
      d_ins_work_time = diff;
      d_avg_work_time = d_avg_work_time + d/d_pc_counter;
      d_var_work_time = d_var_work_time + d*d;
      d_total_work_time += diff;

      d = nproduced - d_avg_nproduced;
      d_ins_nproduced = nproduced;
      d_avg_nproduced = d_avg_nproduced + d/d_pc_counter;
      d_var_nproduced = d_var_nproduced + d*d;

      d = noutput_items - d_avg_noutput_items;
      d_ins_noutput_items = noutput_items;
      d_avg_noutput_items = d_avg_noutput_items + d/d_pc_counter;
      d_var_noutput_items = d_var_noutput_items + d*d;
      d_total_noutput_items += noutput_items;
      d_pc_last_work_time = gr::high_res_timer_now();
      float monitor_time = (float)(d_pc_last_work_time - d_pc_start_time) / (float)gr::high_res_timer_tps();
      d_avg_throughput = d_total_noutput_items / monitor_time;

      for(size_t i=0; i < d_input.size(); i++) {
        buffer_reader_sptr in_buf = d_input[i];
        gr::thread::scoped_lock guard(*in_buf->mutex());
        float pfull = static_cast<float>(in_buf->items_available()) /
          static_cast<float>(in_buf->max_possible_items_available());

        d = pfull - d_avg_input_buffers_full[i];
        d_ins_input_buffers_full[i] = pfull;
        d_avg_input_buffers_full[i] = d_avg_input_buffers_full[i] + d/d_pc_counter;
        d_var_input_buffers_full[i] = d_var_input_buffers_full[i] + d*d;
      }

      for(size_t i=0; i < d_output.size(); i++) {
        buffer_sptr out_buf = d_output[i];
        gr::thread::scoped_lock guard(*out_buf->mutex());
        float pfull = 1.0f - static_cast<float>(out_buf->space_available()) /
          static_cast<float>(out_buf->bufsize());

        d = pfull - d_avg_output_buffers_full[i];
        d_ins_output_buffers_full[i] = pfull;
        d_avg_output_buffers_full[i] = d_avg_output_buffers_full[i] + d/d_pc_counter;
        d_var_output_buffers_full[i] = d_var_output_buffers_full[i] + d*d;
      }
    }

    d_pc_counter++;
  }

  void
  block_detail::reset_perf_counters()
  {
    d_pc_counter = 0;
  }

  float
  block_detail::pc_noutput_items()
  {
    return d_ins_noutput_items;
  }

  float
  block_detail::pc_nproduced()
  {
    return d_ins_nproduced;
  }

  float
  block_detail::pc_input_buffers_full(size_t which)
  {
    if(which < d_ins_input_buffers_full.size())
      return d_ins_input_buffers_full[which];
    else
      return 0;
  }

  std::vector<float>
  block_detail::pc_input_buffers_full()
  {
    return d_ins_input_buffers_full;
  }

  float
  block_detail::pc_output_buffers_full(size_t which)
  {
    if(which < d_ins_output_buffers_full.size())
      return d_ins_output_buffers_full[which];
    else
      return 0;
  }

  std::vector<float>
  block_detail::pc_output_buffers_full()
  {
    return d_ins_output_buffers_full;
  }

  float
  block_detail::pc_work_time()
  {
    return d_ins_work_time;
  }

  float
  block_detail::pc_noutput_items_avg()
  {
    return d_avg_noutput_items;
  }

  float
  block_detail::pc_nproduced_avg()
  {
    return d_avg_nproduced;
  }

  float
  block_detail::pc_input_buffers_full_avg(size_t which)
  {
    if(which < d_avg_input_buffers_full.size())
      return d_avg_input_buffers_full[which];
    else
      return 0;
  }

  std::vector<float>
  block_detail::pc_input_buffers_full_avg()
  {
    return d_avg_input_buffers_full;
  }

  float
  block_detail::pc_output_buffers_full_avg(size_t which)
  {
    if(which < d_avg_output_buffers_full.size())
      return d_avg_output_buffers_full[which];
    else
      return 0;
  }

  std::vector<float>
  block_detail::pc_output_buffers_full_avg()
  {
    return d_avg_output_buffers_full;
  }

  float
  block_detail::pc_work_time_avg()
  {
    return d_avg_work_time;
  }

  float
  block_detail::pc_noutput_items_var()
  {
    return d_var_noutput_items/(d_pc_counter-1);
  }

  float
  block_detail::pc_nproduced_var()
  {
    return d_var_nproduced/(d_pc_counter-1);
  }

  float
  block_detail::pc_input_buffers_full_var(size_t which)
  {
    if(which < d_avg_input_buffers_full.size())
      return d_var_input_buffers_full[which]/(d_pc_counter-1);
    else
      return 0;
  }

  std::vector<float>
  block_detail::pc_input_buffers_full_var()
  {
    std::vector<float> var(d_avg_input_buffers_full.size(), 0);
    for(size_t i = 0; i < d_avg_input_buffers_full.size(); i++)
      var[i] = d_avg_input_buffers_full[i]/(d_pc_counter-1);
    return var;
  }

  float
  block_detail::pc_output_buffers_full_var(size_t which)
  {
    if(which < d_avg_output_buffers_full.size())
      return d_var_output_buffers_full[which]/(d_pc_counter-1);
    else
      return 0;
  }

  std::vector<float>
  block_detail::pc_output_buffers_full_var()
  {
    std::vector<float> var(d_avg_output_buffers_full.size(), 0);
    for(size_t i = 0; i < d_avg_output_buffers_full.size(); i++)
      var[i] = d_avg_output_buffers_full[i]/(d_pc_counter-1);
    return var;
  }

  float
  block_detail::pc_work_time_var()
  {
    return d_var_work_time/(d_pc_counter-1);
  }

  float
  block_detail::pc_work_time_total()
  {
    return d_total_work_time;
  }

  float
  block_detail::pc_throughput_avg() {
    return d_avg_throughput;
  }
} /* namespace gr */
