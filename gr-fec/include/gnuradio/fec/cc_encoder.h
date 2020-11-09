/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FEC_CC_ENCODER_H
#define INCLUDED_FEC_CC_ENCODER_H

#include <gnuradio/fec/api.h>
#include <gnuradio/fec/cc_common.h>
#include <gnuradio/fec/encoder.h>
#include <map>
#include <string>

namespace gr {
namespace fec {
namespace code {

/*!
 * \brief Convolutional Code Encoding class.
 * \ingroup error_coding_blk
 *
 * \details
 * This class performs convolutional encoding for unpacked bits
 * for frames of a constant length. This class is general in its
 * application of the convolutional encoding and allows us to
 * specify the constraint length, the coding rate, and the
 * polynomials used in the coding process.
 *
 * The parameter \p k sets the constraint length directly. We
 * set the coding rate by setting \p rate to R given a desired
 * rate of 1/R. That is, for a rate 1/2 coder, we would set \p
 * rate to 2. And the polynomial is specified as a vector of
 * integers, where each integer represents the coding polynomial
 * for a different arm of the code. The number of polynomials
 * given must be the same as the value \p rate.
 *
 * The encoding object holds a shift register that takes in each
 * bit from the input stream and then ANDs the shift register
 * with each polynomial, and places the parity of the result
 * into the output stream. The output stream is therefore also
 * unpacked bits.
 *
 * The encoder is set up with a number of bits per frame in the
 * constructor. When not being used in a tagged stream mode,
 * this encoder will only process frames of the length provided
 * here. If used in a tagged stream block, this setting becomes
 * the maximum allowable frame size that the block may process.
 *
 * The \p mode is a cc_mode_t that specifies how the convolutional
 * encoder will behave and under what conditions.
 *
 * \li 'CC_STREAMING': mode expects an uninterrupted flow of
 * samples into the encoder, and the output stream is
 * continually encoded.
 *
 * \li 'CC_TERMINATED': is a mode designed for packet-based systems. This mode
 * flushes the encoder with K-1 bits which adds rate*(K-1) bits to the output.
 * This improves the protection of the last bits of a block and helps the
 * decoder.
 *
 * \li 'CC_TAILBITING': is another packet-based method. Instead of adding bits
 * onto the end of a packet (as with 'CC_TERMINATED'), this mode will
 * pre-initialize the state of the encoder with a packet’s last (k-1) bits.
 *
 * \li 'CC_TRUNCATED': a truncated code always resets the registers
 * to the \p start_state between frames.
 *
 * A common convolutional encoder uses K=7, Rate=1/2, and the polynomials
 * \li  1 + x^2 + x^3 + x^5 + x^6
 * \li  1 + x   + x^2 + x^3 + x^6
 * This is the Voyager code from NASA.
 *
 * Another encoder class is provided with gr-fec called the
 * gr::fec::code::ccsds_encoder, which implements the above code
 * that is more highly optimized for just those specific
 * settings.
 */
class FEC_API cc_encoder : virtual public generic_encoder
{
public:
    /*!
     * Build a convolutional code encoding FEC API object.
     *
     * \param frame_size Number of bits per frame; must be > 1. If using in the
     *        tagged stream style, this is the maximum allowable number of bits
     *        per frame.
     * \param k Constraint length (K) of the encoder; must be in the range [2, 31].
     *        K = 1 implies a code without memory which does not make sense;
     *        upper limit is due the way the polynomials of the code are passed
     *        in \p polys.
     * \param rate Inverse of the coder's rate; must be > 1.
     *             (rate=2 means 2 output bits per 1 input).
     * \param polys Vector of polynomials as integers. The least significant bit
     *              (LSB) denotes the coefficient of exponent zero of the coding
     *              polynomial. The position of the most significant set bit
     *              (zero based counting) is \p K-1. Note: this representation
     *              is reversed compared to the common representation as found
     *              in most books and references. The common representation puts
     *              the coefficient of the highest exponent into the LSB and the
     *              coefficient of exponent zero is the highest set bit.
     *              Example: The common binary representation of the Voyager
     *              code polynomials (see above) is 1011011 and 1111001; the
     *              octal representation is 133 and 171. For this block, the
     *              binary representation must be reversed: 1101101 and 1001111;
     *              octal this is 155 and 117; decimal this is 109 and 79. Some
     *              standards (e.g. CCSDS 131.0-B-3) require the inversion of
     *              some outputs. This is supported by providing the negative
     *              value of the polynomial, e.g. -109.
     * \param start_state Initialization state of the shift register; must be in
     *                    range [0, 2^(K-1)-1] where K is the constraint length.
     *                    The bits in \p start_state are also used to flush the
     *                    encoder in mode 'CC_TERMINATED'.
     *                    Note: Most books and references use a shift register
     *                    shifting from left to right. This implementation,
     *                    however, shifts from right to left. This means that
     *                    the start state must be reversed. (The different shift
     *                    direction is also the reason why the polynomials must
     *                    be reversed as described above.)
     * \param mode cc_mode_t mode of the encoding.
     * \param padded true if the encoded frame should be padded
     *               to the nearest byte.
     */
    static generic_encoder::sptr make(int frame_size,
                                      int k,
                                      int rate,
                                      std::vector<int> polys,
                                      int start_state = 0,
                                      cc_mode_t mode = CC_STREAMING,
                                      bool padded = false);

    /*!
     * Sets the uncoded frame size to \p frame_size. If \p
     * frame_size is greater than the value given to the
     * constructor, the frame size will be capped by that initial
     * value and this function will return false. Otherwise, it
     * returns true.
     */
    bool set_frame_size(unsigned int frame_size) override = 0;

    /*!
     * Returns the coding rate of this encoder.
     */
    double rate() override = 0;
};

} /* namespace code */
} /* namespace fec */
} /* namespace gr */

#endif /* INCLUDED_FEC_CC_ENCODER_H */
