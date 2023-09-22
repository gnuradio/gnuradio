/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_FILE_META_SINK_H
#define INCLUDED_BLOCKS_FILE_META_SINK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

constexpr char METADATA_VERSION = 0;
constexpr size_t METADATA_HEADER_SIZE = 149;

enum gr_file_types {
    GR_FILE_BYTE = 0,
    GR_FILE_CHAR = 0,
    GR_FILE_SHORT = 1,
    GR_FILE_INT,
    GR_FILE_LONG,
    GR_FILE_LONG_LONG,
    GR_FILE_FLOAT,
    GR_FILE_DOUBLE,
};

/*!
 * \brief Write stream to file with meta-data headers.
 * \ingroup file_operators_blk
 *
 * \details
 * These files represent data as binary information in between
 * meta-data headers. The headers contain information about the
 * type of data and properties of the data in the next segment of
 * samples. The information includes:
 *
 *   \li rx_rate (double): sample rate of data.
 *   \li rx_time (uint64_t, double): time stamp of first sample in segment.
 *   \li size (uint32_t): item size in bytes.
 *   \li type (::gr_file_types as int32_t): data type.
 *   \li cplx (bool): Is data complex?
 *   \li strt (uint64_t): Starting byte of data in this segment.
 *   \li bytes (uint64_t): Size in bytes of data in this segment.
 *
 * Tags can be sent to the file to update the information, which
 * will create a new header. Headers are found by searching from
 * the first header (at position 0 in the file) and reading where
 * the data segment starts plus the data segment size. Following
 * will either be a new header or EOF.
 */
class BLOCKS_API file_meta_sink : virtual public sync_block
{
public:
    // gr::blocks::file_meta_sink::sptr
    typedef std::shared_ptr<file_meta_sink> sptr;

    /*!
     * \brief Create a meta-data file sink.
     *
     * \param itemsize (size_t): Size of data type.
     * \param filename (string): Name of file to write data to.
     * \param samp_rate (double): Sample rate of data. If sample rate will be
     *    set by a tag, such as rx_tag from a UHD source, this is
     *    basically ignored.
     * \param relative_rate (double): Rate chance from source of sample
     *    rate tag to sink.
     * \param type (gr_file_types): Data type (int, float, etc.)
     * \param complex (bool): If data stream is complex
     * \param max_segment_size (size_t): Length of a single segment
     *    before the header is repeated (in items).
     * \param extra_dict: a PMT dictionary of extra
     *    information.
     * \param detached_header (bool): Set to true to store the header
     *    info in a separate file (named filename.hdr)
     */
    static sptr make(size_t itemsize,
                     const std::string& filename,
                     double samp_rate = 1,
                     double relative_rate = 1,
                     gr_file_types type = GR_FILE_FLOAT,
                     bool complex = true,
                     size_t max_segment_size = 1000000,
                     pmt::pmt_t extra_dict = pmt::make_dict(),
                     bool detached_header = false);

    virtual bool open(const std::string& filename) = 0;
    virtual void close() = 0;
    virtual void do_update() = 0;

    virtual void set_unbuffered(bool unbuffered) = 0;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_FILE_META_SINK_H */
