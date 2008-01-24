/* -*- c++ -*- */
/*
 * Copyright 2004,2007 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_BLOCK_H
#define INCLUDED_GR_BLOCK_H

#include <gr_basic_block.h>

/*!
 * \brief The abstract base class for all 'terminal' processing blocks.
 * \ingroup base
 *
 * A signal processing flow is constructed by creating a tree of 
 * hierarchical blocks, which at any level may also contain terminal nodes
 * that actually implement signal processing functions. This is the base
 * class for all such leaf nodes.
 
 * Blocks have a set of input streams and output streams.  The
 * input_signature and output_signature define the number of input
 * streams and output streams respectively, and the type of the data
 * items in each stream.
 *
 * Although blocks may consume data on each input stream at a
 * different rate, all outputs streams must produce data at the same
 * rate.  That rate may be different from any of the input rates.
 *
 * User derived blocks override two methods, forecast and general_work,
 * to implement their signal processing behavior. forecast is called
 * by the system scheduler to determine how many items are required on
 * each input stream in order to produce a given number of output
 * items.
 *
 * general_work is called to perform the signal processing in the block.
 * It reads the input items and writes the output items.
 */

class gr_block : public gr_basic_block {

 public:
  
  virtual ~gr_block ();

  /*!
   * Assume block computes y_i = f(x_i, x_i-1, x_i-2, x_i-3...)
   * History is the number of x_i's that are examined to produce one y_i.
   * This comes in handy for FIR filters, where we use history to
   * ensure that our input contains the appropriate "history" for the
   * filter.   History should be equal to the number of filter taps.
   */
  unsigned history () const { return d_history; }
  void  set_history (unsigned history) { d_history = history; }
  
  /*!
   * \brief return true if this block has a fixed input to output rate
   *
   * If true, then fixed_rate_in_to_out and fixed_rate_out_to_in may be called.
   */
  bool fixed_rate() const { return d_fixed_rate; }

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
  virtual void forecast (int noutput_items,
			 gr_vector_int &ninput_items_required);

  /*!
   * \brief compute output items from input items
   *
   * \param noutput_items	number of output items to write on each output stream
   * \param ninput_items	number of input items available on each input stream
   * \param input_items		vector of pointers to the input items, one entry per input stream
   * \param output_items	vector of pointers to the output items, one entry per output stream
   *
   * \returns number of items actually written to each output stream, or -1 on EOF.
   * It is OK to return a value less than noutput_items.  -1 <= return value <= noutput_items
   *
   * general_work must call consume or consume_each to indicate how many items
   * were consumed on each input stream.
   */
  virtual int general_work (int noutput_items,
			    gr_vector_int &ninput_items,
			    gr_vector_const_void_star &input_items,
			    gr_vector_void_star &output_items) = 0;

  /*!
   * \brief Called to enable drivers, etc for i/o devices.
   *
   * This allows a block to enable an associated driver to begin
   * transfering data just before we start to execute the scheduler.
   * The end result is that this reduces latency in the pipeline when
   * dealing with audio devices, usrps, etc.
   */
  virtual bool start();

  /*!
   * \brief Called to disable drivers, etc for i/o devices.
   */
  virtual bool stop();

  // ----------------------------------------------------------------

  /*!
   * \brief Constrain the noutput_items argument passed to forecast and general_work
   *
   * set_output_multiple causes the scheduler to ensure that the noutput_items
   * argument passed to forecast and general_work will be an integer multiple
   * of \param multiple  The default value of output multiple is 1.
   */
  void set_output_multiple (int multiple);
  int  output_multiple () const { return d_output_multiple; }

  /*!
   * \brief Tell the scheduler \p how_many_items of input stream \p which_input were consumed.
   */
  void consume (int which_input, int how_many_items);

  /*!
   * \brief Tell the scheduler \p how_many_items were consumed on each input stream.
   */
  void consume_each (int how_many_items);

  /*!
   * \brief Set the approximate output rate / input rate
   *
   * Provide a hint to the buffer allocator and scheduler.
   * The default relative_rate is 1.0
   *
   * decimators have relative_rates < 1.0
   * interpolators have relative_rates > 1.0
   */
  void  set_relative_rate (double relative_rate);

  /*!
   * \brief return the approximate output rate / input rate
   */
  double relative_rate () const	{ return d_relative_rate; }

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
  virtual int fixed_rate_ninput_to_noutput(int ninput);

  /*!
   * \brief Given noutput samples, return number of input samples required to produce noutput.
   * N.B. this is only defined if fixed_rate returns true.
   * Generally speaking, you don't need to override this.
   */
  virtual int fixed_rate_noutput_to_ninput(int noutput);

  // ----------------------------------------------------------------------------

 private:

  int                   d_output_multiple;
  double                d_relative_rate;	// approx output_rate / input_rate
  gr_block_detail_sptr	d_detail;		    // implementation details
  unsigned              d_history;
  bool                  d_fixed_rate;
    
 protected:

  gr_block (const std::string &name,
            gr_io_signature_sptr input_signature,
            gr_io_signature_sptr output_signature);

  void set_fixed_rate(bool fixed_rate){ d_fixed_rate = fixed_rate; }

  // These are really only for internal use, but leaving them public avoids
  // having to work up an ever-varying list of friends

 public:
  gr_block_detail_sptr detail () const { return d_detail; }
  void set_detail (gr_block_detail_sptr detail) { d_detail = detail; }
};

typedef std::vector<gr_block_sptr> gr_block_vector_t;
typedef std::vector<gr_block_sptr>::iterator gr_block_viter_t;

inline gr_block_sptr make_gr_block_sptr(gr_basic_block_sptr p)
{
  return boost::dynamic_pointer_cast<gr_block, gr_basic_block>(p);
}

#endif /* INCLUDED_GR_BLOCK_H */
