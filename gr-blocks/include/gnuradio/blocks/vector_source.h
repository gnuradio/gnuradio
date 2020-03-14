/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2012-2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef VECTOR_SOURCE_H
#define VECTOR_SOURCE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief Source that streams T items based on the input \p data vector.
 * \ingroup misc_blk
 *
 * \details
 * This block produces a stream of samples based on an input
 * vector. In C++, this is a std::vector<T>, and in Python,
 * this is either a list or tuple. The data can repeat infinitely
 * until the flowgraph is terminated by some other event or, the
 * default, run the data once and stop.
 *
 * The vector source can also produce stream tags with the
 * data. Pass in a vector of gr::tag_t objects and they will be
 * emitted based on the specified offset of the tag.
 *
 * GNU Radio provides a utility Python module in gr.tag_utils to
 * convert between tags and Python objects:
 * gr.tag_utils.python_to_tag.
 *
 * We can create tags as Python lists (or tuples) using the list
 * structure [int offset, pmt key, pmt value, pmt srcid]. It is
 * important to define the list/tuple with the values in the
 * correct order and with the correct data type. A python
 * dictionary can also be used using the keys: "offset", "key",
 * "value", and "srcid" with the same data types as for the lists.
 *
 * When given a list of tags, the vector source will emit the tags
 * repeatedly by updating the offset relative to the vector stream
 * length. That is, if the vector has 500 items and a tag has an
 * offset of 0, that tag will be placed on item 0, 500, 1000,
 * 1500, etc.
 */
template <class T>
class BLOCKS_API vector_source : virtual public sync_block
{
public:
    // gr::blocks::vector_source::sptr
    typedef std::shared_ptr<vector_source<T>> sptr;

    static sptr make(const std::vector<T>& data,
                     bool repeat = false,
                     unsigned int vlen = 1,
                     const std::vector<tag_t>& tags = std::vector<tag_t>());

    virtual void rewind() = 0;
    virtual void set_data(const std::vector<T>& data,
                          const std::vector<tag_t>& tags = std::vector<tag_t>()) = 0;
    virtual void set_repeat(bool repeat) = 0;
};

typedef vector_source<std::uint8_t> vector_source_b;
typedef vector_source<std::int16_t> vector_source_s;
typedef vector_source<std::int32_t> vector_source_i;
typedef vector_source<float> vector_source_f;
typedef vector_source<gr_complex> vector_source_c;
} /* namespace blocks */
} /* namespace gr */

#endif /* VECTOR_SOURCE_H */
