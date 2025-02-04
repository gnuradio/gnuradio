/*
 * Copyright 2012, 2018 Free Software Foundation, Inc.
 * Copyright 2025 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "pmt/pmt_sugar.h"
#include "gnuradio/tags.h"
#include <type_traits>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(t1_aggregateness)
{
    bool is_aggregate = std::is_aggregate<gr::tag_t>::value;
    BOOST_CHECK(is_aggregate);
}

BOOST_AUTO_TEST_CASE(t2_moveabilitly)
{
    bool is_move_constructable = std::is_move_constructible<gr::tag_t>::value;
    BOOST_CHECK(is_move_constructable);
}

BOOST_AUTO_TEST_CASE(t3_comparison)
{
    gr::tag_t t_early{ .offset = 1234 };
    gr::tag_t t_late{ .offset = 9999 };
    gr::tag_t t_also_late{ .offset = 9999,
                           .value = pmt::mp("can I still retreat from this exam?") };
    BOOST_CHECK(t_early < t_late);
    BOOST_CHECK(!(t_late < t_early));
    BOOST_CHECK(!(t_late < t_late));
    BOOST_CHECK(!(t_late < t_also_late));
    BOOST_CHECK(!(t_also_late < t_late));
}
