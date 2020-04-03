/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

%include <basic_block.i>

class gr::block;
typedef std::shared_ptr<gr::block> gr::block_sptr;
%template(block_sptr) std::shared_ptr<gr::block>;

// support vectors of these...
namespace std {
  %template(x_vector_block_sptr) vector<gr::block_sptr>;
};

class gr::block : public gr::basic_block
{
 protected:
  block (const std::string &name,
         gr::io_signature::sptr input_signature,
         gr::io_signature::sptr output_signature);

 public:

  virtual ~block ();

  unsigned history () const;

  void declare_sample_delay(int which, int delay);
  void declare_sample_delay(unsigned int delay);
  unsigned sample_delay(int which) const;

  void set_output_multiple(int multiple);
  int  output_multiple () const;
  double relative_rate () const;
  uint64_t relative_rate_i () const;
  uint64_t relative_rate_d () const;

  bool start();
  bool stop();

  uint64_t nitems_read(unsigned int which_input);
  uint64_t nitems_written(unsigned int which_output);

  void set_log_level(std::string level);
  std::string log_level();

  // Methods to manage the block's max_noutput_items size.
  int max_noutput_items();
  void set_max_noutput_items(int m);
  void unset_max_noutput_items();
  bool is_set_max_noutput_items();
  void set_min_noutput_items(int m);
  int min_noutput_items() const;

  // Methods to manage block's min/max buffer sizes.
  long max_output_buffer(int i);
  void set_max_output_buffer(long max_output_buffer);
  void set_max_output_buffer(int port, long max_output_buffer);
  long min_output_buffer(int i);
  void set_min_output_buffer(long min_output_buffer);
  void set_min_output_buffer(int port, long min_output_buffer);

  // Methods to access performance counters
  float pc_noutput_items();
  float pc_noutput_items_avg();
  float pc_noutput_items_var();
  float pc_nproduced();
  float pc_nproduced_avg();
  float pc_nproduced_var();
  float pc_input_buffers_full(int which);
  float pc_input_buffers_full_avg(int which);
  float pc_input_buffers_full_var(int which);
  std::vector<float> pc_input_buffers_full();
  std::vector<float> pc_input_buffers_full_avg();
  std::vector<float> pc_input_buffers_full_var();
  float pc_output_buffers_full(int which);
  float pc_output_buffers_full_avg(int which);
  float pc_output_buffers_full_var(int which);
  std::vector<float> pc_output_buffers_full();
  std::vector<float> pc_output_buffers_full_avg();
  std::vector<float> pc_output_buffers_full_var();
  float pc_work_time();
  float pc_work_time_avg();
  float pc_work_time_var();
  float pc_work_time_total();
  float pc_throughput_avg();

  // Methods to manage processor affinity.
  void set_processor_affinity(const std::vector<int> &mask);
  void unset_processor_affinity();
  std::vector<int> processor_affinity();

  // Methods to manage thread priority
  int active_thread_priority();
  int thread_priority();
  int set_thread_priority(int priority);

  // internal use
  //block_detail_sptr detail () const { return d_detail; }
  //void set_detail (block_detail_sptr detail) { d_detail = detail; }
};
