/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_CATV_TRANSPORT_FRAMING_ENC_BB_IMPL_H
#define INCLUDED_DTV_CATV_TRANSPORT_FRAMING_ENC_BB_IMPL_H

#include <gnuradio/dtv/catv_transport_framing_enc_bb.h>

namespace gr {
namespace dtv {
static const unsigned char crctable[] = {
    0x00, 0x1b, 0x36, 0x2d, 0x6c, 0x77, 0x5a, 0x41, 0xd8, 0xc3, 0xee, 0xf5, 0xb4, 0xaf,
    0x82, 0x99, 0xd3, 0xc8, 0xe5, 0xfe, 0xbf, 0xa4, 0x89, 0x92, 0x0b, 0x10, 0x3d, 0x26,
    0x67, 0x7c, 0x51, 0x4a, 0xc5, 0xde, 0xf3, 0xe8, 0xa9, 0xb2, 0x9f, 0x84, 0x1d, 0x06,
    0x2b, 0x30, 0x71, 0x6a, 0x47, 0x5c, 0x16, 0x0d, 0x20, 0x3b, 0x7a, 0x61, 0x4c, 0x57,
    0xce, 0xd5, 0xf8, 0xe3, 0xa2, 0xb9, 0x94, 0x8f, 0xe9, 0xf2, 0xdf, 0xc4, 0x85, 0x9e,
    0xb3, 0xa8, 0x31, 0x2a, 0x07, 0x1c, 0x5d, 0x46, 0x6b, 0x70, 0x3a, 0x21, 0x0c, 0x17,
    0x56, 0x4d, 0x60, 0x7b, 0xe2, 0xf9, 0xd4, 0xcf, 0x8e, 0x95, 0xb8, 0xa3, 0x2c, 0x37,
    0x1a, 0x01, 0x40, 0x5b, 0x76, 0x6d, 0xf4, 0xef, 0xc2, 0xd9, 0x98, 0x83, 0xae, 0xb5,
    0xff, 0xe4, 0xc9, 0xd2, 0x93, 0x88, 0xa5, 0xbe, 0x27, 0x3c, 0x11, 0x0a, 0x4b, 0x50,
    0x7d, 0x66, 0xb1, 0xaa, 0x87, 0x9c, 0xdd, 0xc6, 0xeb, 0xf0, 0x69, 0x72, 0x5f, 0x44,
    0x05, 0x1e, 0x33, 0x28, 0x62, 0x79, 0x54, 0x4f, 0x0e, 0x15, 0x38, 0x23, 0xba, 0xa1,
    0x8c, 0x97, 0xd6, 0xcd, 0xe0, 0xfb, 0x74, 0x6f, 0x42, 0x59, 0x18, 0x03, 0x2e, 0x35,
    0xac, 0xb7, 0x9a, 0x81, 0xc0, 0xdb, 0xf6, 0xed, 0xa7, 0xbc, 0x91, 0x8a, 0xcb, 0xd0,
    0xfd, 0xe6, 0x7f, 0x64, 0x49, 0x52, 0x13, 0x08, 0x25, 0x3e, 0x58, 0x43, 0x6e, 0x75,
    0x34, 0x2f, 0x02, 0x19, 0x80, 0x9b, 0xb6, 0xad, 0xec, 0xf7, 0xda, 0xc1, 0x8b, 0x90,
    0xbd, 0xa6, 0xe7, 0xfc, 0xd1, 0xca, 0x53, 0x48, 0x65, 0x7e, 0x3f, 0x24, 0x09, 0x12,
    0x9d, 0x86, 0xab, 0xb0, 0xf1, 0xea, 0xc7, 0xdc, 0x45, 0x5e, 0x73, 0x68, 0x29, 0x32,
    0x1f, 0x04, 0x4e, 0x55, 0x78, 0x63, 0x22, 0x39, 0x14, 0x0f, 0x96, 0x8d, 0xa0, 0xbb,
    0xfa, 0xe1, 0xcc, 0xd7
};

static const unsigned char BitReverseTable[] = {
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0,
    0x70, 0xF0, 0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8,
    0x38, 0xB8, 0x78, 0xF8, 0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94,
    0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4, 0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC,
    0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC, 0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2,
    0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 0x0A, 0x8A, 0x4A, 0xCA,
    0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA, 0x06, 0x86,
    0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
    0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE,
    0x7E, 0xFE, 0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1,
    0x31, 0xB1, 0x71, 0xF1, 0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99,
    0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9, 0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
    0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5, 0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD,
    0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD, 0x03, 0x83, 0x43, 0xC3,
    0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3, 0x0B, 0x8B,
    0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
    0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7,
    0x77, 0xF7, 0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF,
    0x3F, 0xBF, 0x7F, 0xFF
};

class catv_transport_framing_enc_bb_impl : public catv_transport_framing_enc_bb
{
private:
    unsigned char compute_sum(const unsigned char* bytes);

public:
    catv_transport_framing_enc_bb_impl();
    ~catv_transport_framing_enc_bb_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_CATV_TRANSPORT_FRAMING_ENC_BB_IMPL_H */
