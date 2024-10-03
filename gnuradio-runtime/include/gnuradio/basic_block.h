/* -*- c++ -*- */
/*
 * Copyright 2006,2008,2009,2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_BASIC_BLOCK_H
#define INCLUDED_GR_BASIC_BLOCK_H

#include <gnuradio/api.h>
#include <gnuradio/io_signature.h>
#include <gnuradio/logger.h>
#include <gnuradio/msg_accepter.h>
#include <gnuradio/runtime_types.h>
#include <gnuradio/sptr_magic.h>
#include <gnuradio/thread/thread.h>
#include <boost/thread/condition_variable.hpp>
#include <deque>
#include <functional>
#include <map>
#include <string>

#include <gnuradio/rpcregisterhelpers.h>

namespace gr {

class msg_queue_comparator
{
private:
    const pmt::pmt_t d_system_port = pmt::intern("system");

public:
    bool operator()(pmt::pmt_t const& queue_key1, pmt::pmt_t const& queue_key2) const
    {
        if (pmt::eqv(queue_key2, d_system_port))
            return false;
        else if (pmt::eqv(queue_key1, d_system_port))
            return true;
        else {
            pmt::comparator cmp;
            return cmp(queue_key1, queue_key2);
        }
    }
};

/*!
 * \brief The abstract base class for all signal processing blocks.
 * \ingroup internal
 *
 * Basic blocks are the bare abstraction of an entity that has a
 * name, a set of inputs and outputs, and a message queue.  These
 * are never instantiated directly; rather, this is the abstract
 * parent class of both gr_hier_block, which is a recursive
 * container, and block, which implements actual signal
 * processing functions.
 */
class GR_RUNTIME_API basic_block : public msg_accepter,
                                   public std::enable_shared_from_this<basic_block>
{
    typedef std::function<void(pmt::pmt_t)> msg_handler_t;

private:
    typedef std::map<pmt::pmt_t, msg_handler_t, pmt::comparator> d_msg_handlers_t;
    d_msg_handlers_t d_msg_handlers;

    typedef std::deque<pmt::pmt_t> msg_queue_t;
    typedef std::map<pmt::pmt_t, msg_queue_t, msg_queue_comparator> msg_queue_map_t;
    typedef std::map<pmt::pmt_t, msg_queue_t, msg_queue_comparator>::iterator
        msg_queue_map_itr;

    gr::thread::mutex mutex; //< protects all vars

protected:
    friend class flowgraph;
    friend class flat_flowgraph; // TODO: will be redundant
    friend class tpb_thread_body;

    enum vcolor { WHITE, GREY, BLACK };

    std::string d_name;
    gr::io_signature::sptr d_input_signature;
    gr::io_signature::sptr d_output_signature;
    long d_unique_id;
    long d_symbolic_id;
    std::string d_symbol_name;
    std::string d_symbol_alias;
    vcolor d_color;
    bool d_rpc_set;

    /*! Used by blocks to access the logger system.
     */
    gr::logger_ptr d_logger;       //! Default logger
    gr::logger_ptr d_debug_logger; //! Verbose logger

    msg_queue_map_t msg_queue;
    std::vector<rpcbasic_sptr> d_rpc_vars; // container for all RPC variables

    basic_block(void) {} // allows pure virtual interface sub-classes

    //! Protected constructor prevents instantiation by non-derived classes
    basic_block(const std::string& name,
                gr::io_signature::sptr input_signature,
                gr::io_signature::sptr output_signature);

    //! may only be called during constructor
    void set_input_signature(gr::io_signature::sptr iosig) { d_input_signature = iosig; }

    //! may only be called during constructor
    void set_output_signature(gr::io_signature::sptr iosig)
    {
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
    virtual bool has_msg_handler(pmt::pmt_t which_port)
    {
        return (d_msg_handlers.find(which_port) != d_msg_handlers.end());
    }

    /*
     * This function is called by the runtime system to dispatch messages.
     *
     * The thread-safety guarantees mentioned in set_msg_handler are
     * implemented by the callers of this method.
     */
    virtual void dispatch_msg(pmt::pmt_t which_port, pmt::pmt_t msg)
    {
        // AA Update this
        if (has_msg_handler(which_port)) {   // Is there a handler?
            d_msg_handlers[which_port](msg); // Yes, invoke it.
        }
    }

    // Message passing interface
    pmt::pmt_t d_message_subscribers;

    /*!
     * \brief This is meant to be called by derived classes (e.g. block) to get
     * a shared pointer internally. This is needed because
     * std::enable_shared_from_this doesn't seem to work with derived classes
     * in an inheritance hierarchy.
     */
    template <typename Derived>
    std::shared_ptr<Derived> shared_from_base()
    {
        return std::static_pointer_cast<Derived>(shared_from_this());
    }

public:
    pmt::pmt_t message_subscribers(pmt::pmt_t port);
    ~basic_block() override;
    long unique_id() const { return d_unique_id; }
    long symbolic_id() const { return d_symbolic_id; }

    /*! The name of the block */
    std::string name() const { return d_name; }

    /*!
     * The symbolic name of the block, which is used in the
     * block_registry. The name is assigned by the block's constructor
     * and never changes during the life of the block.
     */
    std::string symbol_name() const { return d_symbol_name; }
    std::string identifier() const
    {
        return this->name() + "(" + std::to_string(this->unique_id()) + ")";
    }

    gr::io_signature::sptr input_signature() const { return d_input_signature; }
    gr::io_signature::sptr output_signature() const { return d_output_signature; }
    basic_block_sptr to_basic_block(); // Needed for Python type coercion

    /*!
     * True if the block has an alias (see set_block_alias).
     */
    bool alias_set() const { return !d_symbol_alias.empty(); }

    /*!
     * Returns the block's alias as a string.
     */
    std::string alias() const { return alias_set() ? d_symbol_alias : symbol_name(); }

    /*!
     * Returns the block's alias as PMT.
     */
    pmt::pmt_t alias_pmt() const { return pmt::intern(alias()); }

    /*!
     * Set's a new alias for the block; also adds an entry into the
     * block_registry to get the block using either the alias or the
     * original symbol name.
     */
    void set_block_alias(std::string name);

    // ** Message passing interface **
    void message_port_register_in(pmt::pmt_t port_id);
    void message_port_register_out(pmt::pmt_t port_id);
    void message_port_pub(pmt::pmt_t port_id, pmt::pmt_t msg);
    void message_port_sub(pmt::pmt_t port_id, pmt::pmt_t target);
    void message_port_unsub(pmt::pmt_t port_id, pmt::pmt_t target);

    virtual bool message_port_is_hier(pmt::pmt_t port_id)
    {
        (void)port_id;
        return false;
    }
    virtual bool message_port_is_hier_in(pmt::pmt_t port_id)
    {
        (void)port_id;
        return false;
    }
    virtual bool message_port_is_hier_out(pmt::pmt_t port_id)
    {
        (void)port_id;
        return false;
    }

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
    bool empty_p(pmt::pmt_t which_port)
    {
        if (msg_queue.find(which_port) == msg_queue.end())
            throw std::runtime_error("port does not exist!");
        return msg_queue[which_port].empty();
    }
    bool empty_p()
    {
        bool rv = true;
        for (const auto& i : msg_queue) {
            rv &= msg_queue[i.first].empty();
        }
        return rv;
    }

    //! are all msg ports with handlers empty?
    bool empty_handled_p(pmt::pmt_t which_port)
    {
        return (empty_p(which_port) || !has_msg_handler(which_port));
    }
    bool empty_handled_p()
    {
        bool rv = true;
        for (const auto& i : msg_queue) {
            rv &= empty_handled_p(i.first);
        }
        return rv;
    }

    //! How many messages in the queue?
    size_t nmsgs(pmt::pmt_t which_port)
    {
        if (msg_queue.find(which_port) == msg_queue.end())
            throw std::runtime_error("port does not exist!");
        return msg_queue[which_port].size();
    }

    //| Acquires and release the mutex
    void insert_tail(pmt::pmt_t which_port, pmt::pmt_t msg);
    /*!
     * \returns returns pmt at head of queue or pmt::pmt_t() if empty.
     */
    pmt::pmt_t delete_head_nowait(pmt::pmt_t which_port);

    msg_queue_t::iterator get_iterator(pmt::pmt_t which_port)
    {
        return msg_queue[which_port].begin();
    }

    void erase_msg(pmt::pmt_t which_port, msg_queue_t::iterator it)
    {
        msg_queue[which_port].erase(it);
    }

    virtual bool has_msg_port(pmt::pmt_t which_port)
    {
        if (msg_queue.find(which_port) != msg_queue.end()) {
            return true;
        }
        if (pmt::dict_has_key(d_message_subscribers, which_port)) {
            return true;
        }
        return false;
    }

    const msg_queue_map_t& get_msg_map(void) const { return msg_queue; }

#ifdef GR_CTRLPORT
    /*!
     * \brief Add an RPC variable (get or set).
     *
     * Using controlport, we create new getters/setters and need to
     * store them. Each block has a vector to do this, and these never
     * need to be accessed again once they are registered with the RPC
     * backend. This function takes a
     * std::shared_sptr<rpcbasic_base> so that when the block is
     * deleted, all RPC registered variables are cleaned up.
     *
     * \param s an rpcbasic_sptr of the new RPC variable register to store.
     */
    void add_rpc_variable(rpcbasic_sptr s) { d_rpc_vars.push_back(s); }
#endif /* GR_CTRLPORT */

    /*!
     * \brief Set up the RPC registered variables.
     *
     * This must be overloaded by a block that wants to use
     * controlport. This is where rpcbasic_register_{get,set} pointers
     * are created, which then get wrapped as shared pointers
     * (rpcbasic_sptr(...)) and stored using add_rpc_variable.
     */
    virtual void setup_rpc(){};

    /*!
     * \brief Ask if this block has been registered to the RPC.
     *
     * We can only register a block once, so we use this to protect us
     * from calling it multiple times.
     */
    bool is_rpc_set() { return d_rpc_set; }

    /*!
     * \brief When the block is registered with the RPC, set this.
     */
    void rpc_set() { d_rpc_set = true; }

    /*!
     * \brief Confirm that ninputs and noutputs is an acceptable combination.
     *
     * \param ninputs	number of input streams connected
     * \param noutputs	number of output streams connected
     *
     * \returns true if this is a valid configuration for this block.
     *
     * This function is called by the runtime system whenever the
     * topology changes. Most classes do not need to override this.
     * This check is in addition to the constraints specified by the
     * input and output gr::io_signatures.
     */
    virtual bool check_topology(int ninputs, int noutputs)
    {
        (void)ninputs;
        (void)noutputs;
        return true;
    }

    /*!
     * \brief Set the callback that is fired when messages are available.
     *
     * \p msg_handler can be any kind of function pointer or function object
     * that has the signature:
     * <pre>
     *    void msg_handler(pmt::pmt msg);
     * </pre>
     *
     * (You may want to use boost::bind to massage your callable into
     * the correct form.  See gr::blocks::nop for an example that sets
     * up a class method as the callback.)
     *
     * Blocks that desire to handle messages must call this method in
     * their constructors to register the handler that will be invoked
     * when messages are available.
     *
     * If the block inherits from block, the runtime system will
     * ensure that msg_handler is called in a thread-safe manner, such
     * that work and msg_handler will never be called concurrently.
     * This allows msg_handler to update state variables without
     * having to worry about thread-safety issues with work,
     * general_work or another invocation of msg_handler.
     *
     * If the block inherits from hier_block2, the runtime system
     * will ensure that no reentrant calls are made to msg_handler.
     */
    template <typename T>
    void set_msg_handler(pmt::pmt_t which_port, T msg_handler)
    {
        if (msg_queue.find(which_port) == msg_queue.end()) {
            throw std::runtime_error(
                "attempt to set_msg_handler() on bad input message port!");
        }
        d_msg_handlers[which_port] = msg_handler_t(msg_handler);
    }

    virtual void set_processor_affinity(const std::vector<int>& mask) = 0;

    virtual void unset_processor_affinity() = 0;

    virtual std::vector<int> processor_affinity() = 0;

    virtual void set_log_level(const std::string& level) = 0;

    virtual std::string log_level() = 0;
};

inline bool operator<(basic_block_sptr lhs, basic_block_sptr rhs)
{
    return lhs->unique_id() < rhs->unique_id();
}

typedef std::vector<basic_block_sptr> basic_block_vector_t;
typedef std::vector<basic_block_sptr>::iterator basic_block_viter_t;

GR_RUNTIME_API long basic_block_ncurrently_allocated();

inline std::ostream& operator<<(std::ostream& os, basic_block_sptr basic_block)
{
    os << basic_block->identifier();
    return os;
}

} /* namespace gr */

#endif /* INCLUDED_GR_BASIC_BLOCK_H */
