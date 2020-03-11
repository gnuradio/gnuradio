/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ATSC_FAKE_SINGLE_VITERBI_H
#define INCLUDED_ATSC_FAKE_SINGLE_VITERBI_H

namespace gr {
namespace dtv {

class atsc_fake_single_viterbi
{
public:
    atsc_fake_single_viterbi();

    /*!
     * \p INPUT ideally takes on the values +/- 1,3,5,7
     * return is decoded dibit in the range [0, 3]
     */
    char decode(float input);

    void reset();

    //! internal delay of decoder
    int delay() { return 0; }

protected:
    int post_coder_state;
};

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_ATSC_FAKE_SINGLE_VITERBI_H */
