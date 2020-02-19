/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_VITERBI_DECODER_H
#define INCLUDED_DTV_ATSC_VITERBI_DECODER_H

#include <gnuradio/dtv/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace dtv {

/*!
 * \brief ATSC Viterbi Decoder
 *
 * \ingroup dtv_atsc
 */
class DTV_API atsc_viterbi_decoder : virtual public gr::sync_block
{
public:
    // gr::dtv::atsc_viterbi_decoder::sptr
    typedef boost::shared_ptr<atsc_viterbi_decoder> sptr;

    /*!
     * \brief Make a new instance of gr::dtv::atsc_viterbi_decoder.
     */
    static sptr make();

    /*!
     * For each decoder, returns the current best state of the
     * decoding metrics.
     */
    virtual std::vector<float> decoder_metrics() const = 0;
};

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_VITERBI_DECODER_H */
