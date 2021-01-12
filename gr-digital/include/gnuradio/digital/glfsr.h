/* -*- c++ -*- */
/*
 * Copyright 2007,2012,2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_GLFSR_H
#define INCLUDED_DIGITAL_GLFSR_H

#include <gnuradio/digital/api.h>
#include <cstdint>

namespace gr {
namespace digital {

/*!
 * \brief Galois Linear Feedback Shift Register using specified polynomial mask
 * \ingroup waveform_generators_blk
 *
 * \details
 * Generates a maximal length pseudo-random sequence of length 2^degree-1
 * if given a primitive polynomial.
 */
class DIGITAL_API glfsr
{
private:
    uint64_t d_shift_register;
    uint64_t d_mask;

public:
    glfsr(uint64_t mask, uint64_t seed)
    {
        d_shift_register = seed;
        d_mask = mask;
    }
    ~glfsr();

    static uint64_t glfsr_mask(unsigned int degree);

    uint8_t next_bit();

    uint64_t mask() const { return d_mask; }
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_GLFSR_H */
