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
    /*
     * This function is called by the runtime system to dispatch messages.
     *
     * The thread-safety guarantees mentioned in set_msg_handler are implemented
     * by the callers of this method.
     */
    void dispatch_msg(pmt::pmt_t msg)
    {
      if (d_msg_handler)	// Is there a handler?
	d_msg_handler(msg);	// Yes, invoke it.
    };

    msg_handler_t	 d_msg_handler;

protected:
    friend class gr_flowgraph;
    friend class gr_flat_flowgraph; // TODO: will be redundant
    friend class gr_tpb_thread_body;

    enum vcolor { WHITE, GREY, BLACK };

    std::string          d_name;
    gr_io_signature_sptr d_input_signature;
    gr_io_signature_sptr d_output_signature;
    long                 d_unique_id;
    vcolor               d_color;

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

public:
    virtual ~gr_basic_block();
    long unique_id() const { return d_unique_id; }
    std::string name() const { return d_name; }
    gr_io_signature_sptr input_signature() const  { return d_input_signature; }
    gr_io_signature_sptr output_signature() const { return d_output_signature; }
    gr_basic_block_sptr to_basic_block(); // Needed for Python/Guile type coercion

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
    virtual bool check_topology(int ninputs, int noutputs) { return true; }

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
    template <typename T> void set_msg_handler(T msg_handler){
      d_msg_handler = msg_handler_t(msg_handler);
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
