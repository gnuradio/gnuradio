/* -*- c++ -*- */
/*
 * Copyright 2002,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_TRELLIS_INTERLEAVER_H
#define INCLUDED_TRELLIS_INTERLEAVER_H

#include <gnuradio/trellis/api.h>
#include <string>
#include <vector>

namespace gr {
namespace trellis {

/*!
 * \brief  INTERLEAVER class
 * \ingroup trellis_coding_blk
 */
class TRELLIS_API interleaver
{
private:
    unsigned int d_K;
    std::vector<int> d_INTER;
    std::vector<int> d_DEINTER;

public:
    interleaver();
    interleaver(const interleaver& INTERLEAVER);
    interleaver(unsigned int K, const std::vector<int>& INTER);
    interleaver(const char* name);
    interleaver(unsigned int K, int seed);
    unsigned int K() const { return d_K; }
    const std::vector<int>& INTER() const { return d_INTER; }
    const std::vector<int>& DEINTER() const { return d_DEINTER; }
    void write_interleaver_txt(std::string filename);
};

} /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_TRELLIS_INTERLEAVER_H */
