/* -*- c++ -*- */
/*
 * Copyright 2006,2007,2008,2009 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GR_HIER_BLOCK2_H
#define INCLUDED_GR_HIER_BLOCK2_H

#include <gr_core_api.h>
#include <gr_basic_block.h>

/*!
 * \brief public constructor for gr_hier_block2

 */
GR_CORE_API gr_hier_block2_sptr gr_make_hier_block2(const std::string &name,
                                        gr_io_signature_sptr input_signature,
                                        gr_io_signature_sptr output_signature);

class gr_hier_block2_detail;

/*!
 * \brief Hierarchical container class for gr_block's and gr_hier_block2's
 * \ingroup container_blk
 * \ingroup base_blk
 *
 */
class GR_CORE_API gr_hier_block2 : public gr_basic_block
{
private:
  friend class gr_hier_block2_detail;
  friend GR_CORE_API gr_hier_block2_sptr gr_make_hier_block2(const std::string &name,
						 gr_io_signature_sptr input_signature,
						 gr_io_signature_sptr output_signature);
  
  /*!
   * \brief Private implementation details of gr_hier_block2
   */
  gr_hier_block2_detail *d_detail;
    
protected: 
  gr_hier_block2 (void){} //allows pure virtual interface sub-classes
  gr_hier_block2(const std::string &name,
		 gr_io_signature_sptr input_signature,
		 gr_io_signature_sptr output_signature);
  
public:
  virtual ~gr_hier_block2();
  
  /*!
   * \brief typedef for object returned from self().
   *
   * This type is only guaranteed to be passable to connect and disconnect.
   * No other assumptions should be made about it.
   */
  typedef gr_basic_block_sptr	opaque_self;

  /*!
   * \brief Return an object, representing the current block, which can be passed to connect.
   *
   * The returned object may only be used as an argument to connect or disconnect.
   * Any other use of self() results in unspecified (erroneous) behavior.
   */
  opaque_self self();

  /*!
   * \brief Add a stand-alone (possibly hierarchical) block to internal graph
   *
   * This adds a gr-block or hierarchical block to the internal graph
   * without wiring it to anything else.
   */
  void connect(gr_basic_block_sptr block);

  /*!
   * \brief Add gr-blocks or hierarchical blocks to internal graph and wire together
   *
   * This adds (if not done earlier by another connect) a pair of gr-blocks or 
   * hierarchical blocks to the internal flowgraph, and wires the specified output 
   * port to the specified input port.
   */
  void connect(gr_basic_block_sptr src, int src_port, 
	       gr_basic_block_sptr dst, int dst_port);

  /*!
   * \brief Remove a gr-block or hierarchical block from the internal flowgraph.
   *
   * This removes a gr-block or hierarchical block from the internal flowgraph,
   * disconnecting it from other blocks as needed.
   *
   */
  void disconnect(gr_basic_block_sptr block);

  /*!
   * \brief Disconnect a pair of gr-blocks or hierarchical blocks in internal
   *        flowgraph.
   *
   * This disconnects the specified input port from the specified output port
   * of a pair of gr-blocks or hierarchical blocks.
   */
  void disconnect(gr_basic_block_sptr src, int src_port,
		  gr_basic_block_sptr dst, int dst_port);

  /*!
   * \brief Disconnect all connections in the internal flowgraph.
   *
   * This call removes all output port to input port connections in the internal
   * flowgraph.
   */
  void disconnect_all();

  /*!
   * Lock a flowgraph in preparation for reconfiguration.  When an equal
   * number of calls to lock() and unlock() have occurred, the flowgraph
   * will be reconfigured.
   *
   * N.B. lock() and unlock() may not be called from a flowgraph thread
   * (E.g., gr_block::work method) or deadlock will occur when
   * reconfiguration happens.
   */
  virtual void lock();

  /*!
   * Unlock a flowgraph in preparation for reconfiguration.  When an equal
   * number of calls to lock() and unlock() have occurred, the flowgraph
   * will be reconfigured.
   *
   * N.B. lock() and unlock() may not be called from a flowgraph thread
   * (E.g., gr_block::work method) or deadlock will occur when
   * reconfiguration happens.
   */
  virtual void unlock();

  // This is a public method for ease of code organization, but should be
  // ignored by the user.
  gr_flat_flowgraph_sptr flatten() const;

  gr_hier_block2_sptr to_hier_block2(); // Needed for Python/Guile type coercion
};

inline gr_hier_block2_sptr cast_to_hier_block2_sptr(gr_basic_block_sptr block) {
  return boost::dynamic_pointer_cast<gr_hier_block2, gr_basic_block>(block);
}

#endif /* INCLUDED_GR_HIER_BLOCK2_H */
