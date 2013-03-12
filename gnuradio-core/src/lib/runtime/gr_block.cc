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

#include <gr_block.h>
#include <gr_block_detail.h>
#include <stdexcept>
#include <iostream>
#include <gr_block_registry.h>
#include <gr_prefs.h>

gr_block::gr_block (const std::string &name,
                    gr_io_signature_sptr input_signature,
                    gr_io_signature_sptr output_signature)
  : gr_basic_block(name, input_signature, output_signature),
    d_output_multiple (1),
    d_output_multiple_set(false),
    d_unaligned(0),
    d_is_unaligned(false),
    d_relative_rate (1.0),
    d_history(1),
    d_fixed_rate(false),
    d_max_noutput_items_set(false),
    d_max_noutput_items(0),
    d_min_noutput_items(0),
    d_tag_propagation_policy(TPP_ALL_TO_ALL),
    d_max_output_buffer(std::max(output_signature->max_streams(),1), -1),
    d_min_output_buffer(std::max(output_signature->max_streams(),1), -1)
{
    global_block_registry.register_primitive(alias(), this);

#ifdef ENABLE_GR_LOG
#ifdef HAVE_LOG4CXX
    gr_prefs *p = gr_prefs::singleton();
    std::string config_file = p->get_string("LOG", "log_config", "");
    std::string log_level = p->get_string("LOG", "log_level", "off");
    std::string log_file = p->get_string("LOG", "log_file", "");
    std::string debug_level = p->get_string("LOG", "debug_level", "off");
    std::string debug_file = p->get_string("LOG", "debug_file", "");

    GR_CONFIG_LOGGER(config_file);

    GR_LOG_GETLOGGER(LOG, "gr_log." + alias());
    GR_LOG_SET_LEVEL(LOG, log_level);
    if(log_file.size() > 0) {
      if(log_file == "stdout") {
        GR_LOG_ADD_CONSOLE_APPENDER(LOG, "gr::log :%p: %c{1} - %m%n", "System.out");
      }
      else if(log_file == "stderr") {
        GR_LOG_ADD_CONSOLE_APPENDER(LOG, "gr::log :%p: %c{1} - %m%n", "System.err");
      }
      else {
        GR_LOG_ADD_FILE_APPENDER(LOG, "%r :%p: %c{1} - %m%n", log_file, "");
      }
    }
    d_logger = LOG;

    GR_LOG_GETLOGGER(DLOG, "gr_log_debug." + alias());
    GR_LOG_SET_LEVEL(DLOG, debug_level);
    if(debug_file.size() > 0) {
      if(debug_file == "stdout") {
        GR_LOG_ADD_CONSOLE_APPENDER(DLOG, "gr::debug :%p: %c{1} - %m%n", "System.out");
      }
      else if(debug_file == "stderr") {
        GR_LOG_ADD_CONSOLE_APPENDER(DLOG, "gr::debug :%p: %c{1} - %m%n", "System.err");
      }
      else {
        GR_LOG_ADD_FILE_APPENDER(DLOG, "%r :%p: %c{1} - %m%n", debug_file, "");
      }
    }
    d_debug_logger = DLOG;
#endif /* HAVE_LOG4CXX */
#else /* ENABLE_GR_LOG */
    d_logger = NULL;
    d_debug_logger = NULL;
#endif /* ENABLE_GR_LOG */
}

gr_block::~gr_block ()
{
    global_block_registry.unregister_primitive(alias());
}

// stub implementation:  1:1

void
gr_block::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  unsigned ninputs = ninput_items_required.size ();
  for (unsigned i = 0; i < ninputs; i++)
    ninput_items_required[i] = noutput_items + history() - 1;
}

// default implementation

bool
gr_block::start()
{
  return true;
}

bool
gr_block::stop()
{
  return true;
}

void
gr_block::set_output_multiple (int multiple)
{
  if (multiple < 1)
    throw std::invalid_argument ("gr_block::set_output_multiple");

  d_output_multiple_set = true;
  d_output_multiple = multiple;
}

void
gr_block::set_alignment (int multiple)
{
  if (multiple < 1)
    throw std::invalid_argument ("gr_block::set_alignment_multiple");

  d_output_multiple = multiple;
}

void
gr_block::set_unaligned (int na)
{
  // unaligned value must be less than 0 and it doesn't make sense
  // that it's larger than the alignment value.
  if ((na < 0) || (na > d_output_multiple))
    throw std::invalid_argument ("gr_block::set_unaligned");

  d_unaligned = na;
}

void
gr_block::set_is_unaligned (bool u)
{
  d_is_unaligned = u;
}

void
gr_block::set_relative_rate (double relative_rate)
{
  if (relative_rate < 0.0)
    throw std::invalid_argument ("gr_block::set_relative_rate");

  d_relative_rate = relative_rate;
}


void
gr_block::consume (int which_input, int how_many_items)
{
  d_detail->consume (which_input, how_many_items);
}

void
gr_block::consume_each (int how_many_items)
{
  d_detail->consume_each (how_many_items);
}

void
gr_block::produce (int which_output, int how_many_items)
{
  d_detail->produce (which_output, how_many_items);
}

int
gr_block::fixed_rate_ninput_to_noutput(int ninput)
{
  throw std::runtime_error("Unimplemented");
}

int
gr_block::fixed_rate_noutput_to_ninput(int noutput)
{
  throw std::runtime_error("Unimplemented");
}

uint64_t
gr_block::nitems_read(unsigned int which_input)
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
gr_block::nitems_written(unsigned int which_output)
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
gr_block::add_item_tag(unsigned int which_output,
		       const gr_tag_t &tag)
{
  d_detail->add_item_tag(which_output, tag);
}

void
gr_block::remove_item_tag(unsigned int which_input,
		       const gr_tag_t &tag)
{
  d_detail->remove_item_tag(which_input, tag);
}

void
gr_block::get_tags_in_range(std::vector<gr_tag_t> &v,
			    unsigned int which_output,
			    uint64_t start, uint64_t end)
{
  d_detail->get_tags_in_range(v, which_output, start, end);
}

void
gr_block::get_tags_in_range(std::vector<gr_tag_t> &v,
			    unsigned int which_output,
			    uint64_t start, uint64_t end,
			    const pmt::pmt_t &key)
{
  d_detail->get_tags_in_range(v, which_output, start, end, key);
}

gr_block::tag_propagation_policy_t
gr_block::tag_propagation_policy()
{
  return d_tag_propagation_policy;
}

void
gr_block::set_tag_propagation_policy(tag_propagation_policy_t p)
{
  d_tag_propagation_policy = p;
}


int
gr_block::max_noutput_items()
{
  return d_max_noutput_items;
}

void
gr_block::set_max_noutput_items(int m)
{
  if(m <= 0)
    throw std::runtime_error("gr_block::set_max_noutput_items: value for max_noutput_items must be greater than 0.\n");

  d_max_noutput_items = m;
  d_max_noutput_items_set = true;
}

void
gr_block::unset_max_noutput_items()
{
  d_max_noutput_items_set = false;
}

bool
gr_block::is_set_max_noutput_items()
{
  return d_max_noutput_items_set;
}

void
gr_block::set_processor_affinity(const std::vector<unsigned int> &mask)
{
  d_affinity = mask;
  if(d_detail) {
    d_detail->set_processor_affinity(d_affinity);
  }
}

void
gr_block::unset_processor_affinity()
{
  d_affinity.clear();
  if(d_detail) {
    d_detail->unset_processor_affinity();
  }
}

float
gr_block::pc_noutput_items()
{
  if(d_detail) {
    return d_detail->pc_noutput_items();
  }
  else {
    return 0;
  }
}

float
gr_block::pc_noutput_items_var()
{
  if(d_detail) {
    return d_detail->pc_noutput_items_var();
  }
  else {
    return 0;
  }
}

float
gr_block::pc_nproduced()
{
  if(d_detail) {
    return d_detail->pc_nproduced();
  }
  else {
    return 0;
  }
}

float
gr_block::pc_nproduced_var()
{
  if(d_detail) {
    return d_detail->pc_nproduced_var();
  }
  else {
    return 0;
  }
}

float
gr_block::pc_input_buffers_full(int which)
{
  if(d_detail) {
    return d_detail->pc_input_buffers_full(static_cast<size_t>(which));
  }
  else {
    return 0;
  }
}

float
gr_block::pc_input_buffers_full_var(int which)
{
  if(d_detail) {
    return d_detail->pc_input_buffers_full_var(static_cast<size_t>(which));
  }
  else {
    return 0;
  }
}

std::vector<float>
gr_block::pc_input_buffers_full()
{
  if(d_detail) {
    return d_detail->pc_input_buffers_full();
  }
  else {
    return std::vector<float>(1,0);
  }
}

std::vector<float>
gr_block::pc_input_buffers_full_var()
{
  if(d_detail) {
    return d_detail->pc_input_buffers_full_var();
  }
  else {
    return std::vector<float>(1,0);
  }
}

float
gr_block::pc_output_buffers_full(int which)
{
  if(d_detail) {
    return d_detail->pc_output_buffers_full(static_cast<size_t>(which));
  }
  else {
    return 0;
  }
}

float
gr_block::pc_output_buffers_full_var(int which)
{
  if(d_detail) {
    return d_detail->pc_output_buffers_full_var(static_cast<size_t>(which));
  }
  else {
    return 0;
  }
}

std::vector<float>
gr_block::pc_output_buffers_full()
{
  if(d_detail) {
    return d_detail->pc_output_buffers_full();
  }
  else {
    return std::vector<float>(1,0);
  }
}

std::vector<float>
gr_block::pc_output_buffers_full_var()
{
  if(d_detail) {
    return d_detail->pc_output_buffers_full_var();
  }
  else {
    return std::vector<float>(1,0);
  }
}

float
gr_block::pc_work_time()
{
  if(d_detail) {
    return d_detail->pc_work_time();
  }
  else {
    return 0;
  }
}

float
gr_block::pc_work_time_var()
{
  if(d_detail) {
    return d_detail->pc_work_time_var();
  }
  else {
    return 0;
  }
}

void
gr_block::reset_perf_counters()
{
  if(d_detail) {
    d_detail->reset_perf_counters();
  }
}

std::ostream&
operator << (std::ostream& os, const gr_block *m)
{
  os << "<gr_block " << m->name() << " (" << m->unique_id() << ")>";
  return os;
}

int
gr_block::general_work(int noutput_items,
		       gr_vector_int &ninput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  throw std::runtime_error("gr_block::general_work() not implemented");
  return 0;
}
