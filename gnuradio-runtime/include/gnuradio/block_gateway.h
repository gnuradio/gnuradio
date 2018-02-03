/* -*- c++ -*- */
/*
 * Copyright 2011-2013,2017 Free Software Foundation, Inc.
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

#ifndef INCLUDED_RUNTIME_BLOCK_GATEWAY_H
#define INCLUDED_RUNTIME_BLOCK_GATEWAY_H

#include <gnuradio/api.h>
#include <gnuradio/block.h>
#include <gnuradio/feval.h>

namespace gr {

  /*!
   * The work type enum tells the gateway what kind of block to
   * implement.  The choices are familiar gnuradio block overloads
   * (sync, decim, interp).
   */
  enum block_gw_work_type {
    GR_BLOCK_GW_WORK_GENERAL,
    GR_BLOCK_GW_WORK_SYNC,
    GR_BLOCK_GW_WORK_DECIM,
    GR_BLOCK_GW_WORK_INTERP,
  };

  //! Magic return values from general_work, \ref gr::block::WORK_CALLED_PRODUCE
  enum block_gw_work_return_type{
    WORK_CALLED_PRODUCE = -2,
    WORK_DONE = -1
  };
  enum tag_propagation_policy_t {
    TPP_DONT = 0,
    TPP_ALL_TO_ALL = 1,
    TPP_ONE_TO_ONE = 2,
    TPP_CUSTOM = 3
  };

  /*!
   * Shared message structure between python and gateway.
   * Each action type represents a scheduler-called function.
   */
  struct block_gw_message_type {
    enum action_type {
      ACTION_GENERAL_WORK, //dispatch work
      ACTION_WORK, //dispatch work
      ACTION_FORECAST, //dispatch forecast
      ACTION_START, //dispatch start
      ACTION_STOP, //dispatch stop
    };

    action_type action;

    int general_work_args_noutput_items;
    std::vector<int> general_work_args_ninput_items;
    std::vector<void *> general_work_args_input_items; //TODO this should be const void*, but swig can't int cast it right
    std::vector<void *> general_work_args_output_items;
    int general_work_args_return_value;

    int work_args_ninput_items;
    int work_args_noutput_items;
    std::vector<void *> work_args_input_items; //TODO this should be const void*, but swig can't int cast it right
    std::vector<void *> work_args_output_items;
    int work_args_return_value;

    int forecast_args_noutput_items;
    std::vector<int> forecast_args_ninput_items_required;

    bool start_args_return_value;

    bool stop_args_return_value;
  };

  /*!
   * The gateway block which performs all the magic.
   *
   * The gateway provides access to all the gr::block routines.
   * The methods prefixed with gr::block__ are renamed
   * to class methods without the prefix in python.
   */
  class GR_RUNTIME_API block_gateway : virtual public gr::block
  {
  public:
    // gr::block_gateway::sptr
    typedef boost::shared_ptr<block_gateway> sptr;

    /*!
     * Make a new gateway block.
     * \param handler the swig director object with callback
     * \param name the name of the block (Ex: "Shirley")
     * \param in_sig the input signature for this block
     * \param out_sig the output signature for this block
     * \param work_type the type of block overload to implement
     * \param factor the decimation or interpolation factor
     * \return a new gateway block
     */
    static sptr make(gr::feval_ll *handler,
                     const std::string &name,
                     gr::io_signature::sptr in_sig,
                     gr::io_signature::sptr out_sig,
                     const block_gw_work_type work_type,
                     const unsigned factor);

    //! Provide access to the shared message object
    virtual block_gw_message_type &block_message(void) = 0;

    long block__unique_id(void) const {
      return gr::block::unique_id();
    }

    std::string block__name(void) const {
      return gr::block::name();
    }

    unsigned block__history(void) const {
      return gr::block::history();
    }

    void block__set_history(unsigned history) {
      return gr::block::set_history(history);
    }

    void block__set_fixed_rate(bool fixed_rate) {
      return gr::block::set_fixed_rate(fixed_rate);
    }

    bool block__fixed_rate(void) const {
      return gr::block::fixed_rate();
    }

    void block__set_output_multiple(int multiple) {
      return gr::block::set_output_multiple(multiple);
    }

    void block__set_min_output_buffer(int port, long size) {
      return gr::block::set_min_output_buffer(port, size);
    }

    void block__set_min_output_buffer(long size) {
      return gr::block::set_min_output_buffer(size);
    }

    int block__output_multiple(void) const {
      return gr::block::output_multiple();
    }

    void block__consume(int which_input, int how_many_items) {
      return gr::block::consume(which_input, how_many_items);
    }

    void block__consume_each(int how_many_items) {
      return gr::block::consume_each(how_many_items);
    }

    void block__produce(int which_output, int how_many_items) {
      return gr::block::produce(which_output, how_many_items);
    }

    void block__set_relative_rate(double relative_rate) {
      return gr::block::set_relative_rate(relative_rate);
    }

    double block__relative_rate(void) const {
      return gr::block::relative_rate();
    }

    uint64_t block__nitems_read(unsigned int which_input) {
      return gr::block::nitems_read(which_input);
    }

    uint64_t block__nitems_written(unsigned int which_output) {
      return gr::block::nitems_written(which_output);
    }

    block::tag_propagation_policy_t block__tag_propagation_policy(void) {
      return gr::block::tag_propagation_policy();
    }

    void block__set_tag_propagation_policy(block::tag_propagation_policy_t p) {
      return gr::block::set_tag_propagation_policy(p);
    }

    void block__add_item_tag(unsigned int which_output,
                             const tag_t &tag)
    {
      return gr::block::add_item_tag(which_output, tag);
    }

    void block__add_item_tag(unsigned int which_output,
                             uint64_t abs_offset,
                             const pmt::pmt_t &key,
                             const pmt::pmt_t &value,
                             const pmt::pmt_t &srcid=pmt::PMT_F)
    {
      return gr::block::add_item_tag(which_output, abs_offset,
                                     key, value, srcid);
    }

    std::vector<tag_t> block__get_tags_in_range(unsigned int which_input,
                                                uint64_t abs_start,
                                                uint64_t abs_end)
    {
      std::vector<gr::tag_t> tags;
      gr::block::get_tags_in_range(tags, which_input, abs_start, abs_end);
      return tags;
    }

    std::vector<tag_t> block__get_tags_in_range(unsigned int which_input,
                                                uint64_t abs_start,
                                                uint64_t abs_end,
                                                const pmt::pmt_t &key)
    {
      std::vector<gr::tag_t> tags;
      gr::block::get_tags_in_range(tags, which_input, abs_start, abs_end, key);
      return tags;
    }

    std::vector<tag_t> block__get_tags_in_window(unsigned int which_input,
                                                 uint64_t rel_start,
                                                 uint64_t rel_end)
    {
      std::vector<gr::tag_t> tags;
      gr::block::get_tags_in_window(tags, which_input, rel_start, rel_end);
      return tags;
    }

    std::vector<tag_t> block__get_tags_in_window(unsigned int which_input,
                                                 uint64_t rel_start,
                                                 uint64_t rel_end,
                                                 const pmt::pmt_t &key)
    {
      std::vector<gr::tag_t> tags;
      gr::block::get_tags_in_window(tags, which_input, rel_start, rel_end, key);
      return tags;
    }

    /* Message passing interface */
    void block__message_port_register_in(pmt::pmt_t port_id) {
      gr::basic_block::message_port_register_in(port_id);
    }

    void block__message_port_register_out(pmt::pmt_t port_id) {
      gr::basic_block::message_port_register_out(port_id);
    }

    void block__message_port_pub(pmt::pmt_t port_id, pmt::pmt_t msg) {
      gr::basic_block::message_port_pub(port_id, msg);
    }

    void block__message_port_sub(pmt::pmt_t port_id, pmt::pmt_t target) {
      gr::basic_block::message_port_sub(port_id, target);
    }

    void block__message_port_unsub(pmt::pmt_t port_id, pmt::pmt_t target) {
      gr::basic_block::message_port_unsub(port_id, target);
    }

    pmt::pmt_t block__message_subscribers(pmt::pmt_t which_port) {
      return gr::basic_block::message_subscribers(which_port);
    }

    pmt::pmt_t block__message_ports_in() {
      return gr::basic_block::message_ports_in();
    }

    pmt::pmt_t block__message_ports_out() {
      return gr::basic_block::message_ports_out();
    }

    void set_msg_handler_feval(pmt::pmt_t which_port, gr::feval_p *msg_handler)
    {
      if(msg_queue.find(which_port) == msg_queue.end()) {
        throw std::runtime_error("attempt to set_msg_handler_feval() on bad input message port!");
      }
      d_msg_handlers_feval[which_port] = msg_handler;
    }

  protected:
    typedef std::map<pmt::pmt_t, feval_p *, pmt::comparator> msg_handlers_feval_t;
    msg_handlers_feval_t d_msg_handlers_feval;

    bool has_msg_handler(pmt::pmt_t which_port)
    {
      return (d_msg_handlers_feval.find(which_port) != d_msg_handlers_feval.end());
    }

    void dispatch_msg(pmt::pmt_t which_port, pmt::pmt_t msg)
    {
      // Is there a handler?
      if(d_msg_handlers_feval.find(which_port) != d_msg_handlers_feval.end()) {
        d_msg_handlers_feval[which_port]->calleval(msg); // Yes, invoke it.
      }
      else {
        // Pass to generic dispatcher if not found
        gr::basic_block::dispatch_msg(which_port, msg);
      }
    }
  };

} /* namespace gr */

#endif /* INCLUDED_RUNTIME_BLOCK_GATEWAY_H */
