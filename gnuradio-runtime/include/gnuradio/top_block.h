/* -*- c++ -*- */
/*
 * Copyright 2007-2009,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_TOP_BLOCK_H
#define INCLUDED_GR_TOP_BLOCK_H

#include <gnuradio/api.h>
#include <gnuradio/hier_block2.h>

namespace gr {

  class top_block_impl;

  GR_RUNTIME_API top_block_sptr make_top_block(const std::string &name);

  /*!
   *\brief Top-level hierarchical block representing a flowgraph
   * \ingroup container_blk
   */
  class GR_RUNTIME_API top_block : public hier_block2
  {
  private:
    friend GR_RUNTIME_API top_block_sptr
      make_top_block(const std::string &name);

    top_block_impl *d_impl;

  protected:
    top_block(const std::string &name);

  public:
    ~top_block();

    /*!
     * \brief The simple interface to running a flowgraph.
     *
     * Calls start() then wait(). Used to run a flowgraph that will
     * stop on its own, or when another thread will call stop().
     *
     * \param max_noutput_items the maximum number of output items
     * allowed for any block in the flowgraph. This passes through to
     * the start function; see that function for more details.
     */
    void run(int max_noutput_items=100000000);

    /*!
     * Start the contained flowgraph. Creates one or more threads to
     * execute the flow graph. Returns to the caller once the threads
     * are created. Calling start() on a top_block that is already
     * started IS an error.
     *
     * \param max_noutput_items the maximum number of output items
     * allowed for any block in the flowgraph; the noutput_items can
     * always be less than this, but this will cap it as a
     * maximum. Use this to adjust the maximum latency a flowgraph can
     * exhibit.
     */
    void start(int max_noutput_items=100000000);

    /*!
     * Stop the running flowgraph. Notifies each thread created by the
     * scheduler to shutdown, then returns to caller. Calling stop()
     * on a top_block that is already stopped IS NOT an error.
     */
    void stop();

    /*!
     * Wait for a flowgraph to complete. Flowgraphs complete when
     * either (1) all blocks indicate that they are done (typically
     * only when using blocks.file_source, or blocks.head, or (2)
     * after stop() has been called to request shutdown. Calling wait
     * on a top_block that is not running IS NOT an error (wait
     * returns w/o blocking).
     */
    void wait();

    /*!
     * Lock a flowgraph in preparation for reconfiguration. When an
     * equal number of calls to lock() and unlock() have occurred, the
     * flowgraph will be reconfigured.
     *
     * N.B. lock() and unlock() may not be called from a flowgraph
     * thread (E.g., block::work method) or deadlock will occur
     * when reconfiguration happens.
     */
    virtual void lock();

    /*!
     * Unlock a flowgraph in preparation for reconfiguration. When an
     * equal number of calls to lock() and unlock() have occurred, the
     * flowgraph will be reconfigured.
     *
     * N.B. lock() and unlock() may not be called from a flowgraph thread
     * (E.g., block::work method) or deadlock will occur when
     * reconfiguration happens.
     */
    virtual void unlock();

    /*!
     * Returns a string that lists the edge connections in the
     * flattened flowgraph.
     */
    std::string edge_list();

    /*!
     * Returns a string that lists the msg edge connections in the
     * flattened flowgraph.
     */
    std::string msg_edge_list();

    /*!
     * Displays flattened flowgraph edges and block connectivity
     */
    void dump();

    //! Get the number of max noutput_items in the flowgraph
    int max_noutput_items();

    //! Set the maximum number of noutput_items in the flowgraph
    void set_max_noutput_items(int nmax);

    top_block_sptr to_top_block(); // Needed for Python type coercion

    void setup_rpc();
  };

  inline top_block_sptr cast_to_top_block_sptr(basic_block_sptr block) {
    return boost::dynamic_pointer_cast<top_block, basic_block>(block);
  }

} /* namespce gr */

#endif /* INCLUDED_GR_TOP_BLOCK_H */
