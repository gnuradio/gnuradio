/* -*- c++ -*- */
/*
 * Copyright 2002,2012 Free Software Foundation, Inc.
 * Copyright 2025 Marcus Müller <mmueller@gnuradio.org>
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
    std::vector<int> d_interleaver_indices;
    std::vector<int> d_deinterleaver_indices;

public:
    //! \brief construct empty interleaver
    interleaver();
    //! \brief copy constructor
    interleaver(const interleaver& interleaver);
    /*!
     * \brief construct interleaver with redundant vector length parameter
     * \param k            vector length
     * \param interleaver  index vector
     */
    [[deprecated("Will be removed in 3.11")]] interleaver(
        unsigned int k, const std::vector<int>& interleaver);
    /*!
     * \brief construct interleaver from index vector
     * \param interleaver  index vector
     */
    interleaver(const std::vector<int>& interleaver);
    /*!
     * \brief construct interleaver from file
     *
     * \details
     * File format is ASCII,
     * <code>
     * {number of interleaver taps}
     * {empty line}
     * {K space-separated list: permutation of 0 1 2 … K-1}
     * {optional comments}
     * </code>
     *
     * \param name     file containing interleaver specification
     */
    interleaver(const char* name);
    /*!
     * \brief construct random interleaver
     * \param k      length
     * \param seed   seed for PRNG
     */
    interleaver(unsigned int k, int seed);

    //! \brief return length of interleaver
    unsigned int k() const { return d_interleaver_indices.size(); }
    //! \brief return interleaver index vector reference
    const std::vector<int>& interleaver_indices() const { return d_interleaver_indices; }
    //! \brief return deinterleaver index vector reference
    const std::vector<int>& deinterleaver_indices() const
    {
        return d_deinterleaver_indices;
    }

    // TODO in GR 3.11, remove K, INTER and DEINTER
    //! \brief return length of interleaver
    [[deprecated("Will be removed in 3.11")]] unsigned int K() const
    {
        return d_interleaver_indices.size();
    }
    //! \brief return interleaver index vector reference
    [[deprecated("Will be removed in 3.11")]] const std::vector<int>& INTER() const
    {
        return d_interleaver_indices;
    }
    //! \brief return deinterleaver index vector reference
    [[deprecated("Will be removed in 3.11")]] const std::vector<int>& DEINTER() const
    {
        return d_deinterleaver_indices;
    }


    /*!
     * \brief write interleaver index vector to text file
     * \see gr::trellis::interleaver::interleaver(const char* name)
     */
    void write_interleaver_txt(std::string filename);
};

} /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_TRELLIS_INTERLEAVER_H */
