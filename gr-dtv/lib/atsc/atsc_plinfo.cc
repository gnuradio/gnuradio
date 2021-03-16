/* -*- c++ -*- */
/*
 * Copyright 2001,2006,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/dtv/atsc_plinfo.h>

namespace gr {
namespace dtv {

plinfo::plinfo() : d_flags(0), d_segno(0){};
plinfo::plinfo(uint16_t flags, int16_t segno) : d_flags(flags), d_segno(segno){};

void plinfo::reset()
{
    d_flags = 0;
    d_segno = 0;
}

// accessors
bool plinfo::field_sync1_p() const { return (d_flags & fl_field_sync1) != 0; }
bool plinfo::field_sync2_p() const { return (d_flags & fl_field_sync2) != 0; }
bool plinfo::field_sync_p() const { return field_sync1_p() || field_sync2_p(); }

bool plinfo::regular_seg_p() const { return (d_flags & fl_regular_seg) != 0; }

bool plinfo::in_field1_p() const { return (d_flags & fl_field2) == 0; }
bool plinfo::in_field2_p() const { return (d_flags & fl_field2) != 0; }

bool plinfo::first_regular_seg_p() const { return (d_flags & fl_first_regular_seg) != 0; }

bool plinfo::transport_error_p() const { return (d_flags & fl_transport_error) != 0; }

unsigned int plinfo::segno() const { return d_segno; }
unsigned int plinfo::flags() const { return d_flags; }

// setters

void plinfo::set_field_sync1()
{
    d_segno = 0;
    d_flags = fl_field_sync1;
}

void plinfo::set_field_sync2()
{
    d_segno = 0;
    d_flags = fl_field_sync2 | fl_field2;
}

void plinfo::set_regular_seg(bool field2, int segno)
{
    d_segno = segno;
    d_flags = fl_regular_seg;
    if (segno == 0)
        d_flags |= fl_first_regular_seg;
    if (segno >= ATSC_DSEGS_PER_FIELD)
        d_flags |= fl_transport_error;
    if (field2)
        d_flags |= fl_field2;
}

void plinfo::set_transport_error(bool error)
{
    if (error)
        d_flags |= fl_transport_error;
    else
        d_flags &= ~fl_transport_error;
}

void plinfo::delay(plinfo& out, const plinfo& in, int nsegs_of_delay)
{
    assert(in.regular_seg_p());
    assert(nsegs_of_delay >= 0);

    int s = in.segno();
    if (in.in_field2_p())
        s += ATSC_DSEGS_PER_FIELD;

    s -= nsegs_of_delay;
    if (s < 0)
        s += 2 * ATSC_DSEGS_PER_FIELD;

    if (s < ATSC_DSEGS_PER_FIELD)
        out.set_regular_seg(false, s); // field 1
    else
        out.set_regular_seg(true, s - ATSC_DSEGS_PER_FIELD); // field 2
}


} /* namespace dtv */
} /* namespace gr */
