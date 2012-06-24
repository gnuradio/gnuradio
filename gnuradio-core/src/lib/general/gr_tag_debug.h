/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_TAG_DEBUG_H
#define INCLUDED_GR_TAG_DEBUG_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gruel/thread.h>
#include <stddef.h>

class gr_tag_debug;
typedef boost::shared_ptr<gr_tag_debug> gr_tag_debug_sptr;

GR_CORE_API gr_tag_debug_sptr
gr_make_tag_debug(size_t sizeof_stream_item, const std::string &name);

/*!
 * \brief Bit bucket that prints out any tag received.
 * \ingroup sink_blk
 *
 * This block collects all tags sent to it on all input ports and
 * displays them to stdout in a formatted way. The \p name parameter
 * is used to identify which debug sink generated the tag, so when
 * connecting a block to this debug sink, an appropriate name is
 * something that identifies the input block.
 *
 * This block otherwise acts as a NULL sink in that items from the
 * input stream are ignored. It is designed to be able to attach to
 * any block and watch all tags streaming out of that block for
 * debugging purposes.
 *
 * The tags from the last call to this work function are stored and
 * can be retrieved using the function 'current_tags'.
 */
class GR_CORE_API gr_tag_debug : public gr_sync_block
{
 private:
  friend GR_CORE_API gr_tag_debug_sptr
    gr_make_tag_debug(size_t sizeof_stream_item, const std::string &name);
  gr_tag_debug(size_t sizeof_stream_item, const std::string &name);

  std::string d_name;
  std::vector<gr_tag_t> d_tags;
  std::vector<gr_tag_t>::iterator d_tags_itr;
  bool d_display;
  gruel::mutex d_mutex;

 public:
  /*!
   * \brief Returns a vector of gr_tag_t items as of the last call to
   * work.
   */
  std::vector<gr_tag_t> current_tags();

  /*!
   * \brief Set the display of tags to stdout on/off.
   */
  void set_display(bool d);

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GR_TAG_DEBUG_H */
