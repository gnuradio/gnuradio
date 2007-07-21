/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(gr,test);


class gr_test;
typedef boost::shared_ptr<gr_test> gr_test_sptr;


// public constructor
gr_test_sptr gr_make_test (const std::string &name=std::string("gr_test"),
        int min_inputs=1, int max_inputs=1, unsigned int sizeof_input_item=1,
        int min_outputs=1, int max_outputs=1, unsigned int sizeof_output_item=1,
        unsigned int history=1,unsigned int output_multiple=1,double relative_rate=1.0,
        bool fixed_rate=true,gr_consume_type_t cons_type=CONSUME_NOUTPUT_ITEMS, gr_produce_type_t prod_type=PRODUCE_NOUTPUT_ITEMS);


class gr_test : public gr_block {

 public:
  
  ~gr_test ();
  void forecast (int noutput_items,
			 gr_vector_int &ninput_items_required);
  void set_check_topology (bool check_topology);
  bool check_topology (int ninputs, int noutputs);
  int fixed_rate_ninput_to_noutput(int ninput);
  int fixed_rate_noutput_to_ninput(int noutput);
  void set_fixed_rate_public(bool fixed_rate);
  void set_consume_type (gr_consume_type_t cons_type);
  void set_consume_limit (unsigned int limit);
  void set_produce_type (gr_produce_type_t prod_type);
  void set_produce_limit (unsigned int limit);

 protected:
  gr_test (const std::string &name,int min_inputs, int max_inputs, unsigned int sizeof_input_item,
                                   int min_outputs, int max_outputs, unsigned int sizeof_output_item,
                                   unsigned int history,unsigned int output_multiple,double relative_rate,
                                   bool fixed_rate,gr_consume_type_t cons_type, gr_produce_type_t prod_type);

};



