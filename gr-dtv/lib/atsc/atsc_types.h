/* -*- c++ -*- */
/*
 * Copyright 2001,2006,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef DTV_INCLUDED_ATSC_TYPES_H
#define DTV_INCLUDED_ATSC_TYPES_H

#include <gnuradio/dtv/atsc_consts.h>
#include <cassert>
#include <cstring>

#include <gnuradio/dtv/atsc_plinfo.h>

namespace gr {
namespace dtv {

class atsc_mpeg_packet
{
public:
    static constexpr int NPAD = 68;
    unsigned char data[ATSC_MPEG_DATA_LENGTH + 1]; // first byte is sync
    unsigned char _pad_[NPAD];                     // pad to power of 2 (256)

    // overload equality operator
    bool operator==(const atsc_mpeg_packet& other) const
    {
        return std::memcmp(data, other.data, sizeof(data)) == 0;
    };

    bool operator!=(const atsc_mpeg_packet& other) const
    {
        return !(std::memcmp(data, other.data, sizeof(data)) == 0);
    };
};

class atsc_mpeg_packet_no_sync
{
public:
    static constexpr int NPAD = 65;
    plinfo pli;
    unsigned char data[ATSC_MPEG_DATA_LENGTH];
    unsigned char _pad_[NPAD]; // pad to power of 2 (256)

    // overload equality operator
    bool operator==(const atsc_mpeg_packet_no_sync& other) const
    {
        return std::memcmp(data, other.data, sizeof(data)) == 0;
    }

    bool operator!=(const atsc_mpeg_packet_no_sync& other) const
    {
        return !(std::memcmp(data, other.data, sizeof(data)) == 0);
    }
};

class atsc_mpeg_packet_rs_encoded
{
public:
    static constexpr int NPAD = 45;
    plinfo pli;
    unsigned char data[ATSC_MPEG_RS_ENCODED_LENGTH];
    unsigned char _pad_[NPAD]; // pad to power of 2 (256)

    // overload equality operator
    bool operator==(const atsc_mpeg_packet_rs_encoded& other) const
    {
        return std::memcmp(data, other.data, sizeof(data)) == 0;
    }

    bool operator!=(const atsc_mpeg_packet_rs_encoded& other) const
    {
        return !(std::memcmp(data, other.data, sizeof(data)) == 0);
    }
};


//! contains 832 3 bit symbols.  The low 3 bits in the byte hold the symbol.

class atsc_data_segment
{
public:
    static constexpr int NPAD = 188;
    plinfo pli;
    unsigned char data[ATSC_DATA_SEGMENT_LENGTH];
    unsigned char _pad_[NPAD]; // pad to power of 2 (1024)

    // overload equality operator
    bool operator==(const atsc_data_segment& other) const
    {
        return std::memcmp(data, other.data, sizeof(data)) == 0;
    }

    bool operator!=(const atsc_data_segment& other) const
    {
        return !(std::memcmp(data, other.data, sizeof(data)) == 0);
    }
};

/*!
 * Contains 832 bipolar floating point symbols.
 * Nominal values are +/- {1, 3, 5, 7}.
 * This data type represents the input to the viterbi decoder.
 */

class atsc_soft_data_segment
{
public:
    static constexpr int NPAD = 764;
    plinfo pli;
    float data[ATSC_DATA_SEGMENT_LENGTH];
    unsigned char _pad_[NPAD]; // pad to power of 2 (4096)
};

} /* namespace dtv */
} /* namespace gr */

#endif /* _ATSC_TYPES_H_ */
