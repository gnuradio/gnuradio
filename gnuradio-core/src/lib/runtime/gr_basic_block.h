/* -*- c++ -*- */
/*
 * Copyright 2006,2008,2009,2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_BASIC_BLOCK_H
#define INCLUDED_GR_BASIC_BLOCK_H

#include <gr_core_api.h>
#include <gr_runtime_types.h>
#include <gr_sptr_magic.h>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <gr_msg_accepter.h>
#include <string>
#include <deque>
#include <map>
#include <gr_io_signature.h>
#include <gruel/thread.h>
#include <boost/foreach.hpp>
#include <boost/thread/condition_variable.hpp>
#include <iostream>

/*!
 * \brief The abstract base class for all signal processing blocks.
 * \ingroup internal
 *
 * Basic blocks are the bare abstraction of an entity that has a name,
 * a set of inputs and outputs, and a message queue.  These are never instantiated
 * directly; rather, this is the abstract parent class of both gr_hier_block,
 * which is a recursive container, and gr_block, which implements actual
 * signal processing functions.
 */

class GR_CORE_API gr_basic_block : public gr_msg_accepter, public boost::enable_shared_from_this<gr_basic_block>
{
  typedef boost::function<void(pmt::pmt_t)> msg_handler_t;
  
 private:
  
  //msg_handler_t	 d_msg_handler;
  typedef std::map<pmt::pmt_t , msg_handler_t, pmt::pmt_comperator> d_msg_handlers_t;
  d_msg_handlers_t d_msg_handlers;
  
  typedef std::deque<pmt::pmt_t>    msg_queue_t;
  typedef std::map<pmt::pmt_t, msg_queue_t, pmt::pmt_comperator>    msg_queue_map_t;
  typedef std::map<pmt::pmt_t, msg_queue_t, pmt::pmt_comperator>::iterator msg_queue_map_itr;
  std::map<pmt::pmt_t, boost::shared_ptr<boost::condition_variable>, pmt::pmt_comperator> msg_queue_ready;
  
  gruel::mutex          mutex;          //< protects all vars
  
 protected:
  friend class gr_flowgraph;
  friend class gr_flat_flowgraph; // TODO: will be redundant
  friend class gr_tpb_thread_body;
  
  enum vcolor { WHITE, GREY, BLACK };
  
  std::string          d_name;
  gr_io_signature_sptr d_input_signature;
  gr_io_signature_sptr d_output_signature;
  long                 d_unique_id;
  long                 d_symbolic_id;
  std::string          d_symbol_name;
  std::string          d_symbol_alias;
  vcolor               d_color;
  msg_queue_map_t msg_queue;
  
  gr_basic_block(void){} //allows pure virtual interface sub-classes
  
  //! Protected constructor prevents instantiation by non-derived classes
  gr_basic_block(const std::string &name,
		 gr_io_signature_sptr input_signature,
		 gr_io_signature_sptr output_signature);
  
  //! may only be called during constructor
  void set_input_signature(gr_io_signature_sptr iosig) {
    d_input_signature = iosig;
  }
  
  //! may only be called during constructor
  void set_output_signature(gr_io_signature_sptr iosig) {
    d_output_signature = iosig;
  }
  
  /*!
   * \brief Allow the flowgraph to set for sorting and partitioning
   */
  void set_color(vcolor color) { d_color = color; }
  vcolor color() const { return d_color; }

  /*!
   * \brief Tests if there is a handler attached to port \p which_port
   */
   bool has_msg_handler(pmt::pmt_t which_port) {
     return (d_msg_handlers.find(which_port) != d_msg_handlers.end());
   }

  /*
   * This function is called by the runtime system to dispatch messages.
   *
   * The thread-safety guarantees mentioned in set_msg_handler are implemented
   * by the callers of this method.
   */
  virtual void dispatch_msg(pmt::pmt_t which_port, pmt::pmt_t msg)
  {
    // AA Update this
    if(has_msg_handler(which_port)) {  // Is there a handler?
      d_msg_handlers[which_port](msg); // Yes, invoke it.
    }
  }
  
  // Message passing interface
  pmt::pmt_t message_subscribers;
  
 public:
  virtual ~gr_basic_block();
  long unique_id() const { return d_unique_id; }
  long symbolic_id() const { return d_symbolic_id; }
  std::string name() const { return d_name; }
  std::string symbol_name() const { return d_symbol_name; }
  gr_io_signature_sptr input_signature() const  { return d_input_signature; }
  gr_io_signature_sptr output_signature() const { return d_output_signature; }
  gr_basic_block_sptr to_basic_block(); // Needed for Python type coercion
  bool alias_set() { return !d_symbol_alias.empty(); }
  std::string alias(){ return alias_set()?d_symbol_alias:symbol_name(); }
  pmt::pmt_t alias_pmt(){ return pmt::pmt_intern(alias()); }
  void set_block_alias(std::string name);
  
  // ** Message passing interface **
  void message_port_register_in(pmt::pmt_t port_id);
  void message_port_register_out(pmt::pmt_t port_id);
  void message_port_pub(pmt::pmt_t port_id, pmt::pmt_t msg);
  void message_port_sub(pmt::pmt_t port_id, pmt::pmt_t target);
  void message_port_unsub(pmt::pmt_t port_id, pmt::pmt_t target);
  
  virtual bool message_port_is_hier(pmt::pmt_t port_id) { (void) port_id; std::cout << "is_hier\n"; return false; }
  virtual bool message_port_is_hier_in(pmt::pmt_t port_id) { (void) port_id; std::cout << "is_hier_in\n"; return false; }
  virtual bool message_port_is_hier_out(pmt::pmt_t port_id) { (void) port_id; std::cout << "is_hier_out\n"; return false; }
  
  /*!
   * \brief Get input message port names.
   *
   * Returns the available input message ports for a block. The
   * return object is a PMT vector that is filled with PMT symbols.
   */
  pmt::pmt_t message_ports_in();
  
  /*!
   * \brief Get output message port names.
   *
   * Returns the available output message ports for a block. The
   * return object is a PMT vector that is filled with PMT symbols.
   */
  pmt::pmt_t message_ports_out();
  
  /*!
   * Accept msg, place in queue, arrange for thread to be awakened if it's not already.
   */
  void _post(pmt::pmt_t which_port, pmt::pmt_t msg);
  
  //! is the queue empty?
  //bool empty_p(const pmt::pmt_t &which_port) const { return msg_queue[which_port].empty(); }
  bool empty_p(pmt::pmt_t which_port) { 
    if(msg_queue.find(which_port) == msg_queue.end())
      throw std::runtime_error("port does not exist!");
    return msg_queue[which_port].empty(); 
  }
  bool empty_p() { 
    bool rv = true;
    BOOST_FOREACH(msg_queue_map_t::value_type &i, msg_queue) {
      rv &= msg_queue[i.first].empty();
    }
    return rv;
  }

  //! How many messages in the queue?
  size_t nmsgs(pmt::pmt_t which_port) { 
    if(msg_queue.find(which_port) == msg_queue.end())
      throw std::runtime_error("port does not exist!");
    return msg_queue[which_port].size(); 
  }
  
  //| Acquires and release the mutex
  void insert_tail( pmt::pmt_t which_port, pmt::pmt_t msg);
  /*!
   * \returns returns pmt at head of queue or pmt_t() if empty.
   */
  pmt::pmt_t delete_head_nowait( pmt::pmt_t which_port);
  
  /*!
   * \returns returns pmt at head of queue or pmt_t() if empty.
   */
  pmt::pmt_t delete_head_blocking( pmt::pmt_t which_port);
  
  msg_queue_t::iterator get_iterator(pmt::pmt_t which_port){
    return msg_queue[which_port].begin();
  }

  void erase_msg(pmt::pmt_t which_port, msg_queue_t::iterator it){
    msg_queue[which_port].erase(it);
  }
  
  virtual bool has_msg_port(pmt::pmt_t which_port){
    if(msg_queue.find(which_port) != msg_queue.end()){
      return true;
    }
    if(pmt::pmt_dict_has_key(message_subscribers, which_port)){
      return true;
    }
    return false;
  }
  
  
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
  virtual bool check_topology(int ninputs, int noutputs) { (void) ninputs; (void) noutputs; return true; }
  
  /*!
   * \brief Set the callback that is fired when messages are available.
   *
   * \p msg_handler can be any kind of function pointer or function object
   * that has the signature:
   * <pre>
   *    void msg_handler(pmt::pmt msg);
   * </pre>
   *
   * (You may want to use boost::bind to massage your callable into the
   * correct form.  See gr_nop.{h,cc} for an example that sets up a class
   * method as the callback.)
   *
   * Blocks that desire to handle messages must call this method in their
   * constructors to register the handler that will be invoked when messages
   * are available.
   *
   * If the block inherits from gr_block, the runtime system will ensure that
   * msg_handler is called in a thread-safe manner, such that work and
   * msg_handler will never be called concurrently.  This allows msg_handler
   * to update state variables without having to worry about thread-safety
   * issues with work, general_work or another invocation of msg_handler.
   *
   * If the block inherits from gr_hier_block2, the runtime system will
   * ensure that no reentrant calls are made to msg_handler.
   */
  //template <typename T> void set_msg_handler(T msg_handler){
  //  d_msg_handler = msg_handler_t(msg_handler);
  //}
  template <typename T> void set_msg_handler(pmt::pmt_t which_port, T msg_handler){
    if(msg_queue.find(which_port) == msg_queue.end()){ 
      throw std::runtime_error("attempt to set_msg_handler() on bad input message port!"); }
    d_msg_handlers[which_port] = msg_handler_t(msg_handler);
  }
};

inline bool operator<(gr_basic_block_sptr lhs, gr_basic_block_sptr rhs)
{
  return lhs->unique_id() < rhs->unique_id();
}

typedef std::vector<gr_basic_block_sptr> gr_basic_block_vector_t;
typedef std::vector<gr_basic_block_sptr>::iterator gr_basic_block_viter_t;

GR_CORE_API long gr_basic_block_ncurrently_allocated();

inline std::ostream &operator << (std::ostream &os, gr_basic_block_sptr basic_block)
{
  os << basic_block->name() << "(" << basic_block->unique_id() << ")";
  return os;
}

#endif /* INCLUDED_GR_BASIC_BLOCK_H */
