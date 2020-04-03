/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_FILE_META_SOURCE_H
#define INCLUDED_BLOCKS_FILE_META_SOURCE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief Reads stream from file with meta-data headers. Headers
 * are parsed into tags.
 * \ingroup file_operators_blk
 *
 * \details
 * The information in the metadata headers includes:
 *
 *  \li rx_rate (double): sample rate of data.
 *  \li rx_time (uint64_t, double): time stamp of first sample in segment.
 *  \li size (uint32_t): item size in bytes.
 *  \li type (::gr_file_types as int32_t): data type.
 *  \li cplx (bool): Is data complex?
 *  \li strt (uint64_t): Starting byte of data in this segment.
 *  \li bytes (uint64_t): Size in bytes of data in this segment.
 *
 * Any item inside of the extra header dictionary is ready out and
 * made into a stream tag.
 */
class BLOCKS_API file_meta_source : virtual public sync_block
{
public:
    // gr::blocks::file_meta_source::sptr
    typedef std::shared_ptr<file_meta_source> sptr;

    /*!
     * \brief Create a meta-data file source.
     *
     * \param filename (string): Name of file to write data to.
     * \param repeat (bool): Repeats file when EOF is found.
     * \param detached_header (bool): Set to true if header
     *    info is stored in a separate file (usually named filename.hdr)
     * \param hdr_filename (string): Name of detached header file if used.
     *    Defaults to 'filename.hdr' if detached_header is true but this
     *    field is an empty string.
     */
    static sptr make(const std::string& filename,
                     bool repeat = false,
                     bool detached_header = false,
                     const std::string& hdr_filename = "");

    virtual bool open(const std::string& filename,
                      const std::string& hdr_filename = "") = 0;
    virtual void close() = 0;
    virtual void do_update() = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_FILE_META_SOURCE_H */
