/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2009,2010 Free Software Foundation, Inc.
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

#include <gr_core_api.h>
#include <gr_basic_block.h>
#include <gr_tags.h>

/*!
 * \brief The abstract base class for all 'terminal' processing blocks.
 * \ingroup base_blk
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

class GR_CORE_API gr_block : public gr_basic_block {

 public:
  
  //! Magic return values from general_work
  enum {
    WORK_CALLED_PRODUCE = -2,
    WORK_DONE = -1
  };

  enum tag_propagation_policy_t {
    TPP_DONT = 0,
    TPP_ALL_TO_ALL = 1,
    TPP_ONE_TO_ONE = 2
  };

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
   * \brief Return true if this block has a fixed input to output rate.
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
  bool  output_multiple_set () const { return d_output_multiple_set; }

  /*!
   * \brief Constrains buffers to work on a set item alignment (for SIMD)
   *
   * set_alignment_multiple causes the scheduler to ensure that the noutput_items
   * argument passed to forecast and general_work will be an integer multiple
   * of \param multiple  The default value is 1.
   *
   * This control is similar to the output_multiple setting, except
   * that if the number of items passed to the block is less than the
   * output_multiple, this value is ignored and the block can produce
   * like normal. The d_unaligned value is set to the number of items
   * the block is off by. In the next call to general_work, the
   * noutput_items is set to d_unaligned or less until
   * d_unaligned==0. The buffers are now aligned again and the aligned
   * calls can be performed again.
   */
  void set_alignment (int multiple);
  int  alignment () const { return d_output_multiple; }

  void set_unaligned (int na);
  int unaligned () const { return d_unaligned; }
  void set_is_unaligned (bool u);
  bool is_unaligned () const { return d_is_unaligned; }
  
  /*!
   * \brief Tell the scheduler \p how_many_items of input stream \p which_input were consumed.
   */
  void consume (int which_input, int how_many_items);

  /*!
   * \brief Tell the scheduler \p how_many_items were consumed on each input stream.
   */
  void consume_each (int how_many_items);

  /*!
   * \brief Tell the scheduler \p how_many_items were produced on output stream \p which_output.
   *
   * If the block's general_work method calls produce, \p general_work must return WORK_CALLED_PRODUCE.
   */
  void produce (int which_output, int how_many_items);

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

  /*!
   * \brief Return the number of items read on input stream which_input
   */
  uint64_t nitems_read(unsigned int which_input);

  /*!
   * \brief  Return the number of items written on output stream which_output
   */
  uint64_t nitems_written(unsigned int which_output);

  /*!
   * \brief Asks for the policy used by the scheduler to moved tags downstream.
   */
  tag_propagation_policy_t tag_propagation_policy();

  /*!
   * \brief Set the policy by the scheduler to determine how tags are moved downstream.
   */
  void set_tag_propagation_policy(tag_propagation_policy_t p);

  // ----------------------------------------------------------------------------

 private:

  int                   d_output_multiple;
  bool                  d_output_multiple_set;
  int                   d_unaligned;
  bool                  d_is_unaligned;
  double                d_relative_rate;	// approx output_rate / input_rate
  gr_block_detail_sptr	d_detail;		// implementation details
  unsigned              d_history;
  bool                  d_fixed_rate;
  tag_propagation_policy_t d_tag_propagation_policy; // policy for moving tags downstream
    
 protected:
  gr_block (void){} //allows pure virtual interface sub-classes
  gr_block (const std::string &name,
            gr_io_signature_sptr input_signature,
            gr_io_signature_sptr output_signature);

  void set_fixed_rate(bool fixed_rate){ d_fixed_rate = fixed_rate; }

  
  /*!
   * \brief  Adds a new tag onto the given output buffer.
   * 
   * \param which_output an integer of which output stream to attach the tag
   * \param abs_offset   a uint64 number of the absolute item number
   *                     assicated with the tag. Can get from nitems_written.
   * \param key          the tag key as a PMT symbol
   * \param value        any PMT holding any value for the given key
   * \param srcid        optional source ID specifier; defaults to PMT_F
   */
  inline void add_item_tag(unsigned int which_output,
		    uint64_t abs_offset,
		    const pmt::pmt_t &key,
		    const pmt::pmt_t &value,
		    const pmt::pmt_t &srcid=pmt::PMT_F)
    {
        gr_tag_t tag;
        tag.offset = abs_offset;
        tag.key = key;
        tag.value = value;
        tag.srcid = srcid;
        this->add_item_tag(which_output, tag);
    }

 /*!
   * \brief  Adds a new tag onto the given output buffer.
   *
   * \param which_output an integer of which output stream to attach the tag
   * \param tag the tag object to add
   */
  void add_item_tag(unsigned int which_output, const gr_tag_t &tag);

  /*!
   * \brief Given a [start,end), returns a vector of all tags in the range.
   *
   * Range of counts is from start to end-1.
   *
   * Tags are tuples of:
   *      (item count, source id, key, value)
   *
   * \param v            a vector reference to return tags into
   * \param which_input  an integer of which input stream to pull from
   * \param abs_start    a uint64 count of the start of the range of interest
   * \param abs_end      a uint64 count of the end of the range of interest
   */
  void get_tags_in_range(std::vector<gr_tag_t> &v,
			 unsigned int which_input,
			 uint64_t abs_start,
			 uint64_t abs_end);
  
  /*!
   * \brief Given a [start,end), returns a vector of all tags in the range
   * with a given key.
   *
   * Range of counts is from start to end-1.
   *
   * Tags are tuples of:
   *      (item count, source id, key, value)
   *
   * \param v            a vector reference to return tags into
   * \param which_input  an integer of which input stream to pull from
   * \param abs_start    a uint64 count of the start of the range of interest
   * \param abs_end      a uint64 count of the end of the range of interest
   * \param key          a PMT symbol key to filter only tags of this key
   */
  void get_tags_in_range(std::vector<gr_tag_t> &v,
			 unsigned int which_input,
			 uint64_t abs_start,
			 uint64_t abs_end,
			 const pmt::pmt_t &key);

  // These are really only for internal use, but leaving them public avoids
  // having to work up an ever-varying list of friend GR_CORE_APIs

 public:
  gr_block_detail_sptr detail () const { return d_detail; }
  void set_detail (gr_block_detail_sptr detail) { d_detail = detail; }
};

typedef std::vector<gr_block_sptr> gr_block_vector_t;
typedef std::vector<gr_block_sptr>::iterator gr_block_viter_t;

inline gr_block_sptr cast_to_block_sptr(gr_basic_block_sptr p)
{
  return boost::dynamic_pointer_cast<gr_block, gr_basic_block>(p);
}


std::ostream&
operator << (std::ostream& os, const gr_block *m);

#endif /* INCLUDED_GR_BLOCK_H */
