/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_MODULATE_VECTOR_H
#define INCLUDED_DIGITAL_MODULATE_VECTOR_H

#include <gnuradio/digital/api.h>
#include <gnuradio/types.h>

namespace gr {
namespace digital {

/*!
 * \brief Modulate a vector of data and apply a shaping filter.
 *
 * \p modulator: Pointer to a byte-to-complex modulator block.
 * \p data: Vector of bytes to modulate into symbols.
 * \p taps: Post-modulation symbol shaping filter taps.
 *
 * \details
 * This function modulates the input vector and applies a
 * symbol shaping filter. It is intended for use with the
 * corr_est_cc block as the symbol stream to correlate
 * against.
 *
 * Any differential encoding or other data coding must be
 * performed on the input vector before this modulation
 * operation.
 *
 * Be aware that the format of the incoming data must match
 * the format the modulator block is expecting. GNURadio
 * modulator blocks are inconsistent in their data type
 * expectations. For instance, cpmmod_bc expects unpacked,
 * signed bytes in (-1, 1), while gmsk_mod expects packed,
 * unsigned bytes in (0, 1). In other words, the output of
 * gmsk_mod given the input vector [0xaa, 0x00] is equivalent
 * to the output of cpmmod_bc given the input vector
 * [1,255,1,255,1,255,1,255,255,255,255,255,255,255,255,255]
 *
 * Please check the documentation or source of the modulator
 * before using this function.
 *
 */
DIGITAL_API std::vector<gr_complex> modulate_vector_bc(basic_block_sptr modulator,
                                                       std::vector<uint8_t> data,
                                                       std::vector<float> taps);

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_MODULATE_VECTOR_H */
