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
 * GNU General Public License for more detail.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_BLOCK_DETAIL_H
#define INCLUDED_GR_BLOCK_DETAIL_H

#include <gr_core_api.h>
#include <gr_runtime_types.h>
#include <gr_tpb_detail.h>
#include <gr_tags.h>
#include <stdexcept>

/*!
 * \brief Implementation details to support the signal processing abstraction
 * \ingroup internal
 *
 * This class contains implementation detail that should be "out of sight"
 * of almost all users of GNU Radio.  This decoupling also means that
 * we can make changes to the guts without having to recompile everything.
 */
class GR_CORE_API gr_block_detail {
 public:
  ~gr_block_detail ();

  int ninputs () const { return d_ninputs; }
  int noutputs () const { return d_noutputs; }
  bool sink_p () const { return d_noutputs == 0; }
  bool source_p () const { return d_ninputs == 0; }

  void set_done (bool done);
  bool done () const { return d_done; }

  void set_input (unsigned int which, gr_buffer_reader_sptr reader);
  gr_buffer_reader_sptr input (unsigned int which)
  {
    if (which >= d_ninputs)
      throw std::invalid_argument ("gr_block_detail::input");
    return d_input[which];
  }

  void set_output (unsigned int which, gr_buffer_sptr buffer);
  gr_buffer_sptr output (unsigned int which)
  {
    if (which >= d_noutputs)
      throw std::invalid_argument ("gr_block_detail::output");
    return d_output[which];
  }

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
   */
  void produce (int which_output, int how_many_items);

  /*!
   * \brief Tell the scheduler \p how_many_items were produced on each output stream.
   */
  void produce_each (int how_many_items);

  /*!
   * Accept msg, place in queue, arrange for thread to be awakened if it's not already.
   */
  void _post(pmt::pmt_t msg);

  // Return the number of items read on input stream which_input
  uint64_t nitems_read(unsigned int which_input);

  // Return the number of items written on output stream which_output
  uint64_t nitems_written(unsigned int which_output);

  
  /*!
   * \brief  Adds a new tag to the given output stream.
   * 
   * This takes the input parameters and builds a PMT tuple 
   * from it. It then calls gr_buffer::add_item_tag(pmt::pmt_t t),
   * which appends the tag onto its deque.
   *
   * \param which_output  an integer of which output stream to attach the tag
   * \param tag the tag object to add
   */
  void add_item_tag(unsigned int which_output, const gr_tag_t &tag);

  /*!
   * \brief Given a [start,end), returns a vector of all tags in the range.
   *
   * Pass-through function to gr_buffer_reader to get a vector of tags 
   * in given range. Range of counts is from start to end-1.
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
   * Calls get_tags_in_range(which_input, abs_start, abs_end) to get a vector of
   * tags from the buffers. This function then provides a secondary filter to
   * the tags to extract only tags with the given 'key'.
   *
   * Tags are tuples of:
   *      (item count, source id, key, value)
   *
   * \param v            a vector reference to return tags into
   * \param which_input  an integer of which input stream to pull from
   * \param abs_start    a uint64 count of the start of the range of interest
   * \param abs_end      a uint64 count of the end of the range of interest
   * \param key          a PMT symbol to select only tags of this key
   */
  void get_tags_in_range(std::vector<gr_tag_t> &v,
			 unsigned int which_input,
			 uint64_t abs_start,
			 uint64_t abs_end,
			 const pmt::pmt_t &key);

  gr_tpb_detail			     d_tpb;	// used by thread-per-block scheduler
  int				     d_produce_or;

  // ----------------------------------------------------------------------------

 private:
  unsigned int                       d_ninputs;
  unsigned int                       d_noutputs;
  std::vector<gr_buffer_reader_sptr> d_input;
  std::vector<gr_buffer_sptr>	     d_output;
  bool                               d_done;

  gr_block_detail (unsigned int ninputs, unsigned int noutputs);

  friend struct gr_tpb_detail;

  friend GR_CORE_API gr_block_detail_sptr
  gr_make_block_detail (unsigned int ninputs, unsigned int noutputs);
};

GR_CORE_API gr_block_detail_sptr
gr_make_block_detail (unsigned int ninputs, unsigned int noutputs);

GR_CORE_API long
gr_block_detail_ncurrently_allocated ();

#endif /* INCLUDED_GR_BLOCK_DETAIL_H */
