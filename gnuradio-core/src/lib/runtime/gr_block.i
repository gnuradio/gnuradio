/* -*- c++ -*- */
/*
 * Copyright 2004,2010 Free Software Foundation, Inc.
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

%include <gr_basic_block.i>

class gr_block;
typedef boost::shared_ptr<gr_block> gr_block_sptr;
%template(gr_block_sptr) boost::shared_ptr<gr_block>;

// support vectors of these...
namespace std {
  %template(x_vector_gr_block_sptr)	vector<gr_block_sptr>;
};

class gr_block : public gr_basic_block {
 protected:
  gr_block (const std::string &name,
            gr_io_signature_sptr input_signature,
            gr_io_signature_sptr output_signature);

 public:

  virtual ~gr_block ();

  unsigned history () const;

  int  output_multiple () const;
  double relative_rate () const;

  bool start();
  bool stop();

  uint64_t nitems_read(unsigned int which_input);
  uint64_t nitems_written(unsigned int which_output);

  // Methods to manage the block's max_noutput_items size.
  int max_noutput_items();
  void set_max_noutput_items(int m);
  void unset_max_noutput_items();
  bool is_set_max_noutput_items();

  // Methods to manage block's min/max buffer sizes.
  long max_output_buffer(int i);
  void set_max_output_buffer(long max_output_buffer);
  void set_max_output_buffer(int port, long max_output_buffer);
  long min_output_buffer(int i);
  void set_min_output_buffer(long min_output_buffer);
  void set_min_output_buffer(int port, long min_output_buffer);

  // Methods to access performance counters
  float pc_noutput_items();
  float pc_noutput_items_var();
  float pc_nproduced();
  float pc_nproduced_var();
  float pc_input_buffers_full(int which);
  float pc_input_buffers_full_var(int which);
  std::vector<float> pc_input_buffers_full();
  std::vector<float> pc_input_buffers_full_var();
  float pc_output_buffers_full(int which);
  float pc_output_buffers_full_var(int which);
  std::vector<float> pc_output_buffers_full();
  std::vector<float> pc_output_buffers_full_var();
  float pc_work_time();
  float pc_work_time_var();
  
  // Methods to manage processor affinity.
  void set_processor_affinity(const std::vector<int> &mask);
  void unset_processor_affinity();
  std::vector<int> processor_affinity();

  // internal use
  gr_block_detail_sptr detail () const { return d_detail; }
  void set_detail (gr_block_detail_sptr detail) { d_detail = detail; }
};
