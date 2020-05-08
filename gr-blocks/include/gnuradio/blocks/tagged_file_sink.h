/* -*- c++ -*- */
/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_TAGGED_FILE_SINK_H
#define INCLUDED_GR_TAGGED_FILE_SINK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief A file sink that uses tags to save files.
 * \ingroup file_operators_blk
 * \ingroup stream_tag_tools_blk
 *
 * \details
 * The sink uses a tag with the key 'burst' to trigger the saving
 * of the burst data to a new file. If the value of this tag is
 * True, it will open a new file and start writing all incoming
 * data to it. If the tag is False, it will close the file (if
 * already opened).
 * The file names differ between legacy and non-legacy mode.
 * In leagcy mode, names are based on the time when the burst tag
 * was seen. If there is an 'rx_time' tag (standard with
 * UHD sources), that is used as the time. If no 'rx_time' tag is
 * found, the new time is calculated based off the sample rate of
 * the block.
 * In non-legacy mode, filename prefix need to be defined, the rest
 * of the file name will be created basing on current date and time
 * with microsecond precision: prefix_%Y-%m-%d_%H_%M_%S.%f.dat
 */
class BLOCKS_API tagged_file_sink : virtual public sync_block
{
public:
    // gr::blocks::tagged_file_sink::sptr
    typedef std::shared_ptr<tagged_file_sink> sptr;

    /*!
     * \brief Build a tagged_file_sink block.
     *
     * \param itemsize The item size of the input data stream.
     * \param samp_rate The sample rate used to determine the time
     *                  difference between bursts
     * \param directory Directory where file will be be saved,
     *                  can be empty.
     * \param legacy_filename If true, use legacy filename generator.
     * \param filename_prefix Prefix of the saved file in non-legacy mode.
     *                        prefix_%Y-%m-%d_%H_%M_%S.%f.dat,
     */
    static sptr make(size_t itemsize,
                     double samp_rate,
                     const std::string& directory = "",
                     const std::string& filename_prefix = "",
                     bool legacy_filename = true);
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_TAGGED_FILE_SINK_H */
