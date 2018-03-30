/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2009,2010,2013,2017 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_RUNTIME_BLOCK_H
#define INCLUDED_GR_RUNTIME_BLOCK_H

#include <gnuradio/api.h>
#include <gnuradio/basic_block.h>
#include <gnuradio/tags.h>
#include <gnuradio/logger.h>

namespace gr {

  /*!
   * \brief The abstract base class for all 'terminal' processing blocks.
   * \ingroup base_blk
   *
   * A signal processing flow is constructed by creating a tree of
   * hierarchical blocks, which at any level may also contain terminal
   * nodes that actually implement signal processing functions. This
   * is the base class for all such leaf nodes.
   *
   * Blocks have a set of input streams and output streams.  The
   * input_signature and output_signature define the number of input
   * streams and output streams respectively, and the type of the data
   * items in each stream.
   *
   * Blocks report the number of items consumed on each input in
   * general_work(), using consume() or consume_each().
   *
   * If the same number of items is produced on each output, the block
   * returns that number from general_work(). Otherwise, the block
   * calls produce() for each output, then returns
   * WORK_CALLED_PRODUCE. The input and output rates are not required
   * to be related.
   *
   * User derived blocks override two methods, forecast and
   * general_work, to implement their signal processing
   * behavior. forecast is called by the system scheduler to determine
   * how many items are required on each input stream in order to
   * produce a given number of output items.
   *
   * general_work is called to perform the signal processing in the
   * block.  It reads the input items and writes the output items.
   */
  class GR_RUNTIME_API block : public basic_block
  {
  public:

    //! Magic return values from general_work
    enum {
      WORK_CALLED_PRODUCE = -2,
      WORK_DONE = -1
    };

    /*!
     * \brief enum to represent different tag propagation policies.
     */
    enum tag_propagation_policy_t {
      TPP_DONT = 0, /*!< Scheduler doesn't propagate tags from in- to output. The block itself is free to insert tags as it wants. */
      TPP_ALL_TO_ALL = 1, /*!< Propagate tags from all in- to all outputs. The scheduler takes care of that. */
      TPP_ONE_TO_ONE = 2, /*!< Propagate tags from n. input to n. output. Requires same number of in- and outputs */
      TPP_CUSTOM = 3 /*!< Like TPP_DONT, but signals the block it should implement application-specific forwarding behaviour. */
    };

    virtual ~block();

    /*!
     * Assume block computes y_i = f(x_i, x_i-1, x_i-2, x_i-3...)
     * History is the number of x_i's that are examined to produce one y_i.
     * This comes in handy for FIR filters, where we use history to
     * ensure that our input contains the appropriate "history" for the
     * filter. History should be equal to the number of filter taps. First
     * history samples (when there are no previous samples) are
     * initialized with zeroes.
     */
    unsigned history() const;
    void  set_history(unsigned history);

    /*!
     * Declares the block's delay in samples. Since the delay of
     * blocks like filters is derived from the taps and not the block
     * itself, we cannot automatically calculate this value and so
     * leave it as a user-defined property. It defaults to 0 is not
     * set.
     *
     * This does not actively set the delay; it just tells the
     * scheduler what the delay is.
     *
     * This delay is mostly used to adjust the placement of the tags
     * and is not currently used for any signal processing. When a tag
     * is passed through a block with internal delay, its location
     * should be moved based on the delay of the block. This interface
     * allows us to tell the scheduler this value.
     *
     * \param which The buffer on which to set the delay.
     * \param delay The sample delay of the data stream.
     */
    void declare_sample_delay(int which, unsigned delay);

    /*!
     * Convenience wrapper to gr::block::declare_delay(int which, unsigned delay)
     * to set all ports to the same delay.
     */
    void declare_sample_delay(unsigned delay);

    /*!
     * Gets the delay of the block. Since the delay of blocks like
     * filters is derived from the taps and not the block itself, we
     * cannot automatically calculate this value and so leave it as a
     * user-defined property. It defaults to 0 is not set.
     *
     * \param which Which port from which to get the sample delay.
     */
    unsigned sample_delay(int which) const;

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
     * Given a request to product \p noutput_items, estimate the
     * number of data items required on each input stream.  The
     * estimate doesn't have to be exact, but should be close.
     */
    virtual void forecast(int noutput_items,
                          gr_vector_int &ninput_items_required);

    /*!
     * \brief compute output items from input items
     *
     * \param noutput_items	number of output items to write on each output stream
     * \param ninput_items	number of input items available on each input stream
     * \param input_items	vector of pointers to the input items, one entry per input stream
     * \param output_items	vector of pointers to the output items, one entry per output stream
     *
     * \returns number of items actually written to each output stream
     * or WORK_CALLED_PRODUCE or WORK_DONE.  It is OK to return a
     * value less than noutput_items.
     *
     * WORK_CALLED_PRODUCE is used where not all outputs produce the
     * same number of items. general_work must call produce() for each
     * output to indicate the numer of items actually produced.
     *
     * WORK_DONE indicates that no more data will be produced by this block.
     *
     * general_work must call consume or consume_each to indicate how
     * many items were consumed on each input stream.
     */
    virtual int general_work(int noutput_items,
                             gr_vector_int &ninput_items,
                             gr_vector_const_void_star &input_items,
                             gr_vector_void_star &output_items);

    /*!
     * \brief Called to enable drivers, etc for i/o devices.
     *
     * This allows a block to enable an associated driver to begin
     * transferring data just before we start to execute the scheduler.
     * The end result is that this reduces latency in the pipeline
     * when dealing with audio devices, usrps, etc.
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
     * set_output_multiple causes the scheduler to ensure that the
     * noutput_items argument passed to forecast and general_work will
     * be an integer multiple of \param multiple The default value of
     * output multiple is 1.
     */
    void set_output_multiple(int multiple);
    int  output_multiple() const { return d_output_multiple; }
    bool output_multiple_set() const { return d_output_multiple_set; }

    /*!
     * \brief Constrains buffers to work on a set item alignment (for SIMD)
     *
     * set_alignment_multiple causes the scheduler to ensure that the
     * noutput_items argument passed to forecast and general_work will
     * be an integer multiple of \param multiple The default value is
     * 1.
     *
     * This control is similar to the output_multiple setting, except
     * that if the number of items passed to the block is less than
     * the output_multiple, this value is ignored and the block can
     * produce like normal. The d_unaligned value is set to the number
     * of items the block is off by. In the next call to general_work,
     * the noutput_items is set to d_unaligned or less until
     * d_unaligned==0. The buffers are now aligned again and the
     * aligned calls can be performed again.
     */
    void set_alignment(int multiple);
    int  alignment() const { return d_output_multiple; }

    void set_unaligned(int na);
    int  unaligned() const { return d_unaligned; }
    void set_is_unaligned(bool u);
    bool is_unaligned() const { return d_is_unaligned; }

    /*!
     * \brief Tell the scheduler \p how_many_items of input stream \p
     * which_input were consumed.
     *
     * This function should be used in general_work() to tell the scheduler the
     * number of input items processed. Calling consume() multiple times in the
     * same general_work() call is safe. Every invocation of consume() updates
     * the values returned by nitems_read().
     */
    void consume(int which_input, int how_many_items);

    /*!
     * \brief Tell the scheduler \p how_many_items were consumed on
     * each input stream.
     *
     * Also see notes on consume().
     */
    void consume_each(int how_many_items);

    /*!
     * \brief Tell the scheduler \p how_many_items were produced on
     * output stream \p which_output.
     *
     * This function should be used in general_work() to tell the scheduler the
     * number of output items produced. If produce() is called in
     * general_work(), general_work() must return \p WORK_CALLED_PRODUCE.
     * Calling produce() multiple times in the same general_work() call is safe.
     * Every invocation of produce() updates the values returned by
     * nitems_written().
     */
    void produce(int which_output, int how_many_items);

    /*!
     * \brief Set the approximate output rate / input rate
     *
     * Provide a hint to the buffer allocator and scheduler.
     * The default relative_rate is 1.0
     *
     * decimators have relative_rates < 1.0
     * interpolators have relative_rates > 1.0
     */
    void set_relative_rate(double relative_rate);

    /*!
     * \brief return the approximate output rate / input rate
     */
    double relative_rate() const { return d_relative_rate; }

    /*
     * The following two methods provide special case info to the
     * scheduler in the event that a block has a fixed input to output
     * ratio.  sync_block, sync_decimator and
     * sync_interpolator override these.  If you're fixed rate,
     * subclass one of those.
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

    /*!
     * \brief Return the minimum number of output items this block can
     * produce during a call to work.
     *
     * Should be 0 for most blocks.  Useful if we're dealing with
     * packets and the block produces one packet per call to work.
     */
    int min_noutput_items() const { return d_min_noutput_items; }

    /*!
     * \brief Set the minimum number of output items this block can
     * produce during a call to work.
     *
     * \param m the minimum noutput_items this block can produce.
     */
    void set_min_noutput_items(int m) { d_min_noutput_items = m; }

    /*!
     * \brief Return the maximum number of output items this block will
     * handle during a call to work.
     */
    int max_noutput_items();

    /*!
     * \brief Set the maximum number of output items this block will
     * handle during a call to work.
     *
     * \param m the maximum noutput_items this block will handle.
     */
    void set_max_noutput_items(int m);

    /*!
     * \brief Clear the switch for using the max_noutput_items value of this block.
     *
     * When is_set_max_noutput_items() returns 'true', the scheduler
     * will use the value returned by max_noutput_items() to limit the
     * size of the number of items possible for this block's work
     * function. If is_set_max_notput_items() returns 'false', then
     * the scheduler ignores the internal value and uses the value set
     * globally in the top_block.
     *
     * Use this value to clear the 'is_set' flag so the scheduler will
     * ignore this. Use the set_max_noutput_items(m) call to both set
     * a new value for max_noutput_items and to re-enable its use in
     * the scheduler.
     */
    void unset_max_noutput_items();

    /*!
     * \brief Ask the block if the flag is or is not set to use the
     * internal value of max_noutput_items during a call to work.
     */
    bool is_set_max_noutput_items();

    /*
     * Used to expand the vectors that hold the min/max buffer sizes.
     *
     * Specifically, when -1 is used, the vectors are just initialized
     * with 1 value; this is used by the flat_flowgraph to expand when
     * required to add a new value for new ports on these blocks.
     */
    void expand_minmax_buffer(int port);

    /*!
     * \brief Returns max buffer size on output port \p i.
     */
    long max_output_buffer(size_t i);

    /*!
     * \brief Request limit on max buffer size on all output ports.
     *
     * \details
     * This is an advanced feature. Calling this can affect some
     * fundamental assumptions about the system behavior and
     * performance.
     *
     * The actual buffer size is determined by a number of other
     * factors from the block and system. This function only provides
     * a requested maximum. The buffers will always be a multiple of
     * the system page size, which may be larger than the value asked
     * for here.
     *
     * \param max_output_buffer the requested maximum output size in items.
     */
    void set_max_output_buffer(long max_output_buffer);

    /*!
     * \brief Request limit on max buffer size on output port \p port.
     *
     * \details
     * This is an advanced feature. Calling this can affect some
     * fundamental assumptions about the system behavior and
     * performance.
     *
     * The actual buffer size is determined by a number of other
     * factors from the block and system. This function only provides
     * a requested maximum. The buffers will always be a multiple of
     * the system page size, which may be larger than the value asked
     * for here.
     *
     * \param port the output port the request applies to.
     * \param max_output_buffer the requested maximum output size in items.
     */
    void set_max_output_buffer(int port, long max_output_buffer);

    /*!
     * \brief Returns min buffer size on output port \p i.
     */
    long min_output_buffer(size_t i);

    /*!
     * \brief Request limit on the minimum buffer size on all output
     * ports.
     *
     * \details
     * This is an advanced feature. Calling this can affect some
     * fundamental assumptions about the system behavior and
     * performance.
     *
     * The actual buffer size is determined by a number of other
     * factors from the block and system. This function only provides
     * a requested minimum. The buffers will always be a multiple of
     * the system page size, which may be larger than the value asked
     * for here.
     *
     * \param min_output_buffer the requested minimum output size in items.
     */
    void set_min_output_buffer(long min_output_buffer);

    /*!
     * \brief Request limit on min buffer size on output port \p port.
     *
     * \details
     * This is an advanced feature. Calling this can affect some
     * fundamental assumptions about the system behavior and
     * performance.
     *
     * The actual buffer size is determined by a number of other
     * factors from the block and system. This function only provides
     * a requested minimum. The buffers will always be a multiple of
     * the system page size, which may be larger than the value asked
     * for here.
     *
     * \param port the output port the request applies to.
     * \param min_output_buffer the requested minimum output size in items.
     */
    void set_min_output_buffer(int port, long min_output_buffer);

    // --------------- Performance counter functions -------------

    /*!
     * \brief Gets instantaneous noutput_items performance counter.
     */
    float pc_noutput_items();

    /*!
     * \brief Gets average noutput_items performance counter.
     */
    float pc_noutput_items_avg();

    /*!
     * \brief Gets variance of noutput_items performance counter.
     */
    float pc_noutput_items_var();

    /*!
     * \brief Gets instantaneous num items produced performance counter.
     */
    float pc_nproduced();

    /*!
     * \brief Gets average num items produced performance counter.
     */
    float pc_nproduced_avg();

    /*!
     * \brief Gets variance of  num items produced performance counter.
     */
    float pc_nproduced_var();

    /*!
     * \brief Gets instantaneous fullness of \p which input buffer.
     */
    float pc_input_buffers_full(int which);

    /*!
     * \brief Gets average fullness of \p which input buffer.
     */
    float pc_input_buffers_full_avg(int which);

    /*!
     * \brief Gets variance of fullness of \p which input buffer.
     */
    float pc_input_buffers_full_var(int which);

    /*!
     * \brief Gets instantaneous fullness of all input buffers.
     */
    std::vector<float> pc_input_buffers_full();

    /*!
     * \brief Gets average fullness of all input buffers.
     */
    std::vector<float> pc_input_buffers_full_avg();

    /*!
     * \brief Gets variance of fullness of all input buffers.
     */
    std::vector<float> pc_input_buffers_full_var();

    /*!
     * \brief Gets instantaneous fullness of \p which input buffer.
     */
    float pc_output_buffers_full(int which);

    /*!
     * \brief Gets average fullness of \p which input buffer.
     */
    float pc_output_buffers_full_avg(int which);

    /*!
     * \brief Gets variance of fullness of \p which input buffer.
     */
    float pc_output_buffers_full_var(int which);

    /*!
     * \brief Gets instantaneous fullness of all output buffers.
     */
    std::vector<float> pc_output_buffers_full();

    /*!
     * \brief Gets average fullness of all output buffers.
     */
    std::vector<float> pc_output_buffers_full_avg();

    /*!
     * \brief Gets variance of fullness of all output buffers.
     */
    std::vector<float> pc_output_buffers_full_var();

    /*!
     * \brief Gets instantaneous clock cycles spent in work.
     */
    float pc_work_time();

    /*!
     * \brief Gets average clock cycles spent in work.
     */
    float pc_work_time_avg();

    /*!
     * \brief Gets average clock cycles spent in work.
     */
    float pc_work_time_var();

    /*!
     * \brief Gets total clock cycles spent in work.
     */
    float pc_work_time_total();

    /*!
     * \brief Gets average throughput.
     */
    float pc_throughput_avg();

    /*!
     * \brief Resets the performance counters
     */
    void reset_perf_counters();

    /*!
     * \brief Sets up export of perf. counters to ControlPort. Only
     * called by the scheduler.
     */
    void setup_pc_rpc();

    /*!
     * \brief Checks if this block is already exporting perf. counters
     * to ControlPort.
     */
    bool is_pc_rpc_set() { return d_pc_rpc_set; }

    /*!
     * \brief If the block calls this in its constructor, it's
     * perf. counters will not be exported.
     */
    void no_pc_rpc() { d_pc_rpc_set = true; }


    // ----------------------------------------------------------------------------
    // Functions to handle thread affinity

    /*!
     * \brief Set the thread's affinity to processor core \p n.
     *
     * \param mask a vector of ints of the core numbers available to this block.
     */
    void set_processor_affinity(const std::vector<int> &mask);

    /*!
     * \brief Remove processor affinity to a specific core.
     */
    void unset_processor_affinity();

    /*!
     * \brief Get the current processor affinity.
     */
    std::vector<int> processor_affinity() { return d_affinity; }

    /*!
     * \brief Get the current thread priority in use
     */
    int active_thread_priority();

    /*!
     * \brief Get the current thread priority stored
     */
    int thread_priority();

    /*!
     * \brief Set the current thread priority
     */
    int set_thread_priority(int priority);

    bool update_rate() const;

    // ----------------------------------------------------------------------------

	/*!
	 * \brief the system message handler
     */
    void system_handler(pmt::pmt_t msg);

	/*!
     * \brief returns true when execution has completed due to a message connection
    */
    bool finished();

  private:
    int                   d_output_multiple;
    bool                  d_output_multiple_set;
    int                   d_unaligned;
    bool                  d_is_unaligned;
    double                d_relative_rate;	// approx output_rate / input_rate
    block_detail_sptr     d_detail;		// implementation details
    unsigned              d_history;
    unsigned              d_attr_delay;         // the block's sample delay
    bool                  d_fixed_rate;
    bool                  d_max_noutput_items_set;     // if d_max_noutput_items is valid
    int                   d_max_noutput_items;         // value of max_noutput_items for this block
    int                   d_min_noutput_items;
    tag_propagation_policy_t d_tag_propagation_policy; // policy for moving tags downstream
    std::vector<int>      d_affinity;              // thread affinity proc. mask
    int                   d_priority;              // thread priority level
    bool                  d_pc_rpc_set;
    bool                  d_update_rate;           // should sched update rel rate?
    bool d_finished;    // true if msg ports think we are finished

  protected:
    block(void) {} // allows pure virtual interface sub-classes
    block(const std::string &name,
          gr::io_signature::sptr input_signature,
          gr::io_signature::sptr output_signature);

    void set_fixed_rate(bool fixed_rate) { d_fixed_rate = fixed_rate; }

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
      tag_t tag;
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
    void add_item_tag(unsigned int which_output, const tag_t &tag);

    /*!
     * \brief DEPRECATED. Will be removed in 3.8.
     *
     * \param which_input an integer of which input stream to remove the tag from
     * \param abs_offset   a uint64 number of the absolute item number
     *                     assicated with the tag. Can get from nitems_written.
     * \param key          the tag key as a PMT symbol
     * \param value        any PMT holding any value for the given key
     * \param srcid        optional source ID specifier; defaults to PMT_F
     *
     * If no such tag is found, does nothing.
     */
    inline void remove_item_tag(unsigned int which_input,
                                uint64_t abs_offset,
                                const pmt::pmt_t &key,
                                const pmt::pmt_t &value,
                                const pmt::pmt_t &srcid=pmt::PMT_F)
    {
      tag_t tag;
      tag.offset = abs_offset;
      tag.key = key;
      tag.value = value;
      tag.srcid = srcid;
      this->remove_item_tag(which_input, tag);
    }

    /*!
     * \brief DEPRECATED. Will be removed in 3.8.
     *
     * \param which_input an integer of which input stream to remove the tag from
     * \param tag the tag object to remove
     */
    void remove_item_tag(unsigned int which_input, const tag_t &tag);

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
    void get_tags_in_range(std::vector<tag_t> &v,
                           unsigned int which_input,
                           uint64_t abs_start,
                           uint64_t abs_end);

    /*!
     * \brief Given a [start,end), returns a vector of all tags in the
     * range with a given key.
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
    void get_tags_in_range(std::vector<tag_t> &v,
                           unsigned int which_input,
                           uint64_t abs_start,
                           uint64_t abs_end,
                           const pmt::pmt_t &key);

    /*!
     * \brief Gets all tags within the relative window of the current call to work.
     *
     * \details
     *
     * This opperates much like get_tags_in_range but allows us to
     * work within the current window of items. Item range is
     * therefore within the possible range of 0 to
     * ninput_items[whic_input].
     *
     * Range of items counts from \p rel_start to \p rel_end-1 within
     * current window.
     *
     * Tags are tuples of:
     *      (item count, source id, key, value)
     *
     * \param v            a vector reference to return tags into
     * \param which_input  an integer of which input stream to pull from
     * \param rel_start    a uint64 count of the start of the range of interest
     * \param rel_end      a uint64 count of the end of the range of interest
     */
    void get_tags_in_window(std::vector<tag_t> &v,
                            unsigned int which_input,
                            uint64_t rel_start,
                            uint64_t rel_end);

    /*!
     * \brief Operates like gr::block::get_tags_in_window with the
     * ability to only return tags with the specified \p key.
     *
     * \details
     *
     * \param v            a vector reference to return tags into
     * \param which_input  an integer of which input stream to pull from
     * \param rel_start    a uint64 count of the start of the range of interest
     * \param rel_end      a uint64 count of the end of the range of interest
     * \param key          a PMT symbol key to filter only tags of this key
     */
    void get_tags_in_window(std::vector<tag_t> &v,
                            unsigned int which_input,
                            uint64_t rel_start,
                            uint64_t rel_end,
                            const pmt::pmt_t &key);

    void enable_update_rate(bool en);

    std::vector<long> d_max_output_buffer;
    std::vector<long> d_min_output_buffer;

    /*! Used by block's setters and work functions to make
     * setting/resetting of parameters thread-safe.
     *
     * Used by calling gr::thread::scoped_lock l(d_setlock);
     */
    gr::thread::mutex d_setlock;

    /*! Used by blocks to access the logger system.
     */
    gr::logger_ptr d_logger;
    gr::logger_ptr d_debug_logger;

    // These are really only for internal use, but leaving them public avoids
    // having to work up an ever-varying list of friend GR_RUNTIME_APIs

    /*! PMT Symbol for "hey, we're done here"
     */
    const pmt::pmt_t d_pmt_done;

    /*! PMT Symbol of the system port, `pmt::mp("system")`
     */
    const pmt::pmt_t d_system_port;

  public:
    block_detail_sptr detail() const { return d_detail; }
    void set_detail(block_detail_sptr detail) { d_detail = detail; }

   /*! \brief Tell msg neighbors we are finished
	*/
   void notify_msg_neighbors();

   /*! \brief Make sure we don't think we are finished
	*/
   void clear_finished(){ d_finished = false; }

  };

  typedef std::vector<block_sptr> block_vector_t;
  typedef std::vector<block_sptr>::iterator block_viter_t;

  inline block_sptr cast_to_block_sptr(basic_block_sptr p)
  {
    return boost::dynamic_pointer_cast<block, basic_block>(p);
  }

  GR_RUNTIME_API std::ostream&
  operator << (std::ostream& os, const block *m);

} /* namespace gr */

#endif /* INCLUDED_GR_RUNTIME_BLOCK_H */
