/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#ifndef INCLUDED_GR_DTV_ATSC_SYMINFO_H
#define INCLUDED_GR_DTV_ATSC_SYMINFO_H

namespace gr {
namespace dtv {
namespace atsc {

static constexpr unsigned int SI_SEGMENT_NUM_MASK = 0x1ff;
static constexpr unsigned int SI_FIELD_SYNC_SEGMENT_NUM =
    SI_SEGMENT_NUM_MASK; // conceptually -1

struct syminfo {
    unsigned int symbol_num : 10; // 0..831
    unsigned int segment_num : 9; // 0..311 and SI_FIELD_SYNC_SEGMENT_NUM
    unsigned int field_num : 1;   // 0..1
    unsigned int valid : 1;       // contents are valid
};


static inline bool tag_is_start_field_sync(syminfo tag)
{
    return tag.symbol_num == 0 && tag.segment_num == SI_FIELD_SYNC_SEGMENT_NUM &&
           tag.valid;
}

static inline bool tag_is_start_field_sync_1(syminfo tag)
{
    return tag_is_start_field_sync(tag) && tag.field_num == 0;
}

static inline bool tag_is_start_field_sync_2(syminfo tag)
{
    return tag_is_start_field_sync(tag) && tag.field_num == 1;
}

} /* namespace atsc */
} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_GR_DTV_ATSC_SYMINFO_H */
