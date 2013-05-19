/* -*- c++ -*- */
/*
 * Copyright 2009,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_HISTO_SINK_F_H
#define INCLUDED_GR_HISTO_SINK_F_H

#include <gnuradio/wxgui/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/msg_queue.h>

namespace gr {
  namespace wxgui {

    /*!
     * \brief Histogram module.
     * \ingroup sink_blk
     */
    class WXGUI_API histo_sink_f : virtual public sync_block
    {
    public:
      // gr::blocks::histo_sink_f::sptr
      typedef boost::shared_ptr<histo_sink_f> sptr;

      static sptr make(msg_queue::sptr msgq);

      virtual unsigned int get_frame_size(void) = 0;
      virtual unsigned int get_num_bins(void) = 0;

      virtual void set_frame_size(unsigned int frame_size) = 0;
      virtual void set_num_bins(unsigned int num_bins) = 0;
    };

  } /* namespace wxgui */
} /* namespace gr */

#endif /* INCLUDED_GR_HISTO_SINK_F_H */
