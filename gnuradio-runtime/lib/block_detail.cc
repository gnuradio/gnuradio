/* -*- c++ -*- */
/*
 * Copyright 2004,2009,2010,2013,2014 Free Software Foundation, Inc.
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

// linux specific perf events
#ifdef GR_ENABLE_LINUX_PERF
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <unistd.h>
#include <asm/unistd.h>
#include <errno.h>
#endif

namespace gr {

#ifdef GR_ENABLE_LINUX_PERF
  /* linux specific perf events, but this could act as an OS abstraction */
  long
  perf_event_open(struct perf_event_attr *hw_event,
  int pid, int cpu, int group_fd, unsigned long flags)
  {
    int ret;

    // os-generic calls
    // thread::gr_thread_t pid = 0; //thread::get_current_thread_id();
    // linux specific
    ret = syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
    return ret;
  }
#endif

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
      d_pc_counter(0)
  {
    s_ncurrently_allocated++;
#ifdef GR_ENABLE_LINUX_PERF
    memset(&hw_perf_results, 0, sizeof(struct linux_perf_events));
#endif
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
    if(how_many_items > 0) {
      input(which_input)->update_read_pointer(how_many_items);
    }
  }

  void
  block_detail::consume_each(int how_many_items)
  {
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

#ifdef GR_ENABLE_LINUX_PERF
    struct perf_event_attr hw_events;
    memset(&hw_events, 0, sizeof(struct perf_event_attr));
    hw_events.size = sizeof(struct perf_event_attr);
    hw_events.disabled = 0;
    hw_events.exclude_kernel = 1;
    hw_events.exclude_hv = 1;
    hw_events.read_format = PERF_FORMAT_GROUP;
    _perf_fd.clear();

    hw_events.type = PERF_TYPE_HARDWARE;
    hw_events.config = PERF_COUNT_HW_BRANCH_INSTRUCTIONS;
    _perf_fd.push_back(perf_event_open(&hw_events, 0, -1, -1, 0));
    hw_events.config = PERF_COUNT_HW_BRANCH_MISSES;
    _perf_fd.push_back(perf_event_open(&hw_events, 0, -1, _perf_fd[0], 0));
    hw_events.config = PERF_COUNT_HW_REF_CPU_CYCLES;
    _perf_fd.push_back(perf_event_open(&hw_events, 0, -1, _perf_fd[0], 0));

    hw_events.type = PERF_TYPE_HW_CACHE;
    hw_events.config = PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_ACCESS << 16);
    _perf_fd.push_back(perf_event_open(&hw_events, 0, -1, _perf_fd[0], 0));
    hw_events.config = PERF_COUNT_HW_CACHE_L1D | (PERF_COUNT_HW_CACHE_OP_READ << 8) | (PERF_COUNT_HW_CACHE_RESULT_MISS << 16);
    _perf_fd.push_back(perf_event_open(&hw_events, 0, -1, _perf_fd[0], 0));

    hw_events.type = PERF_TYPE_SOFTWARE;
    hw_events.config = PERF_COUNT_SW_CONTEXT_SWITCHES;
    _perf_fd.push_back(perf_event_open(&hw_events, 0, -1, _perf_fd[0], 0));
    hw_events.config = PERF_COUNT_SW_CPU_MIGRATIONS;
    _perf_fd.push_back(perf_event_open(&hw_events, 0, -1, _perf_fd[0], 0));
    for(size_t ii=0; ii < _perf_fd.size(); ++ii) {
      ioctl(_perf_fd[ii], PERF_EVENT_IOC_RESET, 0);
    }
#endif
  }

  void
  block_detail::stop_perf_counters(int noutput_items, int nproduced)
  {
    d_end_of_work = gr::high_res_timer_now_perfmon();
    gr::high_res_timer_type diff = d_end_of_work - d_start_of_work;
    d_pc_last_time = gr::high_res_timer_now();

#ifdef GR_ENABLE_LINUX_PERF
    // read linux perf events as a group
    ioctl(_perf_fd[0], PERF_EVENT_IOC_DISABLE, 0);
    int read_size = read(_perf_fd[0], &hw_perf_results, sizeof(struct linux_perf_events) );
    if( read_size < sizeof(struct linux_perf_events)  ) {
        fprintf(stderr, "Could not read all linux perf events (read_size %d is less than %d). Actual number of events is %d\n",
                      read_size, sizeof(struct linux_perf_events), hw_perf_results.num_events);
        if(read_size == -1)
            fprintf(stderr, "Reading perf event errno: %s", strerror(errno));
    }
    for(size_t ii=0; ii < _perf_fd.size(); ++ii) {
      close(_perf_fd[ii]);
    }
    d_ins_branch_references   = (float)hw_perf_results.hw_branch_instructions;
    d_ins_branch_mispredicts  = (float)hw_perf_results.hw_branch_misses;
    d_ins_cache_misses        = (float)hw_perf_results.hw_cache_misses;
    d_ins_cache_references    = (float)hw_perf_results.hw_cache_references;
    d_ins_hw_cpu_cycles       = (float)hw_perf_results.hw_cpu_cycles;
    d_ins_sw_context_switches = (float)hw_perf_results.sw_context_switches;
    d_ins_sw_cpu_migrations   = (float)hw_perf_results.sw_cpu_migrations;
#endif

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
      d_total_noutput_items = noutput_items;
      d_var_noutput_items = 0;
      d_pc_start_time = gr::high_res_timer_now();
#ifdef GR_ENABLE_LINUX_PERF
      d_total_branch_references  = d_ins_branch_references;
      d_total_branch_mispredicts = d_ins_branch_mispredicts;
      d_total_cache_misses       = d_ins_cache_misses;
      d_total_cache_references   = d_ins_cache_references;
      d_total_hw_cpu_cycles      = d_ins_hw_cpu_cycles;
      d_total_sw_context_switches= d_ins_sw_context_switches;
      d_total_sw_cpu_migrations  = d_ins_sw_cpu_migrations;
#endif
      for(size_t i=0; i < d_input.size(); i++) {
        gr::thread::scoped_lock guard(*d_input[i]->mutex());
        float pfull = static_cast<float>(d_input[i]->items_available()) /
        static_cast<float>(d_input[i]->max_possible_items_available());
        d_ins_input_buffers_full[i] = pfull;
        d_avg_input_buffers_full[i] = pfull;
        d_var_input_buffers_full[i] = 0;
      }
      for(size_t i=0; i < d_output.size(); i++) {
        gr::thread::scoped_lock guard(*d_output[i]->mutex());
        float pfull = 1.0f - static_cast<float>(d_output[i]->space_available()) /
        static_cast<float>(d_output[i]->bufsize());
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
      d_total_noutput_items += noutput_items;
      d_avg_noutput_items = d_avg_noutput_items + d/d_pc_counter;
      d_var_noutput_items = d_var_noutput_items + d*d;

      for(size_t i=0; i < d_input.size(); i++) {
        gr::thread::scoped_lock guard(*d_input[i]->mutex());
        float pfull = static_cast<float>(d_input[i]->items_available()) /
        static_cast<float>(d_input[i]->max_possible_items_available());

        d = pfull - d_avg_input_buffers_full[i];
        d_ins_input_buffers_full[i] = pfull;
        d_avg_input_buffers_full[i] = d_avg_input_buffers_full[i] + d/d_pc_counter;
        d_var_input_buffers_full[i] = d_var_input_buffers_full[i] + d*d;
      }

      for(size_t i=0; i < d_output.size(); i++) {
        gr::thread::scoped_lock guard(*d_output[i]->mutex());
        float pfull = 1.0f - static_cast<float>(d_output[i]->space_available()) /
          static_cast<float>(d_output[i]->bufsize());

        d = pfull - d_avg_output_buffers_full[i];
        d_ins_output_buffers_full[i] = pfull;
        d_avg_output_buffers_full[i] = d_avg_output_buffers_full[i] + d/d_pc_counter;
        d_var_output_buffers_full[i] = d_var_output_buffers_full[i] + d*d;
      }
#ifdef GR_ENABLE_LINUX_PERF
      d_total_branch_mispredicts += d_ins_branch_mispredicts;
      d_total_branch_references  += d_ins_branch_references;
      d_total_cache_misses       += d_ins_cache_misses;
      d_total_cache_references   += d_ins_cache_references;
      d_total_hw_cpu_cycles      += d_ins_hw_cpu_cycles;
      d_total_sw_context_switches+= d_ins_sw_context_switches;
      d_total_sw_cpu_migrations  += d_ins_sw_cpu_migrations;
#endif
    }

    d_pc_counter++;
  }

  void
  block_detail::reset_perf_counters()
  {
    d_pc_counter = 0;
#ifdef GR_ENABLE_LINUX_PERF
    // don't leave dangling FDs
    for(size_t ii=0; ii < _perf_fd.size(); ++ii) {
      close(_perf_fd[ii]);
    }
#endif
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
  block_detail::pc_throughput_avg()
  {
    float pc_monitor_time = (float)(d_pc_last_time - d_pc_start_time)/(float)gr::high_res_timer_tps();
    return d_total_noutput_items / pc_monitor_time;
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
  block_detail::pc_noutput_items_total()
  {
    return d_total_noutput_items;
  }

#ifdef GR_ENABLE_LINUX_PERF
  float
  block_detail::pc_branch_miss_rate()
  {
    return d_ins_branch_mispredicts / d_ins_branch_references;
  }

  float
  block_detail::pc_branch_miss_rate_avg()
  {
    return d_total_branch_mispredicts / d_total_branch_references;
  }

  float
  block_detail::pc_cache_miss_rate()
  {
    return d_ins_cache_misses / d_ins_cache_references;
  }

  float
  block_detail::pc_cache_miss_rate_avg()
  {
    return d_total_cache_misses / d_total_cache_references;
  }

  float
  block_detail::pc_hw_cpu_cycles()
  {
    return d_ins_hw_cpu_cycles;
  }

  float
  block_detail::pc_hw_cpu_cycles_avg()
  {
    return d_total_hw_cpu_cycles / (d_pc_counter-1);
  }

  float
  block_detail::pc_hw_cpu_cycles_total()
  {
    return d_total_hw_cpu_cycles;
  }

  float
  block_detail::pc_sw_context_switches()
  {
    return d_ins_sw_context_switches;
  }

  float
  block_detail::pc_sw_context_switches_avg()
  {
    return d_total_sw_context_switches / (d_pc_counter-1);
  }

  float
  block_detail::pc_sw_context_switches_total()
  {
    return d_total_sw_context_switches;
  }

  float
  block_detail::pc_sw_cpu_migrations()
  {
    return d_ins_sw_cpu_migrations;
  }

  float
  block_detail::pc_sw_cpu_migrations_avg()
  {
    return d_total_sw_cpu_migrations / (d_pc_counter-1);
  }

  float
  block_detail::pc_sw_cpu_migrations_total()
  {
    return d_total_sw_cpu_migrations;
  }
#else
  // We need stubs to help export via swig when counters are disabled
  float
  block_detail::pc_branch_miss_rate()
  {
    return 0;
  }

  float
  block_detail::pc_branch_miss_rate_avg()
  {
    return 0;
  }

  float
  block_detail::pc_cache_miss_rate()
  {
    return 0;
  }

  float
  block_detail::pc_cache_miss_rate_avg()
  {
    return 0;
  }

  float
  block_detail::pc_hw_cpu_cycles()
  {
    return 0;
  }

  float
  block_detail::pc_hw_cpu_cycles_avg()
  {
    return 0;
  }

  float
  block_detail::pc_hw_cpu_cycles_total()
  {
    return 0;
  }

  float
  block_detail::pc_sw_context_switches()
  {
    return 0;
  }

  float
  block_detail::pc_sw_context_switches_avg()
  {
    return 0;
  }

  float
  block_detail::pc_sw_context_switches_total()
  {
    return 0;
  }

  float
  block_detail::pc_sw_cpu_migrations()
  {
    return 0;
  }

  float
  block_detail::pc_sw_cpu_migrations_avg()
  {
    return 0;
  }

  float
  block_detail::pc_sw_cpu_migrations_total()
  {
    return 0;
  }
#endif

} /* namespace gr */
