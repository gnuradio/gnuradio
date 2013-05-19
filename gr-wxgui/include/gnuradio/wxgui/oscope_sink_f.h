/* -*- c++ -*- */
/*
 * Copyright 2003-2005,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_OSCOPE_SINK_F_H
#define INCLUDED_GR_OSCOPE_SINK_F_H

#include <gnuradio/wxgui/api.h>
#include <gnuradio/wxgui/oscope_sink_x.h>
#include <gnuradio/msg_queue.h>

namespace gr {
  namespace wxgui {

    /*!
     * \brief Building block for python oscilloscope module.
     * \ingroup sink_blk
     *
     * Accepts multiple float streams.
     */
    class WXGUI_API oscope_sink_f
      : virtual public oscope_sink_x
    {
    public:
      // gr::blocks::oscope_sink_f::sptr
      typedef boost::shared_ptr<oscope_sink_f> sptr;

      static sptr make(double sampling_rate, msg_queue::sptr msgq);
    };

  } /* namespace wxgui */
} /* namespace gr */

#endif /* INCLUDED_GR_OSCOPE_SINK_F_H */

