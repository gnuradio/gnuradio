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
 * GNU General Public License for more detail.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_RUNTIME_BLOCK_DETAIL_H
#define INCLUDED_GR_RUNTIME_BLOCK_DETAIL_H

#include <gnuradio/api.h>
#include <gnuradio/runtime_types.h>
#include <gnuradio/tpb_detail.h>
#include <gnuradio/tags.h>
#include <gnuradio/high_res_timer.h>
#include <stdexcept>

namespace gr {

  /*!
   * \brief Implementation details to support the signal processing abstraction
   * \ingroup internal
   *
   * This class contains implementation detail that should be "out of
   * sight" of almost all users of GNU Radio.  This decoupling also
   * means that we can make changes to the guts without having to
   * recompile everything.
   */
  class GR_RUNTIME_API block_detail
  {
  public:
    ~block_detail();

    int ninputs() const { return d_ninputs; }
    int noutputs() const { return d_noutputs; }
    bool sink_p() const { return d_noutputs == 0; }
    bool source_p() const { return d_ninputs == 0; }

    void set_done(bool done);
    bool done() const { return d_done; }

    void set_input(unsigned int which, buffer_reader_sptr reader);
    buffer_reader_sptr input(unsigned int which)
    {
      if(which >= d_ninputs)
        throw std::invalid_argument("block_detail::input");
      return d_input[which];
    }

    void set_output(unsigned int which, buffer_sptr buffer);
    buffer_sptr output(unsigned int which)
    {
      if(which >= d_noutputs)
        throw std::invalid_argument("block_detail::output");
      return d_output[which];
    }

    /*!
     * \brief Tell the scheduler \p how_many_items of input stream \p
     * which_input were consumed.
     */
    void consume(int which_input, int how_many_items);

    /*!
     * \brief Tell the scheduler \p how_many_items were consumed on
     * each input stream.
     */
    void consume_each(int how_many_items);

    /*!
     * \brief Tell the scheduler \p how_many_items were produced on
     * output stream \p which_output.
     */
    void produce(int which_output, int how_many_items);

    /*!
     * \brief Tell the scheduler \p how_many_items were produced on
     * each output stream.
     */
    void produce_each(int how_many_items);

    // Return the number of items read on input stream which_input
    uint64_t nitems_read(unsigned int which_input);

    // Return the number of items written on output stream which_output
    uint64_t nitems_written(unsigned int which_output);

    // sets nitems_read and nitems_written to 0 for all input/output
    // buffers.
    void reset_nitem_counters();

    // Clears all tags from the input buffers.
    void clear_tags();

    /*!
     * \brief  Adds a new tag to the given output stream.
     *
     * Calls gr::buffer::add_item_tag(),
     * which appends the tag onto its deque.
     *
     * \param which_output  an integer of which output stream to attach the tag
     * \param tag the tag object to add
     */
    void add_item_tag(unsigned int which_output, const tag_t &tag);

    /*!
     * \brief  Removes a tag from the given input stream.
     *
     * Calls gr::buffer::remove_item_tag().
     * The tag in question will then no longer appear on subsequent calls of get_tags_in_range().
     *
     * \param which_input  an integer of which input stream to remove the tag from
     * \param tag the tag object to add
     * \param id The unique block ID (use gr::block::unique_id())
     */
    void remove_item_tag(unsigned int which_input, const tag_t &tag, long id);

    /*!
     * \brief Given a [start,end), returns a vector of all tags in the range.
     *
     * Pass-through function to gr::buffer_reader to get a vector of
     * tags in given range. Range of counts is from start to end-1.
     *
     * Tags are tuples of:
     *      (item count, source id, key, value)
     *
     * \param v            a vector reference to return tags into
     * \param which_input  an integer of which input stream to pull from
     * \param abs_start    a uint64 count of the start of the range of interest
     * \param abs_end      a uint64 count of the end of the range of interest
     * \param id           Block ID
     */
    void get_tags_in_range(std::vector<tag_t> &v,
                           unsigned int which_input,
                           uint64_t abs_start,
                           uint64_t abs_end,
			   long id);

    /*!
     * \brief Given a [start,end), returns a vector of all tags in the
     * range with a given key.
     *
     * Calls get_tags_in_range(which_input, abs_start, abs_end) to get
     * a vector of tags from the buffers. This function then provides
     * a secondary filter to the tags to extract only tags with the
     * given 'key'.
     *
     * Tags are tuples of:
     *      (item count, source id, key, value)
     *
     * \param v            a vector reference to return tags into
     * \param which_input  an integer of which input stream to pull from
     * \param abs_start    a uint64 count of the start of the range of interest
     * \param abs_end      a uint64 count of the end of the range of interest
     * \param key          a PMT symbol to select only tags of this key
     * \param id           Block ID
     */
    void get_tags_in_range(std::vector<tag_t> &v,
                           unsigned int which_input,
                           uint64_t abs_start,
                           uint64_t abs_end,
			   const pmt::pmt_t &key,
			   long id);

    /*!
     * \brief Set core affinity of block to the cores in the vector
     * mask.
     *
     * \param mask a vector of ints of the core numbers available to
     * this block.
     */
    void set_processor_affinity(const std::vector<int> &mask);

    /*!
     * \brief Unset core affinity.
     */
    void unset_processor_affinity();

    /*!
     * \brief Get the current thread priority
     */
    int thread_priority();

    /*!
     * \brief Set the current thread priority
     *
     * \param priority the new thread priority to set
     */
    int set_thread_priority(int priority);

    bool                    threaded;  // set if thread is currently running.
    gr::thread::gr_thread_t thread;    // portable thread handle

    void start_perf_counters();
    void stop_perf_counters(int noutput_items, int nproduced);
    void reset_perf_counters();

    // Calls to get performance counter items
    float pc_noutput_items();
    float pc_nproduced();
    float pc_input_buffers_full(size_t which);
    std::vector<float> pc_input_buffers_full();
    float pc_output_buffers_full(size_t which);
    std::vector<float> pc_output_buffers_full();
    float pc_work_time();

    float pc_noutput_items_avg();
    float pc_nproduced_avg();
    float pc_input_buffers_full_avg(size_t which);
    std::vector<float> pc_input_buffers_full_avg();
    float pc_output_buffers_full_avg(size_t which);
    std::vector<float> pc_output_buffers_full_avg();
    float pc_work_time_avg();
    float pc_throughput_avg();

    float pc_noutput_items_var();
    float pc_nproduced_var();
    float pc_input_buffers_full_var(size_t which);
    std::vector<float> pc_input_buffers_full_var();
    float pc_output_buffers_full_var(size_t which);
    std::vector<float> pc_output_buffers_full_var();
    float pc_work_time_var();

    float pc_work_time_total();

    tpb_detail d_tpb;	// used by thread-per-block scheduler
    int d_produce_or;

    int consumed() const;

    // ----------------------------------------------------------------------------

  private:
    unsigned int                    d_ninputs;
    unsigned int                    d_noutputs;
    std::vector<buffer_reader_sptr> d_input;
    std::vector<buffer_sptr>        d_output;
    bool                            d_done;
    int                             d_consumed;

    // Performance counters
    float d_ins_noutput_items;
    float d_avg_noutput_items;
    float d_var_noutput_items;
    float d_total_noutput_items;
    gr::high_res_timer_type d_pc_start_time;
    gr::high_res_timer_type d_pc_last_work_time;
    float d_ins_nproduced;
    float d_avg_nproduced;
    float d_var_nproduced;
    std::vector<float> d_ins_input_buffers_full;
    std::vector<float> d_avg_input_buffers_full;
    std::vector<float> d_var_input_buffers_full;
    std::vector<float> d_ins_output_buffers_full;
    std::vector<float> d_avg_output_buffers_full;
    std::vector<float> d_var_output_buffers_full;
    gr::high_res_timer_type d_start_of_work, d_end_of_work;
    float d_ins_work_time;
    float d_avg_work_time;
    float d_var_work_time;
    float d_total_work_time;
    float d_avg_throughput;
    float d_pc_counter;

    block_detail(unsigned int ninputs, unsigned int noutputs);

    friend struct tpb_detail;

    friend GR_RUNTIME_API block_detail_sptr
      make_block_detail(unsigned int ninputs, unsigned int noutputs);
  };

  GR_RUNTIME_API block_detail_sptr
  make_block_detail(unsigned int ninputs, unsigned int noutputs);

  GR_RUNTIME_API long
  block_detail_ncurrently_allocated();

} /* namespace gr */

#endif /* INCLUDED_GR_RUNTIME_BLOCK_DETAIL_H */
