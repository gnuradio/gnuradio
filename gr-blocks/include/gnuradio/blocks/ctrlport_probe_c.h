/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CTRLPORT_PROBE_C_H
#define INCLUDED_CTRLPORT_PROBE_C_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/rpcregisterhelpers.h>
#include <gnuradio/sync_block.h>
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
    typedef std::shared_ptr<ctrlport_probe_c> sptr;

    /*!
     * \brief Make a ControlPort probe block.
     * \param id A string ID to name the probe over ControlPort.
     * \param desc A string describing the probe.
     */
    static sptr make(const std::string& id, const std::string& desc);

    virtual std::vector<gr_complex> get() = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_CTRLPORT_GR_CTRLPORT_PROBE_C_H */
