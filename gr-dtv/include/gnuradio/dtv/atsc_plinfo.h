/* -*- c++ -*- */
/*
 * Copyright 2001,2006,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef DTV_INCLUDED_ATSC_PLINFO_H
#define DTV_INCLUDED_ATSC_PLINFO_H

#include <gnuradio/dtv/atsc_consts.h>
#include <cassert>
#include <cstdint>
#include <cstring>

#include <gnuradio/dtv/api.h>

namespace gr {
namespace dtv {

/*!
 * \brief pipeline info that flows with data
 *
 * Not all modules need all the info
 */
class DTV_API plinfo
{
public:
    plinfo();
    plinfo(uint16_t flags, int16_t segno);

    /**
     * @brief Resets the flags and segno fields
     *
     */
    void reset();

    // accessors
    bool field_sync1_p() const;
    bool field_sync2_p() const;
    bool field_sync_p() const;

    bool regular_seg_p() const;

    bool in_field1_p() const;
    bool in_field2_p() const;

    bool first_regular_seg_p() const;

    bool transport_error_p() const;

    unsigned int segno() const;
    unsigned int flags() const;

    // setters

    void set_field_sync1();

    void set_field_sync2();

    void set_regular_seg(bool field2, int segno);

    void set_transport_error(bool error);

    /*!
     * Set \p OUT such that it reflects a \p NSEGS_OF_DELAY
     * pipeline delay from \p IN.
     */
    static void delay(plinfo& out, const plinfo& in, int nsegs_of_delay);

    // these three are mutually exclusive
    //     This is a regular data segment.
    static constexpr int fl_regular_seg = 0x0001;
    //	 This is a field sync segment, for 1st half of a field.
    static constexpr int fl_field_sync1 = 0x0002;
    //	 This is a field sync segment, for 2nd half of a field.
    static constexpr int fl_field_sync2 = 0x0004;

    // This bit is on ONLY when fl_regular_seg is set AND when this is
    // the first regular data segment AFTER a field sync segment.  This
    // segment causes various processing modules to reset.
    static constexpr int fl_first_regular_seg = 0x0008;

    // which field are we in?
    static constexpr int fl_field2 = 0x0010; // else field 1

    // This bit is set when Reed-Solomon decoding detects an error that it
    // can't correct.  Note that other error detection (e.g. Viterbi) do not
    // set it, since Reed-Solomon will correct many of those.  This bit is
    // then copied into the final Transport Stream packet so that MPEG
    // software can see that the 188-byte data segment has been corrupted.
    static constexpr int fl_transport_error = 0x0020;

private:
    uint16_t d_flags = 0; // bitmask
    int16_t d_segno = 0;  // segment number [-1,311] -1 is the field sync segment
};

} /* namespace dtv */
} /* namespace gr */

#endif /* _ATSC_PLINFO_H_ */
