/* -*- c++ -*- */
/*
 * Copyright 2006,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


/*
 * a bit of template hackery...
 */
#ifndef INCLUDED_BLOCKS_LOG2_CONST_H
#define INCLUDED_BLOCKS_LOG2_CONST_H

#include <gnuradio/blocks/api.h>
#include <assert.h>

namespace gr {
namespace blocks {

template <unsigned int k>
static inline constexpr int log2_const()
{
    assert(0);
    return 0;
}

template <>
inline constexpr int log2_const<1>()
{
    return 0;
}
template <>
inline constexpr int log2_const<2>()
{
    return 1;
}
template <>
inline constexpr int log2_const<4>()
{
    return 2;
}
template <>
inline constexpr int log2_const<8>()
{
    return 3;
}
template <>
inline constexpr int log2_const<16>()
{
    return 4;
}
template <>
inline constexpr int log2_const<32>()
{
    return 5;
}
template <>
inline constexpr int log2_const<64>()
{
    return 6;
}
template <>
inline constexpr int log2_const<128>()
{
    return 7;
}
template <>
inline constexpr int log2_const<256>()
{
    return 8;
}
template <>
inline constexpr int log2_const<512>()
{
    return 9;
}
template <>
inline constexpr int log2_const<1024>()
{
    return 10;
}

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_BLOCKS_LOG2_CONST_H */
