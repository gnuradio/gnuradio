/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_NETWORK_PACKET_HEADERS_H
#define INCLUDED_NETWORK_PACKET_HEADERS_H

#include <gnuradio/network/udp_header_types.h>

class header_seq_num
{
public:
    // size: 8 (64-bit)
    uint64_t seqnum;

    header_seq_num() { seqnum = 0; };
};

class header_seq_plus_size
{
public:
    // size: 10 (80-bit)
    uint64_t seqnum;
    int16_t length;

    header_seq_plus_size()
    {
        seqnum = 0;
        length = 0;
    };
};

class ata_header
{
public:
    // Total size: 64 bytes
    uint8_t group, version, bitsPerSample, binaryPoint;
    uint32_t order;
    uint8_t type, streams, polCode, hdrLen;
    uint32_t src;
    uint32_t chan;
    uint32_t seq;
    double freq;
    double sampleRate;
    float usableFraction;
    float reserved;
    uint64_t absTime;
    uint32_t flags;
    uint32_t len;

    ata_header()
    {
        seq = 0;
        freq = 0.0;
        sampleRate = 0.0;
    };
};

#endif /* INCLUDED_NETWORK_PACKET_HEADERS_H */
