/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CTRLPORT_PROBE2_C_H
#define INCLUDED_CTRLPORT_PROBE2_C_H

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
class BLOCKS_API ctrlport_probe2_c : virtual public sync_block
{
public:
    // gr::blocks::ctrlport_probe2_c::sptr
    typedef std::shared_ptr<ctrlport_probe2_c> sptr;

    /*!
     * \brief Make a ControlPort probe block.
     * \param id A string ID to name the probe over ControlPort.
     * \param desc A string describing the probe.
     * \param len Number of samples to transmit.
     * \param disp_mask Mask to set default display params.
     */
    static sptr
    make(const std::string& id, const std::string& desc, int len, unsigned int disp_mask);

    virtual std::vector<gr_complex> get() = 0;

    virtual void set_length(int len) = 0;
    virtual int length() const = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_CTRLPORT_PROBE2_C_H */
