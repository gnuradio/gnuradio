/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RUNTIME_TAG_CHECKER_H
#define INCLUDED_GR_RUNTIME_TAG_CHECKER_H

#include <gnuradio/tags.h>
#include <algorithm>
#include <vector>

namespace gr {

class tag_checker
{
private:
    std::vector<tag_t> d_tags;
    tag_t d_next_tag;
    bool d_has_next_tag;
    unsigned int d_next_tag_index;

public:
    tag_checker(std::vector<tag_t>& tags) : d_has_next_tag(false), d_next_tag_index(0)
    {
        d_tags = tags;
        std::sort(d_tags.begin(), d_tags.end(), &gr::tag_t::offset_compare);
        if (!d_tags.empty()) {
            d_has_next_tag = true;
            d_next_tag = tags[0];
        }
    };

    ~tag_checker(){};

    void get_tags(std::vector<tag_t>& tag_list, unsigned int offset)
    {
        while (d_has_next_tag && (offset >= d_next_tag.offset)) {
            if (offset == d_next_tag.offset) {
                tag_list.push_back(d_next_tag);
            }
            d_next_tag_index += 1;
            if (d_next_tag_index >= d_tags.size()) {
                d_has_next_tag = false;
            } else {
                d_next_tag = d_tags[d_next_tag_index];
            }
        }
    };
};
} // namespace gr

#endif /* INCLUDED_GR_RUNTIME_TAG_CHECKER_H */
