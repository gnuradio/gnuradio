/* -*- c++ -*- */
/*
 * Copyright 2004,2005,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_SIMPLE_FRAMER_SYNC_H
#define INCLUDED_GR_SIMPLE_FRAMER_SYNC_H

namespace gr {
namespace digital {

/*!
 * Here are a couple of maximum length sequences (m-sequences)
 * that were generated by the the "mseq" matlab/octave code
 * downloaded from: <a
 * href="http://www.mathworks.com/matlabcentral/fileexchange/990">http://www.mathworks.com/matlabcentral/fileexchange/990</a>
 *
 * <pre>
 * 31-bit m-sequence:
 *	0110100100001010111011000111110
 *	0x690AEC76 (padded on right with a zero)
 *
 * 63-bit m-sequence:
 *	101011001101110110100100111000101111001010001100001000001111110
 *      0xACDDA4E2F28C20FC (padded on right with a zero)
 * </pre>
 */
static constexpr unsigned long long GRSF_SYNC = 0xacdda4e2f28c20fcULL;

static constexpr int GRSF_BITS_PER_BYTE = 8;
static constexpr int GRSF_SYNC_OVERHEAD = sizeof(GRSF_SYNC);
static constexpr int GRSF_PAYLOAD_OVERHEAD = 1; // 1 byte seqno
static constexpr int GRSF_TAIL_PAD = 1;         // one byte trailing padding
static constexpr int GRSF_OVERHEAD =
    GRSF_SYNC_OVERHEAD + GRSF_PAYLOAD_OVERHEAD + GRSF_TAIL_PAD;

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_GR_SIMPLE_FRAMER_SYNC_H */