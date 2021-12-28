/* -*- c++ -*- */
/*
 * Copyright 2015,2016,2019 Free Software Foundation, Inc.
 * Copyright 2021 Marcus Müller
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "gnuradio/dtv/dvb_config.h"
namespace gr {
namespace dtv {
namespace dvb {
namespace constants {
using r = dvb_code_rate_t;
static const std::map<dtv::dvb_framesize_t, std::map<dtv::dvb_code_rate_t, unsigned int>>
    kbch_mapping{
        { dvb_framesize_t::FECFRAME_NORMAL,
          { { r::C1_4, 16008 },     { r::C1_3, 21408 },     { r::C2_5, 25728 },
            { r::C1_2, 32208 },     { r::C3_5, 38688 },     { r::C2_3, 43040 },
            { r::C3_4, 48408 },     { r::C4_5, 51648 },     { r::C5_6, 53840 },
            { r::C8_9, 57472 },     { r::C9_10, 58192 },    { r::C2_9_VLSNR, 14208 },
            { r::C13_45, 18528 },   { r::C9_20, 28968 },    { r::C90_180, 32208 },
            { r::C96_180, 34368 },  { r::C11_20, 35448 },   { r::C100_180, 35808 },
            { r::C104_180, 37248 }, { r::C26_45, 37248 },   { r::C18_30, 38688 },
            { r::C28_45, 40128 },   { r::C23_36, 41208 },   { r::C116_180, 41568 },
            { r::C20_30, 43008 },   { r::C124_180, 44448 }, { r::C25_36, 44808 },
            { r::C128_180, 45888 }, { r::C13_18, 46608 },   { r::C132_180, 47328 },
            { r::C22_30, 47328 },   { r::C135_180, 48408 }, { r::C140_180, 50208 },
            { r::C7_9, 50208 },     { r::C154_180, 55248 } } },
        { dvb_framesize_t::FECFRAME_SHORT,
          { { r::C1_4, 3072 },
            { r::C1_3, 5232 },
            { r::C2_5, 6312 },
            { r::C1_2, 7032 },
            { r::C3_5, 9552 },
            { r::C2_3, 10632 },
            { r::C3_4, 11712 },
            { r::C4_5, 12432 },
            { r::C5_6, 13152 },
            { r::C8_9, 14232 },
            { r::C11_45, 3792 },
            { r::C4_15, 4152 },
            { r::C14_45, 4872 },
            { r::C7_15, 7392 },
            { r::C8_15, 8472 },
            { r::C26_45, 9192 },
            { r::C32_45, 11352 },
            { r::C1_5_VLSNR_SF2, 2512 },
            { r::C11_45_VLSNR_SF2, 3792 },
            { r::C1_5_VLSNR, 3072 },
            { r::C4_15_VLSNR, 4152 },
            { r::C1_3_VLSNR, 5232 } } },
        { dvb_framesize_t::FECFRAME_MEDIUM,
          {
              { r::C1_5_MEDIUM, 5660 },
              { r::C11_45_MEDIUM, 7740 },
              { r::C1_3_MEDIUM, 10620 },
          } }
    };
} // namespace constants
} // namespace dvb
} // namespace dtv
} // namespace gr
