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

#include <gnuradio/block.h>
#include <gnuradio/block_registry.h>
#include <gnuradio/block_detail.h>
#include <gnuradio/buffer.h>
#include <gnuradio/prefs.h>
#include <stdexcept>
#include <iostream>

namespace gr {

  block::block(const std::string &name,
               io_signature::sptr input_signature,
               io_signature::sptr output_signature)
    : basic_block(name, input_signature, output_signature),
      d_output_multiple (1),
      d_output_multiple_set(false),
      d_unaligned(0),
      d_is_unaligned(false),
      d_relative_rate (1.0),
      d_history(1),
      d_attr_delay(0),
      d_fixed_rate(false),
      d_max_noutput_items_set(false),
      d_max_noutput_items(0),
      d_min_noutput_items(0),
      d_tag_propagation_policy(TPP_ALL_TO_ALL),
      d_priority(-1),
      d_pc_rpc_set(false),
      d_update_rate(false),
      d_max_output_buffer(std::max(output_signature->max_streams(),1), -1),
      d_min_output_buffer(std::max(output_signature->max_streams(),1), -1),
      d_pmt_done(pmt::intern("done")),
      d_system_port(pmt::intern("system"))
  {
    global_block_registry.register_primitive(alias(), this);
    message_port_register_in(d_system_port);
    set_msg_handler(d_system_port, boost::bind(&block::system_handler, this, _1));

    configure_default_loggers(d_logger, d_debug_logger, symbol_name());
  }

  block::~block()
  {
    global_block_registry.unregister_primitive(symbol_name());
  }

  unsigned
  block::history() const
  {
    return d_history;
  }

  void
  block::set_history(unsigned history)
  {
    d_history = history;
  }

  void
  block::declare_sample_delay(unsigned delay)
  {
    d_attr_delay = delay;
    if(d_detail) {
      unsigned int nins = static_cast<unsigned int>(d_detail->ninputs());
      for(unsigned int n = 0; n < nins; n++) {
        d_detail->input(n)->declare_sample_delay(d_attr_delay);
      }
    }
  }

  void
  block::declare_sample_delay(int which, unsigned delay)
  {
    d_attr_delay = delay;
    if(d_detail) {
      d_detail->input(which)->declare_sample_delay(d_attr_delay);
    }
  }

  unsigned
  block::sample_delay(int which) const
  {
    return d_attr_delay;
  }

  // stub implementation:  1:1

  void
  block::forecast(int noutput_items, gr_vector_int &ninput_items_required)
  {
    unsigned ninputs = ninput_items_required.size ();
    for(unsigned i = 0; i < ninputs; i++)
      ninput_items_required[i] = noutput_items + history() - 1;
  }

  // default implementation

  bool
  block::start()
  {
    return true;
  }

  bool
  block::stop()
  {
    return true;
  }

  void
  block::set_output_multiple(int multiple)
  {
    if(multiple < 1)
      throw std::invalid_argument("block::set_output_multiple");

    d_output_multiple_set = true;
    d_output_multiple = multiple;
  }

  void
  block::set_alignment(int multiple)
  {
    if(multiple < 1)
      throw std::invalid_argument("block::set_alignment_multiple");

    d_output_multiple = multiple;
  }

  void
  block::set_unaligned(int na)
  {
    // unaligned value must be less than 0 and it doesn't make sense
    // that it's larger than the alignment value.
    if((na < 0) || (na > d_output_multiple))
      throw std::invalid_argument("block::set_unaligned");

    d_unaligned = na;
  }

  void
  block::set_is_unaligned(bool u)
  {
    d_is_unaligned = u;
  }

  void
  block::set_relative_rate(double relative_rate)
  {
    if(relative_rate < 0.0)
      throw std::invalid_argument("block::set_relative_rate");

    d_relative_rate = relative_rate;
  }

  void
  block::consume(int which_input, int how_many_items)
  {
    d_detail->consume(which_input, how_many_items);
  }

  void
  block::consume_each(int how_many_items)
  {
    d_detail->consume_each(how_many_items);
  }

  void
  block::produce(int which_output, int how_many_items)
  {
    d_detail->produce(which_output, how_many_items);
  }

  int
  block::fixed_rate_ninput_to_noutput(int ninput)
  {
    throw std::runtime_error("Unimplemented");
  }

  int
  block::fixed_rate_noutput_to_ninput(int noutput)
  {
    throw std::runtime_error("Unimplemented");
  }

  uint64_t
  block::nitems_read(unsigned int which_input)
  {
    if(d_detail) {
      return d_detail->nitems_read(which_input);
    }
    else {
      //throw std::runtime_error("No block_detail associated with block yet");
      return 0;
    }
  }

  uint64_t
  block::nitems_written(unsigned int which_output)
  {
    if(d_detail) {
      return d_detail->nitems_written(which_output);
    }
    else {
      //throw std::runtime_error("No block_detail associated with block yet");
      return 0;
    }
  }

  void
  block::add_item_tag(unsigned int which_output,
                         const tag_t &tag)
  {
    d_detail->add_item_tag(which_output, tag);
  }

  void
  block::remove_item_tag(unsigned int which_input,
                         const tag_t &tag)
  {
    d_detail->remove_item_tag(which_input, tag, unique_id());
  }

  void
  block::get_tags_in_range(std::vector<tag_t> &v,
                           unsigned int which_input,
                           uint64_t start, uint64_t end)
  {
    d_detail->get_tags_in_range(v, which_input, start, end, unique_id());
  }

  void
  block::get_tags_in_range(std::vector<tag_t> &v,
                           unsigned int which_input,
                           uint64_t start, uint64_t end,
                           const pmt::pmt_t &key)
  {
    d_detail->get_tags_in_range(v, which_input, start, end, key, unique_id());
  }

  void
  block::get_tags_in_window(std::vector<tag_t> &v,
                            unsigned int which_input,
                            uint64_t start, uint64_t end)
  {
    d_detail->get_tags_in_range(v, which_input,
                                nitems_read(which_input) + start,
                                nitems_read(which_input) + end,
                                unique_id());
  }

  void
  block::get_tags_in_window(std::vector<tag_t> &v,
                            unsigned int which_input,
                            uint64_t start, uint64_t end,
                            const pmt::pmt_t &key)
  {
    d_detail->get_tags_in_range(v, which_input,
                                nitems_read(which_input) + start,
                                nitems_read(which_input) + end,
                                key, unique_id());
  }

  block::tag_propagation_policy_t
  block::tag_propagation_policy()
  {
    return d_tag_propagation_policy;
  }

  void
  block::set_tag_propagation_policy(tag_propagation_policy_t p)
  {
    d_tag_propagation_policy = p;
  }

  int
  block::max_noutput_items()
  {
    return d_max_noutput_items;
  }

  void
  block::set_max_noutput_items(int m)
  {
    if(m <= 0)
      throw std::runtime_error("block::set_max_noutput_items: value for max_noutput_items must be greater than 0.\n");

    d_max_noutput_items = m;
    d_max_noutput_items_set = true;
  }

  void
  block::unset_max_noutput_items()
  {
    d_max_noutput_items_set = false;
  }

  bool
  block::is_set_max_noutput_items()
  {
    return d_max_noutput_items_set;
  }

  void
  block::set_processor_affinity(const std::vector<int> &mask)
  {
    d_affinity = mask;
    if(d_detail) {
      d_detail->set_processor_affinity(d_affinity);
    }
  }

  void
  block::unset_processor_affinity()
  {
    d_affinity.clear();
    if(d_detail) {
      d_detail->unset_processor_affinity();
    }
  }

  int
  block::active_thread_priority()
  {
    if(d_detail) {
      return d_detail->thread_priority();
    }
    return -1;
  }

  int
  block::thread_priority()
  {
    return d_priority;
  }

  int
  block::set_thread_priority(int priority)
  {
    d_priority = priority;
    if(d_detail) {
      return d_detail->set_thread_priority(priority);
    }
    return d_priority;
  }

  void
  block::expand_minmax_buffer(int port)
  {
    if((size_t)port >= d_max_output_buffer.size())
      set_max_output_buffer(port, -1);
    if((size_t)port >= d_min_output_buffer.size())
      set_min_output_buffer(port, -1);
  }

  long
  block::max_output_buffer(size_t i)
  {
    if(i >= d_max_output_buffer.size())
      throw std::invalid_argument("basic_block::max_output_buffer: port out of range.");
    return d_max_output_buffer[i];
  }

  void
  block::set_max_output_buffer(long max_output_buffer)
  {
    for(int i = 0; i < output_signature()->max_streams(); i++) {
      set_max_output_buffer(i, max_output_buffer);
    }
  }

  void
  block::set_max_output_buffer(int port, long max_output_buffer)
  {
    if((size_t)port >= d_max_output_buffer.size())
      d_max_output_buffer.push_back(max_output_buffer);
    else
      d_max_output_buffer[port] = max_output_buffer;
  }

  long
  block::min_output_buffer(size_t i)
  {
    if(i >= d_min_output_buffer.size())
      throw std::invalid_argument("basic_block::min_output_buffer: port out of range.");
    return d_min_output_buffer[i];
  }

  void
  block::set_min_output_buffer(long min_output_buffer)
  {
    std::cout << "set_min_output_buffer on block " << unique_id() << " to " << min_output_buffer << std::endl;
    for(int i=0; i<output_signature()->max_streams(); i++) {
      set_min_output_buffer(i, min_output_buffer);
    }
  }

  void
  block::set_min_output_buffer(int port, long min_output_buffer)
  {
    if((size_t)port >= d_min_output_buffer.size())
      d_min_output_buffer.push_back(min_output_buffer);
    else
      d_min_output_buffer[port] = min_output_buffer;
  }


  bool
  block::update_rate() const
  {
    return d_update_rate;
  }

  void
  block::enable_update_rate(bool en)
  {
    d_update_rate = en;
  }

  float
  block::pc_noutput_items()
  {
    if(d_detail) {
      return d_detail->pc_noutput_items();
    }
    else {
      return 0;
    }
  }

  float
  block::pc_noutput_items_avg()
  {
    if(d_detail) {
      return d_detail->pc_noutput_items_avg();
    }
    else {
      return 0;
    }
  }

  float
  block::pc_noutput_items_var()
  {
    if(d_detail) {
      return d_detail->pc_noutput_items_var();
    }
    else {
      return 0;
    }
  }

  float
  block::pc_nproduced()
  {
    if(d_detail) {
      return d_detail->pc_nproduced();
    }
    else {
      return 0;
    }
  }

  float
  block::pc_nproduced_avg()
  {
    if(d_detail) {
      return d_detail->pc_nproduced_avg();
    }
    else {
      return 0;
    }
  }

  float
  block::pc_nproduced_var()
  {
    if(d_detail) {
      return d_detail->pc_nproduced_var();
    }
    else {
      return 0;
    }
  }

  float
  block::pc_input_buffers_full(int which)
  {
    if(d_detail) {
      return d_detail->pc_input_buffers_full(static_cast<size_t>(which));
    }
    else {
      return 0;
    }
  }

  float
  block::pc_input_buffers_full_avg(int which)
  {
    if(d_detail) {
      return d_detail->pc_input_buffers_full_avg(static_cast<size_t>(which));
    }
    else {
      return 0;
    }
  }

  float
  block::pc_input_buffers_full_var(int which)
  {
    if(d_detail) {
      return d_detail->pc_input_buffers_full_var(static_cast<size_t>(which));
    }
    else {
      return 0;
    }
  }

  std::vector<float>
  block::pc_input_buffers_full()
  {
    if(d_detail) {
      return d_detail->pc_input_buffers_full();
    }
    else {
      return std::vector<float>(1,0);
    }
  }

  std::vector<float>
  block::pc_input_buffers_full_avg()
  {
    if(d_detail) {
      return d_detail->pc_input_buffers_full_avg();
    }
    else {
      return std::vector<float>(1,0);
    }
  }

  std::vector<float>
  block::pc_input_buffers_full_var()
  {
    if(d_detail) {
      return d_detail->pc_input_buffers_full_var();
    }
    else {
      return std::vector<float>(1,0);
    }
  }

  float
  block::pc_output_buffers_full(int which)
  {
    if(d_detail) {
      return d_detail->pc_output_buffers_full(static_cast<size_t>(which));
    }
    else {
      return 0;
    }
  }

  float
  block::pc_output_buffers_full_avg(int which)
  {
    if(d_detail) {
      return d_detail->pc_output_buffers_full_avg(static_cast<size_t>(which));
    }
    else {
      return 0;
    }
  }

  float
  block::pc_output_buffers_full_var(int which)
  {
    if(d_detail) {
      return d_detail->pc_output_buffers_full_var(static_cast<size_t>(which));
    }
    else {
      return 0;
    }
  }

  std::vector<float>
  block::pc_output_buffers_full()
  {
    if(d_detail) {
      return d_detail->pc_output_buffers_full();
    }
    else {
      return std::vector<float>(1,0);
    }
  }

  std::vector<float>
  block::pc_output_buffers_full_avg()
  {
    if(d_detail) {
      return d_detail->pc_output_buffers_full_avg();
    }
    else {
      return std::vector<float>(1,0);
    }
  }

  std::vector<float>
  block::pc_output_buffers_full_var()
  {
    if(d_detail) {
      return d_detail->pc_output_buffers_full_var();
    }
    else {
      return std::vector<float>(1,0);
    }
  }

  float
  block::pc_work_time()
  {
    if(d_detail) {
      return d_detail->pc_work_time();
    }
    else {
      return 0;
    }
  }

  float
  block::pc_work_time_avg()
  {
    if(d_detail) {
      return d_detail->pc_work_time_avg();
    }
    else {
      return 0;
    }
  }

  float
  block::pc_work_time_var()
  {
    if(d_detail) {
      return d_detail->pc_work_time_var();
    }
    else {
      return 0;
    }
  }

  float
  block::pc_work_time_total()
  {
    if(d_detail) {
      return d_detail->pc_work_time_total();
    }
    else {
      return 0;
    }
  }

  float
  block::pc_throughput_avg() {
    if(d_detail) {
      return d_detail->pc_throughput_avg();
    }
    else {
      return 0;
    }
  }

  void
  block::reset_perf_counters()
  {
    if(d_detail) {
      d_detail->reset_perf_counters();
    }
  }


  void
  block::system_handler(pmt::pmt_t msg)
  {
    //std::cout << "system_handler " << msg << "\n";
    pmt::pmt_t op = pmt::car(msg);
    if(pmt::eqv(op, d_pmt_done)){
        d_finished = pmt::to_long(pmt::cdr(msg));
        global_block_registry.notify_blk(alias());
    } else {
        std::cout << "WARNING: bad message op on system port!\n";
        pmt::print(msg);
    }
  }

  void
  block::notify_msg_neighbors()
  {
    size_t len = pmt::length(d_message_subscribers);
    pmt::pmt_t port_names = pmt::make_vector(len, pmt::PMT_NIL);
    pmt::pmt_t keys = pmt::dict_keys(d_message_subscribers);
    for(size_t i = 0; i < len; i++) {
      // for each output port
      pmt::pmt_t oport = pmt::nth(i,keys);

      // for each subscriber on this port
      pmt::pmt_t currlist = pmt::dict_ref(d_message_subscribers, oport, pmt::PMT_NIL);

      // iterate through subscribers on port
      while(pmt::is_pair(currlist)) {
        pmt::pmt_t target = pmt::car(currlist);

        pmt::pmt_t block = pmt::car(target);

        currlist = pmt::cdr(currlist);
        basic_block_sptr blk = global_block_registry.block_lookup(block);
        blk->post(d_system_port, pmt::cons(d_pmt_done, pmt::mp(true)));
        }
    }
  }

  bool
  block::finished()
  {
    if((detail()->ninputs() != 0) || (detail()->noutputs() != 0))
      return false;
    else
      return d_finished;
  }



  void
  block::setup_pc_rpc()
  {
    d_pc_rpc_set = true;
#if defined(GR_CTRLPORT) && defined(GR_PERFORMANCE_COUNTERS)
#include <gnuradio/rpcregisterhelpers.h>
    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_trigger<block>(
        alias(), "reset_perf_counters", &block::reset_perf_counters,
        "Reset the Performance Counters", RPC_PRIVLVL_MIN)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_get<block, float>(
        alias(), "noutput_items", &block::pc_noutput_items,
        pmt::mp(0), pmt::mp(32768), pmt::mp(0),
        "", "noutput items", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_get<block, float>(
        alias(), "avg noutput_items", &block::pc_noutput_items_avg,
        pmt::mp(0), pmt::mp(32768), pmt::mp(0),
        "", "Average noutput items", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_get<block, float>(
        alias(), "var noutput_items", &block::pc_noutput_items_var,
        pmt::mp(0), pmt::mp(32768), pmt::mp(0),
        "", "Var. noutput items", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_get<block, float>(
        alias(), "nproduced", &block::pc_nproduced,
        pmt::mp(0), pmt::mp(32768), pmt::mp(0),
        "", "items produced", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_get<block, float>(
        alias(), "avg nproduced", &block::pc_nproduced_avg,
        pmt::mp(0), pmt::mp(32768), pmt::mp(0),
        "", "Average items produced", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_get<block, float>(
        alias(), "var nproduced", &block::pc_nproduced_var,
        pmt::mp(0), pmt::mp(32768), pmt::mp(0),
        "", "Var. items produced", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_get<block, float>(
        alias(), "work time", &block::pc_work_time,
        pmt::mp(0), pmt::mp(1e9), pmt::mp(0),
        "", "clock cycles in call to work", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_get<block, float>(
        alias(), "avg work time", &block::pc_work_time_avg,
        pmt::mp(0), pmt::mp(1e9), pmt::mp(0),
        "", "Average clock cycles in call to work", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_get<block, float>(
        alias(), "var work time", &block::pc_work_time_var,
        pmt::mp(0), pmt::mp(1e9), pmt::mp(0),
        "", "Var. clock cycles in call to work", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_get<block, float>(
        alias(), "total work time", &block::pc_work_time_total,
        pmt::mp(0), pmt::mp(1e9), pmt::mp(0),
        "", "Total clock cycles in calls to work", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_get<block, float>(
        alias(), "avg throughput", &block::pc_throughput_avg,
        pmt::mp(0), pmt::mp(1e9), pmt::mp(0),
        "items/s", "Average items throughput in call to work", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_get<block, std::vector<float> >(
        alias(), "input \% full", &block::pc_input_buffers_full,
        pmt::make_f32vector(0,0), pmt::make_f32vector(0,1), pmt::make_f32vector(0,0),
        "", "how full input buffers are", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_get<block, std::vector<float> >(
        alias(), "avg input \% full", &block::pc_input_buffers_full_avg,
        pmt::make_f32vector(0,0), pmt::make_f32vector(0,1), pmt::make_f32vector(0,0),
        "", "Average of how full input buffers are", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_get<block, std::vector<float> >(
        alias(), "var input \% full", &block::pc_input_buffers_full_var,
        pmt::make_f32vector(0,0), pmt::make_f32vector(0,1), pmt::make_f32vector(0,0),
        "", "Var. of how full input buffers are", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_get<block, std::vector<float> >(
        alias(), "output \% full", &block::pc_output_buffers_full,
        pmt::make_f32vector(0,0), pmt::make_f32vector(0,1), pmt::make_f32vector(0,0),
        "", "how full output buffers are", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_get<block, std::vector<float> >(
        alias(), "avg output \% full", &block::pc_output_buffers_full_avg,
        pmt::make_f32vector(0,0), pmt::make_f32vector(0,1), pmt::make_f32vector(0,0),
        "", "Average of how full output buffers are", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));

    d_rpc_vars.push_back(
      rpcbasic_sptr(new rpcbasic_register_get<block, std::vector<float> >(
        alias(), "var output \% full", &block::pc_output_buffers_full_var,
        pmt::make_f32vector(0,0), pmt::make_f32vector(0,1), pmt::make_f32vector(0,0),
        "", "Var. of how full output buffers are", RPC_PRIVLVL_MIN,
        DISPTIME | DISPOPTSTRIP)));
#endif /* defined(GR_CTRLPORT) && defined(GR_PERFORMANCE_COUNTERS) */
  }

  std::ostream&
  operator << (std::ostream& os, const block *m)
  {
    os << "<block " << m->name() << " (" << m->unique_id() << ")>";
    return os;
  }

  int
  block::general_work(int noutput_items,
                      gr_vector_int &ninput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items)
  {
    throw std::runtime_error("block::general_work() not implemented");
    return 0;
  }

} /* namespace gr */
