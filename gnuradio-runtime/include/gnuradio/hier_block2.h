/* -*- c++ -*- */
/*
 * Copyright 2006-2009,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_RUNTIME_HIER_BLOCK2_H
#define INCLUDED_GR_RUNTIME_HIER_BLOCK2_H

#include <gnuradio/api.h>
#include <gnuradio/basic_block.h>

namespace gr {

  /*!
   * \brief public constructor for hier_block2
   */
  GR_RUNTIME_API hier_block2_sptr
  make_hier_block2(const std::string &name,
                   gr::io_signature::sptr input_signature,
                   gr::io_signature::sptr output_signature);

  class hier_block2_detail;

  /*!
   * \brief Hierarchical container class for gr::block's and gr::hier_block2's
   * \ingroup container_blk
   * \ingroup base_blk
   */
  class GR_RUNTIME_API hier_block2 : public basic_block
  {
  private:
    friend class hier_block2_detail;
    friend GR_RUNTIME_API hier_block2_sptr
      make_hier_block2(const std::string &name,
                       gr::io_signature::sptr input_signature,
                       gr::io_signature::sptr output_signature);

    /*!
     * \brief Private implementation details of gr::hier_block2
     */
    hier_block2_detail *d_detail;


  protected:
    hier_block2(void) {} // allows pure virtual interface sub-classes
    hier_block2(const std::string &name,
                gr::io_signature::sptr input_signature,
                gr::io_signature::sptr output_signature);

  public:
    virtual ~hier_block2();

    /*!
     * \brief typedef for object returned from self().
     *
     * This type is only guaranteed to be passable to connect and
     * disconnect. No other assumptions should be made about it.
     */
    typedef basic_block_sptr opaque_self;

    /*!
     * \brief Return an object, representing the current block, which
     * can be passed to connect.
     *
     * The returned object may only be used as an argument to connect
     * or disconnect.  Any other use of self() results in unspecified
     * (erroneous) behavior.
     */
    opaque_self self();

    /*!
     * \brief Add a stand-alone (possibly hierarchical) block to
     * internal graph
     *
     * This adds a gr-block or hierarchical block to the internal
     * graph without wiring it to anything else.
     */
    void connect(basic_block_sptr block);

    /*!
     * \brief Add gr-blocks or hierarchical blocks to internal graph
     * and wire together
     *
     * This adds (if not done earlier by another connect) a pair of
     * gr-blocks or hierarchical blocks to the internal flowgraph, and
     * wires the specified output port to the specified input port.
     */
    void connect(basic_block_sptr src, int src_port,
                 basic_block_sptr dst, int dst_port);

    /*!
     * \brief Add gr-blocks or hierarchical blocks to internal graph
     * and wire together
     *
     * This adds (if not done earlier by another connect) a pair of
     * gr-blocks or hierarchical blocks to the internal message port
     * subscription
     */
    void msg_connect(basic_block_sptr src, pmt::pmt_t srcport,
                     basic_block_sptr dst, pmt::pmt_t dstport);
    void msg_connect(basic_block_sptr src, std::string srcport,
                     basic_block_sptr dst, std::string dstport);
    void msg_disconnect(basic_block_sptr src, pmt::pmt_t srcport,
                        basic_block_sptr dst, pmt::pmt_t dstport);
    void msg_disconnect(basic_block_sptr src, std::string srcport,
                        basic_block_sptr dst, std::string dstport);

    /*!
     * \brief Remove a gr-block or hierarchical block from the
     * internal flowgraph.
     *
     * This removes a gr-block or hierarchical block from the internal
     * flowgraph, disconnecting it from other blocks as needed.
     */
    void disconnect(basic_block_sptr block);

    /*!
     * \brief Disconnect a pair of gr-blocks or hierarchical blocks in
     *        internal flowgraph.
     *
     * This disconnects the specified input port from the specified
     * output port of a pair of gr-blocks or hierarchical blocks.
     */
    void disconnect(basic_block_sptr src, int src_port,
                    basic_block_sptr dst, int dst_port);

    /*!
     * \brief Disconnect all connections in the internal flowgraph.
     *
     * This call removes all output port to input port connections in
     * the internal flowgraph.
     */
    void disconnect_all();

    /*!
     * Lock a flowgraph in preparation for reconfiguration.  When an
     * equal number of calls to lock() and unlock() have occurred, the
     * flowgraph will be reconfigured.
     *
     * N.B. lock() and unlock() may not be called from a flowgraph
     * thread (E.g., gr::block::work method) or deadlock will occur
     * when reconfiguration happens.
     */
    virtual void lock();

    /*!
     * Unlock a flowgraph in preparation for reconfiguration.  When an
     * equal number of calls to lock() and unlock() have occurred, the
     * flowgraph will be reconfigured.
     *
     * N.B. lock() and unlock() may not be called from a flowgraph
     * thread (E.g., gr::block::work method) or deadlock will occur
     * when reconfiguration happens.
     */
    virtual void unlock();

    /*!
     * \brief Returns max buffer size (itemcount) on output port \p i.
     */
    int max_output_buffer(size_t port=0);

    /*!
     * \brief Sets max buffer size (itemcount) on all output ports.
     */
    void set_max_output_buffer(int max_output_buffer);

    /*!
     * \brief Sets max buffer size (itemcount) on output port \p port.
     */
    void set_max_output_buffer(size_t port, int max_output_buffer);

    /*!
     * \brief Returns min buffer size (itemcount) on output port \p i.
     */
    int min_output_buffer(size_t port=0);

    /*!
     * \brief Sets min buffer size (itemcount) on all output ports.
     */
    void set_min_output_buffer(int min_output_buffer);

    /*!
     * \brief Sets min buffer size (itemcount) on output port \p port.
     */
    void set_min_output_buffer(size_t port, int min_output_buffer);


    // This is a public method for ease of code organization, but should be
    // ignored by the user.
    flat_flowgraph_sptr flatten() const;

    hier_block2_sptr to_hier_block2(); // Needed for Python type coercion

    bool has_msg_port(pmt::pmt_t which_port) {
      return message_port_is_hier(which_port) || basic_block::has_msg_port(which_port);
    }

    bool message_port_is_hier(pmt::pmt_t port_id) {
      return message_port_is_hier_in(port_id) || message_port_is_hier_out(port_id);
    }

    bool message_port_is_hier_in(pmt::pmt_t port_id) {
      return pmt::list_has(hier_message_ports_in, port_id);
    }

    bool message_port_is_hier_out(pmt::pmt_t port_id) {
      return pmt::list_has(hier_message_ports_out, port_id);
    }

    pmt::pmt_t hier_message_ports_in;
    pmt::pmt_t hier_message_ports_out;

    void message_port_register_hier_in(pmt::pmt_t port_id) {
      if(pmt::list_has(hier_message_ports_in, port_id))
        throw std::invalid_argument("hier msg in port by this name already registered");
      if(msg_queue.find(port_id) != msg_queue.end())
        throw std::invalid_argument("block already has a primitive input port by this name");
      hier_message_ports_in = pmt::list_add(hier_message_ports_in, port_id);
    }

    void message_port_register_hier_out(pmt::pmt_t port_id) {
      if(pmt::list_has(hier_message_ports_out, port_id))
        throw std::invalid_argument("hier msg out port by this name already registered");
      if(pmt::dict_has_key(d_message_subscribers, port_id))
        throw std::invalid_argument("block already has a primitive output port by this name");
      hier_message_ports_out = pmt::list_add(hier_message_ports_out, port_id);
    }

    /*!
     * \brief Set the affinity of all blocks in hier_block2 to processor core \p n.
     *
     * \param mask a vector of ints of the core numbers available to this block.
     */
    void set_processor_affinity(const std::vector<int> &mask);

    /*!
     * \brief Remove processor affinity for all blocks in hier_block2.
     */
    void unset_processor_affinity();

    /*!
     * \brief Get the current processor affinity.
     *
     * \details This returns the processor affinity value for the first
     * block in the hier_block2's list of blocks with the assumption
     * that they have always only been set through the hier_block2's
     * interface. If any block has been individually set, then this
     * call could be misleading.
     */
    std::vector<int> processor_affinity();

    /*!
     * \brief Get if all block min buffers should be set.
     *
     * \details this returns whether all the block min output buffers
     * should be set or just the block ports connected to the hier ports.
     */
    bool all_min_output_buffer_p(void);

    /*!
     * \brief Get if all block max buffers should be set.
     *
     * \details this returns whether all the block max output buffers
     * should be set or just the block ports connected to the hier ports.
     */
    bool all_max_output_buffer_p(void);
  };

  /*!
   * \brief Return hierarchical block's flow graph represented in dot language
   */
  GR_RUNTIME_API std::string dot_graph(hier_block2_sptr hierblock2);

  inline hier_block2_sptr cast_to_hier_block2_sptr(basic_block_sptr block) {
    return boost::dynamic_pointer_cast<hier_block2, basic_block>(block);
  }

} /* namespace gr */

#endif /* INCLUDED_GR_RUNTIME_HIER_BLOCK2_H */
