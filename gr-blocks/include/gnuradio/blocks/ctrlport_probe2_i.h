/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_CTRLPORT_PROBE2_I_H
#define INCLUDED_CTRLPORT_PROBE2_I_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief A ControlPort probe to export vectors of signals.
     * \ingroup measurement_tools_blk
     * \ingroup controlport_blk
     *
     * \details
     * This block acts as a sink in the flowgraph but also exports
     * vectors of complex samples over ControlPort. This block holds
     * the latest \p len number of complex samples so that every query
     * by a ControlPort client will get the same length vector.
     */
    class BLOCKS_API ctrlport_probe2_i : virtual public sync_block
    {
    public:
      // gr::blocks::ctrlport_probe2_i::sptr
      typedef boost::shared_ptr<ctrlport_probe2_i> sptr;

      /*!
       * \brief Make a ControlPort probe block.
       * \param id A string ID to name the probe over ControlPort.
       * \param desc A string describing the probe.
       * \param len Number of samples to transmit.
       * \param disp_mask Mask to set default display params.
       */
      static sptr make(const std::string &id, const std::string &desc,
                       int len, unsigned int disp_mask);

      virtual std::vector<int> get() = 0;

      virtual void set_length(int len) = 0;
      virtual int length() const = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_CTRLPORT_PROBE2_I_H */

