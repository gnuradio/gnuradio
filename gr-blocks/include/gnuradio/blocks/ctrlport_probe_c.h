/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_CTRLPORT_PROBE_C_H
#define INCLUDED_CTRLPORT_PROBE_C_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/rpcregisterhelpers.h>
#include <boost/thread/shared_mutex.hpp>

namespace gr {
  namespace blocks {

    /*!
     * \brief A ControlPort probe to export vectors of signals.
     * \ingroup measurement_tools_blk
     * \ingroup controlport_blk
     *
     * \details
     * This block acts as a sink in the flowgraph but also exports
     * vectors of complex samples over ControlPort. This block simply
     * sends the current vector held in the work function when the
     * queried by a ControlPort client.
     */
    class BLOCKS_API ctrlport_probe_c : virtual public sync_block
    {
    public:
      // gr::blocks::ctrlport_probe_c::sptr
      typedef boost::shared_ptr<ctrlport_probe_c> sptr;

      /*!
       * \brief Make a ControlPort probe block.
       * \param id A string ID to name the probe over ControlPort.
       * \param desc A string describing the probe.
       */
      static sptr make(const std::string &id, const std::string &desc);

      virtual std::vector<gr_complex> get() = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_CTRLPORT_GR_CTRLPORT_PROBE_C_H */

