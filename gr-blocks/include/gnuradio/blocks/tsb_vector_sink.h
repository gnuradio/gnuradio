/* -*- c++ -*- */
/*
 * Copyright 2014,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef TSB_VECTOR_SINK_H
#define TSB_VECTOR_SINK_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/tagged_stream_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief A vector sink for tagged streams.
 *
 * Unlike a gr::blocks::vector_sink_f, this only works with tagged streams.
 *
 * \ingroup blocks
 */
template <class T>
class BLOCKS_API tsb_vector_sink : virtual public gr::tagged_stream_block
{
public:
    using sptr = std::shared_ptr<tsb_vector_sink<T>>;

    virtual void reset() = 0;
    virtual std::vector<std::vector<T>> data() const = 0;
    /*! Doesn't include the TSB tags.
     */
    virtual std::vector<tag_t> tags() const = 0;

    /*!
     * \param vlen Vector length
     * \param tsb_key Tagged Stream Key
     */
    static sptr make(unsigned int vlen = 1, const std::string& tsb_key = "ts_last");
};
using tsb_vector_sink_b = tsb_vector_sink<std::uint8_t>;
using tsb_vector_sink_i = tsb_vector_sink<std::uint32_t>;
using tsb_vector_sink_s = tsb_vector_sink<std::uint16_t>;
using tsb_vector_sink_f = tsb_vector_sink<float>;
using tsb_vector_sink_c = tsb_vector_sink<gr_complex>;

} // namespace blocks
} // namespace gr

#endif /* TSB_VECTOR_SINK_H */
