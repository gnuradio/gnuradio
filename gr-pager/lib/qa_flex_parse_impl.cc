/* -*- c++ -*- */
/*
 * Copyright 2016, Andrew Jeffery
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cppunit/TestCase.h>

#include "qa_flex_parse_impl.h"
#include "flex_parse_impl.h"
#include <gnuradio/msg_queue.h>
#include <gnuradio/blocks/message_source.h>
#include <gnuradio/blocks/message_sink.h>

uint32_t inactive_frame_data[88] = {
  0x0000040b, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
};

uint32_t inactive_page_data[88] = {
  0x0000080b, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
};

uint32_t simple_aln_data[88] = {
  0x00000807, 0x001d7edd, 0x0004c1d1, 0x00001a04, 0x00122a06, 0x000829c9,
  0x000829c9, 0x00151041, 0x001529c5, 0x00116820, 0x0013e4d2, 0x0010e4c4,
  0x00106820, 0x000822c7, 0x001462d3, 0x0013a2d5, 0x001064d4, 0x0013904c,
  0x0010a6d5, 0x00082945, 0x000d5c20, 0x0000da33, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
};

std::string simple_aln_expected("148.812""\x80""   1900253""\x80""ALN""\x80""THIS IS A TEST PERIODIC PAGE SEQUENTIAL NUMBER  8534");

uint32_t simple_standard_num_data[88] = {
  0x0000080b, 0x001df8a1, 0x000001b6, 0x0014C840, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
};

std::string simple_standard_num_expected("148.812""\x80""   1931425""\x80""NUM""\x80""0123");

uint32_t simple_numbered_num_data[88] = {
  0x0000080b, 0x001df8a1, 0x000001f6, 0x0014C840, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
};

std::string simple_numbered_num_expected("148.812""\x80""   1931425""\x80""NNM""\x80""23");

uint32_t simple_laddr_aln_data[88] = {
  0x00000C0B, 0x00008000, 0x001F8000, 0x000082D0, 0x00000800, 0x0010E141,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
};

std::string simple_laddr_aln_expected("148.812""\x80""1075810304""\x80""ALN""\x80""ABC");

uint32_t simple_laddr_num_data[88] = {
  0x00000C0B, 0x00008000, 0x001F8000, 0x00000236, 0x0014C840, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
};

std::string simple_laddr_num_expected("148.812""\x80""1075810304""\x80""NUM""\x80""0123");

uint32_t simple_laddr_saddr_data[88] = {
  0x0000100B, 0x00008000, 0x001F8000, 0x001d7edd, 0x00008450, 0x00000000,
  0x0004c4d1, 0x00000800, 0x0010E141, 0x00001a04, 0x00122a06, 0x000829c9,
  0x000829c9, 0x00151041, 0x001529c5, 0x00116820, 0x0013e4d2, 0x0010e4c4,
  0x00106820, 0x000822c7, 0x001462d3, 0x0013a2d5, 0x001064d4, 0x0013904c,
  0x0010a6d5, 0x00082945, 0x000d5c20, 0x0000da33, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
};

std::string simple_laddr_saddr_expected[] = {
  std::string("148.812""\x80""1075810304""\x80""ALN""\x80""ABC"),
  std::string("148.812""\x80""   1900253""\x80""ALN""\x80""THIS IS A TEST PERIODIC PAGE SEQUENTIAL NUMBER  8534"),
};

uint32_t split_aln_two_data[][88] = {
  {
    0x00000c03, 0x001bf7e7, 0x001df8a1, 0x000ec2d6, 0x00062052, 0x0000182c,
    0x0011a6a0, 0x00081d53, 0x0011a1aa, 0x00116953, 0x00125053, 0x000c21ce,
    0x000dd830, 0x000e1920, 0x000e182f, 0x000d98af, 0x000c5820, 0x000e193a,
    0x00116920, 0x0013e853, 0x0008224e, 0x0014aa53, 0x001521d5, 0x00116955,
    0x0013e220, 0x0014e2cd, 0x0010e4d4, 0x0010502c, 0x0014a0cc, 0x0013104d,
    0x00116b45, 0x00081d4c, 0x00081631, 0x000819b1, 0x0010aac1, 0x0013a955,
    0x0015a0a0, 0x001666a0, 0x00132a52, 0x00109045, 0x0012e741, 0x001066ac,
    0x00105d50, 0x00082644, 0x000c59b1, 0x000c6220, 0x00151631, 0x0010d047,
    0x000d1bb1, 0x000c6a20, 0x000b1a38, 0x000f5ea0, 0x0014a4c6, 0x00125045,
    0x0011904e, 0x001266c1, 0x00082ccc, 0x0013e7d2, 0x000e904d, 0x00132241,
    0x000c9832, 0x001323a0, 0x000d1a4f, 0x000e9039, 0x00001e24, 0x0011a6d6,
    0x00081d53, 0x0011a1aa, 0x00116953, 0x00125053, 0x000c21ce, 0x000dd830,
    0x000e1920, 0x000e182f, 0x000d98af, 0x000c5820, 0x000e193a, 0x00116920,
    0x0013e853, 0x0008224e, 0x0014aa53, 0x001521d5, 0x00116955, 0x0013e220,
    0x0014e2cd, 0x0010e4d4, 0x0010502c, 0x0014a0cc,
  },
  {
    0x00000807, 0x001df8a1, 0x000741d6, 0x000001a0, 0x0013104d, 0x00116b45,
    0x00081d4c, 0x00081631, 0x000819b1, 0x0010aac1, 0x0013a955, 0x0015a0a0,
    0x001666a0, 0x00132a52, 0x00109045, 0x0012e741, 0x001066ac, 0x00105d50,
    0x00082644, 0x000c59b1, 0x000c6220, 0x00151631, 0x0010d047, 0x000d1bb1,
    0x000c6a20, 0x000b1a38, 0x00105d20, 0x000ca644, 0x00081930, 0x0013e647,
    0x000e5a34, 0x0000dd20, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  },
};

std::string split_aln_two_expected[] = {
  "148.812""\x80""   1800167""\x80""ALN""\x80""MFS: *CFSRES INC0007 28/08/16 01:28 RESPOND STRUCTURE DOMESTIC, ALARM LEVEL: 1, 13 AUBURN AV MYRTLE BANK,MAP:ADL 131 D11,TG C174 T184, ==FIRE IN FAMILY ROOM :ADL202 GLO449 :",
  "148.812""\x80""   1931425""\x80""ALN""\x80""MFS: *CFSRES INC0007 28/08/16 01:28 RESPOND STRUCTURE DOMESTIC, ALARM LEVEL: 1, 13 AUBURN AV MYRTLE BANK,MAP:ADL 131 D11,TG C174 T184, :ADL202 GLO449 :",
};

uint32_t split_aln_three_data[][88] = {
  {
    0x00000807, 0x001da428, 0x0011c1d3, 0x00001a0f, 0x0013e763, 0x0011a4d4,
    0x001061c9, 0x0013e4d4, 0x0013504e, 0x000ea9c6, 0x0010d520, 0x0014a9c6,
    0x000829c5, 0x0010e749, 0x000d58b0, 0x000c9036, 0x000c17b7, 0x000c57b8,
    0x000c9036, 0x000c5d33, 0x00149032, 0x001429c5, 0x0011274f, 0x00156920,
    0x00126142, 0x00082453, 0x0008294f, 0x0014e0d7, 0x000b22d4, 0x001320a0,
    0x00136941, 0x00116620, 0x001322d6, 0x000c503a, 0x000c502c, 0x00081cb4,
    0x00116b41, 0x00116ace, 0x00112920, 0x001321a0, 0x00116941, 0x001161ce,
    0x001063a0, 0x00116252, 0x000b29ce, 0x001420cd, 0x001120ba, 0x000c504c,
    0x00081cb2, 0x000d98cd, 0x0011ea2c, 0x000c61a0, 0x00081837, 0x000e58d4,
    0x00081630, 0x00081ebd, 0x00106755, 0x00116a54, 0x0011624e, 0x00119044,
    0x00116949, 0x0013a4a0, 0x0014a220, 0x000b26d5, 0x00116920, 0x00082941,
    0x0014a0d9, 0x000e9044, 0x0014a4c1, 0x0014e2c4, 0x00081acb, 0x00136a53,
    0x000c5834, 0x0000dd20, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000,

  },
  {
    0x00000c03, 0x001bf7f1, 0x001df89c, 0x0010c2d3, 0x00042450, 0x00001939,
    0x0011a68a, 0x00081d53, 0x0011a1aa, 0x00116953, 0x00125053, 0x000c21ce,
    0x000d9ab1, 0x000dd920, 0x000e182f, 0x000d98af, 0x000cd920, 0x000c98ba,
    0x00116920, 0x0013e853, 0x0008224e, 0x0010aad2, 0x0014e4c2, 0x0013d048,
    0x0015d052, 0x001529c1, 0x00081645, 0x00106641, 0x000826d2, 0x0015a2cc,
    0x000ea645, 0x000b18a0, 0x000d18a0, 0x00105039, 0x0013a2d6, 0x000822d5,
    0x00082252, 0x00106643, 0x0013a2d2, 0x000822c3, 0x0014a0c7, 0x0013a2c4,
    0x00135653, 0x000ea841, 0x00132241, 0x000c98a0, 0x00135039, 0x000b1b31,
    0x000823d4, 0x000dd8c3, 0x00151030, 0x000c1cb1, 0x000f502c, 0x0015503d,
    0x001520ce, 0x0013a2d4, 0x001122c4, 0x00126320, 0x000822d2, 0x00082749,
    0x00156944, 0x0008164d, 0x001062d2, 0x00165052, 0x00112941, 0x00105d20,
    0x00112949, 0x0012e9c5, 0x0014d035, 0x000d26d4, 0x000818b0, 0x0000c1ba,
    0x00001d46, 0x0011a6cb, 0x00081d53, 0x0011a1aa, 0x00116953, 0x00125053,
    0x000c21ce, 0x000d9ab1, 0x000dd920, 0x000e182f, 0x000d98af, 0x000cd920,
    0x000c98ba, 0x00116920, 0x0013e853, 0x0008224e,
  },
  {
    0x00000807, 0x001df89c, 0x0009c1dc, 0x000001f6, 0x0010aad2, 0x0014e4c2,
    0x0013d048, 0x0015d052, 0x001529c1, 0x00081645, 0x00106641, 0x000826d2,
    0x0015a2cc, 0x000ea645, 0x000b18a0, 0x000d18a0, 0x00105039, 0x0013a2d6,
    0x000822d5, 0x00082252, 0x00106643, 0x0013a2d2, 0x000822c3, 0x0014a0c7,
    0x0013a2c4, 0x00135653, 0x000ea841, 0x00132241, 0x000c98a0, 0x00135039,
    0x000b1b31, 0x000823d4, 0x000dd8c3, 0x00151030, 0x000c1cb1, 0x000e902c,
    0x0014a4c1, 0x0014e2c4, 0x00081acb, 0x00136a53, 0x000c5834, 0x0000dd20,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
    0x001fffff, 0x00000000, 0x001fffff, 0x00000000,
  }
};

std::string split_aln_three_expected[] = {
  std::string("148.812""\x80""   1909800""\x80""ALN\x80""NOTIFICATION MFS: *CFSRES INC0156 27/08/16 23:12 RESPOND RUBBISH OR WASTE, ALARM LEVEL: 1, 149 AVENUE RD CLARENCE GARDENS,MAP:ADL 129 M16,TG C170 T190, == UNATTENDED FIRE IN DRUM, REAR YARD :AIRDESK5 STM401 :"),
  std::string("148.812""\x80""   1800177""\x80""ALN""\x80""MFS: *CFSRES INC0156 27/08/16 23:12 RESPOND RUBBISH OR WASTE, ALARM LEVEL: 1, 149 AVENUE RD CLARENCE GARDENS,MAP:ADL 129 M16,TG C170 T190, == UNATTENDED FIRE IN DRUM, REAR YARD :AIRDESK5 STM401 :"),
  std::string("148.812""\x80""   1931420""\x80""ALN""\x80""MFS: *CFSRES INC0156 27/08/16 23:12 RESPOND RUBBISH OR WASTE, ALARM LEVEL: 1, 149 AVENUE RD CLARENCE GARDENS,MAP:ADL 129 M16,TG C170 T190, :AIRDESK5 STM401 :")
};

#define ASSERT_QHEAD_STREQ(e, q) \
  do { \
    std::string __e_evaluated = e; \
    std::string __found = (q)->delete_head()->to_string(); \
    int __cmp = (__e_evaluated).compare(__found); \
    CPPUNIT_ASSERT(__cmp == 0); \
  } while(0);

namespace gr {
  namespace pager {

    static const unsigned int frequency = 148812000;

    void qa_flex_parse_impl::inactive_frame() {
      int i;

      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 88; i++) {
        in.push_back(&inactive_frame_data[i]);
      }
      fp.work(88, in, out);

      CPPUNIT_ASSERT(0 == q->count());
    }

    void qa_flex_parse_impl::inactive_page() {
      int i;

      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 88; i++) {
        in.push_back(&inactive_page_data[i]);
      }
      fp.work(88, in, out);

      CPPUNIT_ASSERT(0 == q->count());
    }

    void qa_flex_parse_impl::simple_aln() {
      int i;
      std::string parsed;

      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 88; i++) {
        in.push_back(&simple_aln_data[i]);
      }
      fp.work(88, in, out);

      CPPUNIT_ASSERT(1 == q->count());

      ASSERT_QHEAD_STREQ(simple_aln_expected, q);
    }

    void qa_flex_parse_impl::simple_standard_num() {
      int i;
      std::string parsed;

      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 88; i++) {
        in.push_back(&simple_standard_num_data[i]);
      }
      fp.work(88, in, out);

      CPPUNIT_ASSERT(1 == q->count());

      ASSERT_QHEAD_STREQ(simple_standard_num_expected, q);
    }

    void qa_flex_parse_impl::simple_numbered_num() {
      int i;
      std::string parsed;

      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 88; i++) {
        in.push_back(&simple_numbered_num_data[i]);
      }
      fp.work(88, in, out);

      CPPUNIT_ASSERT(1 == q->count());

      ASSERT_QHEAD_STREQ(simple_numbered_num_expected, q);
    }

    void qa_flex_parse_impl::simple_laddr_aln() {
      int i;
      std::string parsed;

      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 88; i++) {
        in.push_back(&simple_laddr_aln_data[i]);
      }
      fp.work(88, in, out);

      CPPUNIT_ASSERT(1 == q->count());

      ASSERT_QHEAD_STREQ(simple_laddr_aln_expected, q);
    }

    void qa_flex_parse_impl::simple_laddr_num() {
      int i;
      std::string parsed;

      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 88; i++) {
        in.push_back(&simple_laddr_num_data[i]);
      }
      fp.work(88, in, out);

      CPPUNIT_ASSERT(1 == q->count());

      ASSERT_QHEAD_STREQ(simple_laddr_num_expected, q);
    }

    void qa_flex_parse_impl::simple_laddr_saddr() {
      int i;
      std::string parsed;

      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 88; i++) {
        in.push_back(&simple_laddr_saddr_data[i]);
      }
      fp.work(88, in, out);

      CPPUNIT_ASSERT(2 == q->count());

      for (i = 0; i < 2; i++) {
        ASSERT_QHEAD_STREQ(simple_laddr_saddr_expected[i], q);
      }
    }

    void qa_flex_parse_impl::split_aln_two() {
      int i, j;
      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 2; i++) {
        in.clear();
        for (j = 0; j < 88; j++) {
          in.push_back(&split_aln_two_data[i][j]);
        }
        fp.work(88, in, out);
      }

      CPPUNIT_ASSERT(2 == q->count());

      for (i = 0; i < 2; i++) {
        ASSERT_QHEAD_STREQ(split_aln_two_expected[i], q);
      }
    }

    void qa_flex_parse_impl::split_aln_three() {
      int i, j;

      gr::msg_queue::sptr q = gr::msg_queue::make(0);
      gr::pager::flex_parse_impl fp(q, frequency);
      gr_vector_const_void_star in;
      gr_vector_void_star out;

      for (i = 0; i < 3; i++) {
        in.clear();
        for (j = 0; j < 88; j++) {
          in.push_back(&split_aln_three_data[i][j]);
        }
        fp.work(88, in, out);
      }

      CPPUNIT_ASSERT(3 == q->count());

      for (i = 0; i < 3; i++) {
        ASSERT_QHEAD_STREQ(split_aln_three_expected[i], q);
      }
    }
  };
};
