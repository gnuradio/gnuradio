/* -*- c++ -*- */
/*
 * Copyright 2026 Jimmy Fitzpatrick
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_BLOCKS_SIGMF_SINK_H
#define INCLUDED_BLOCKS_SIGMF_SINK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Create a SigMF recording.
 * \ingroup file_operators_blk
 *
 * This block writes the input stream to a SigMF recording, comprising a Metadata file and
 * the Dataset file it describes.
 */
class BLOCKS_API sigmf_sink : virtual public sync_block
{
public:
    /* gr::blocks::sigmf_sink::sptr */
    typedef std::shared_ptr<sigmf_sink> sptr;

    /*!
     * \brief Create a SigMF sink block.
     *
     * \param filename The base file name (no extension) of the SigMF Dataset and Metadata
     * files. If an extension is provided, it is ignored. \param datatype The data type
     * of each sample in the input stream as a SigMF Dataset format.
     */
    static sptr make(const std::string& filename, const std::string& datatype);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_SIGMF_SINK_H */