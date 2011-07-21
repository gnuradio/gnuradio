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

#ifndef INCLUDED_GR_TEST_H
#define INCLUDED_GR_TEST_H

#include <gr_core_api.h>
#include <gr_block.h>
#include <string>
#include "gr_test_types.h"

class gr_test;
typedef boost::shared_ptr<gr_test> gr_test_sptr;

// public constructor
GR_CORE_API gr_test_sptr gr_make_test (const std::string &name=std::string("gr_test"),
        int min_inputs=1, int max_inputs=1, unsigned int sizeof_input_item=1,
        int min_outputs=1, int max_outputs=1, unsigned int sizeof_output_item=1,
        unsigned int history=1,unsigned int output_multiple=1,double relative_rate=1.0,
        bool fixed_rate=true,gr_consume_type_t cons_type=CONSUME_NOUTPUT_ITEMS, gr_produce_type_t prod_type=PRODUCE_NOUTPUT_ITEMS);

/*!
 * \brief Test class for testing runtime system (setting up buffers and such.)
 * \ingroup misc
 *
 * This block does not do any usefull actual data processing.
 * It just exposes setting all standard block parameters using the contructor or public methods.
 *
 * This block can be usefull when testing the runtime system.
 * You can force this block to have a large history, decimation 
 * factor and/or large output_multiple.
 * The runtime system should detect this and create large enough buffers
 * all through the signal chain.
 */
class GR_CORE_API gr_test : public gr_block {

 public:
  
  ~gr_test (){}
  
int general_work (int noutput_items,
			    gr_vector_int &ninput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items);
  // ----------------------------------------------------------------
  //		override these to define your behavior
  // ----------------------------------------------------------------

  /*!
   * \brief  Estimate input requirements given output request
   *
   * \param noutput_items           number of output items to produce
   * \param ninput_items_required   number of input items required on each input stream
   *
   * Given a request to product \p noutput_items, estimate the number of
   * data items required on each input stream.  The estimate doesn't have
   * to be exact, but should be close.
   */
   void forecast (int noutput_items,
			 gr_vector_int &ninput_items_required)
   {
     unsigned ninputs = ninput_items_required.size ();
     for (unsigned i = 0; i < ninputs; i++)
       ninput_items_required[i] = (int)((double)noutput_items / relative_rate()) + (int)history();
   }


  /*!
   * \brief Force check topology to return true or false.
   *
   * \param check_topology	value to return when check_topology is called (true or false)
   * default check_topology returns true
   *
   */
   void set_check_topology (bool check_topology){ d_check_topology=check_topology;}

  /*!
   * \brief Confirm that ninputs and noutputs is an acceptable combination.
   *
   * \param ninputs	number of input streams connected
   * \param noutputs	number of output streams connected
   *
   * \returns true if this is a valid configuration for this block.
   *
   * This function is called by the runtime system whenever the
   * topology changes.  Most classes do not need to override this.
   * This check is in addition to the constraints specified by the input
   * and output gr_io_signatures.
   */
  bool check_topology (int ninputs, int noutputs) { return d_check_topology;}

  // ----------------------------------------------------------------
  /*
   * The following two methods provide special case info to the
   * scheduler in the event that a block has a fixed input to output
   * ratio.  gr_sync_block, gr_sync_decimator and gr_sync_interpolator
   * override these.  If you're fixed rate, subclass one of those.
   */
  /*!
   * \brief Given ninput samples, return number of output samples that will be produced.
   * N.B. this is only defined if fixed_rate returns true.
   * Generally speaking, you don't need to override this.
   */
  int fixed_rate_ninput_to_noutput(int ninput) { return (int)((double)ninput/relative_rate()); }

  /*!
   * \brief Given noutput samples, return number of input samples required to produce noutput.
   * N.B. this is only defined if fixed_rate returns true.
   */
  int fixed_rate_noutput_to_ninput(int noutput)  { return (int)((double)noutput*relative_rate()); }

  /*!
   * \brief Set if fixed rate should return true.
   * N.B. This is normally a private method but we make it available here as public.
   */
  void set_fixed_rate_public(bool fixed_rate){ set_fixed_rate(fixed_rate);}

  /*!
   * \brief Set the consume pattern.
   *
   * \param cons_type	which consume pattern to use
   */
  void set_consume_type (gr_consume_type_t cons_type) { d_consume_type=cons_type;}

  /*!
   * \brief Set the consume limit.
   *
   * \param limit	min or maximum items to consume (depending on consume_type)
   */
  void set_consume_limit (unsigned int limit) { d_min_consume=limit; d_max_consume=limit;}

  /*!
   * \brief Set the produce pattern.
   *
   * \param prod_type	which produce pattern to use
   */
  void set_produce_type (gr_produce_type_t prod_type) { d_produce_type=prod_type;}

  /*!
   * \brief Set the produce limit.
   *
   * \param limit	min or maximum items to produce (depending on produce_type)
   */
  void set_produce_limit (unsigned int limit) { d_min_produce=limit; d_max_produce=limit;}

  // ----------------------------------------------------------------------------


  
 protected:
  unsigned int d_sizeof_input_item;
  unsigned int d_sizeof_output_item;
  bool d_check_topology;
  char d_temp;
  gr_consume_type_t d_consume_type;
  int d_min_consume;
  int d_max_consume;
  gr_produce_type_t d_produce_type;
  int d_min_produce;
  int d_max_produce;
  gr_test (const std::string &name,int min_inputs, int max_inputs, unsigned int sizeof_input_item,
                                   int min_outputs, int max_outputs, unsigned int sizeof_output_item,
                                   unsigned int history,unsigned int output_multiple,double relative_rate,
                                   bool fixed_rate,gr_consume_type_t cons_type, gr_produce_type_t prod_type);



  friend GR_CORE_API gr_test_sptr gr_make_test (const std::string &name,int min_inputs, int max_inputs, unsigned int sizeof_input_item,
                                   int min_outputs, int max_outputs, unsigned int sizeof_output_item,
                                   unsigned int history,unsigned int output_multiple,double relative_rate,
                                   bool fixed_rate,gr_consume_type_t cons_type, gr_produce_type_t prod_type);
};



#endif /* INCLUDED_GR_TEST_H */
