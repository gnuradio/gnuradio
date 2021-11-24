#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr
import pmt


def make_lengthtags(lengths, offsets, tagname='length', vlen=1):
    tags = []
    assert(len(offsets) == len(lengths))
    for offset, length in zip(offsets, lengths):
        tag = gr.tag_t()
        tag.offset = offset // vlen
        tag.key = pmt.string_to_symbol(tagname)
        tag.value = pmt.from_long(length // vlen)
        tags.append(tag)
    return tags


def string_to_vector(string):
    v = []
    for s in string:
        v.append(ord(s))
    return v


def strings_to_vectors(strings, tsb_tag_key):
    vs = [string_to_vector(string) for string in strings]
    return packets_to_vectors(vs, tsb_tag_key)


def vector_to_string(v):
    s = []
    for d in v:
        s.append(chr(d))
    return ''.join(s)


def vectors_to_strings(data, tags, tsb_tag_key):
    packets = vectors_to_packets(data, tags, tsb_tag_key)
    return [vector_to_string(packet) for packet in packets]


def count_bursts(data, tags, tsb_tag_key, vlen=1):
    lengthtags = [t for t in tags
                  if pmt.symbol_to_string(t.key) == tsb_tag_key]
    lengths = {}
    for tag in lengthtags:
        if tag.offset in lengths:
            raise ValueError(
                "More than one tags with key {0} with the same offset={1}."
                .format(tsb_tag_key, tag.offset))
        lengths[tag.offset] = pmt.to_long(tag.value) * vlen
    in_burst = False
    in_packet = False
    packet_length = None
    packet_pos = None
    burst_count = 0
    for pos in range(len(data)):
        if pos in lengths:
            if in_packet:
                print("Got tag at pos {0} current packet_pos is {1}".format(
                    pos, packet_pos))
                raise Exception("Received packet tag while in packet.")
            packet_pos = -1
            packet_length = lengths[pos]
            in_packet = True
            if not in_burst:
                burst_count += 1
            in_burst = True
        elif not in_packet:
            in_burst = False
        if in_packet:
            packet_pos += 1
            if packet_pos == packet_length - 1:
                in_packet = False
                packet_pos = None
    return burst_count


def vectors_to_packets(data, tags, tsb_tag_key, vlen=1):
    lengthtags = [t for t in tags
                  if pmt.symbol_to_string(t.key) == tsb_tag_key]
    lengths = {}
    for tag in lengthtags:
        if tag.offset in lengths:
            raise ValueError(
                "More than one tags with key {0} with the same offset={1}."
                .format(tsb_tag_key, tag.offset))
        lengths[tag.offset] = pmt.to_long(tag.value) * vlen
    if 0 not in lengths:
        raise ValueError("There is no tag with key {0} and an offset of 0"
                         .format(tsb_tag_key))
    pos = 0
    packets = []
    while pos < len(data):
        if pos not in lengths:
            raise ValueError("There is no tag with key {0} and an offset of {1}."
                             "We were expecting one."
                             .format(tsb_tag_key, pos))
        length = lengths[pos]
        if length == 0:
            raise ValueError("Packets cannot have zero length.")
        if pos + length > len(data):
            raise ValueError("The final packet is incomplete.")
        packets.append(data[pos: pos + length])
        pos += length
    return packets


def packets_to_vectors(packets, tsb_tag_key, vlen=1):
    """ Returns a single data vector and a set of tags.
    If used with blocks.vector_source_X, this set of data
    and tags will produced a correct tagged stream. """
    tags = []
    data = []
    offset = 0
    for packet in packets:
        data.extend(packet)
        tag = gr.tag_t()
        tag.offset = offset // vlen
        tag.key = pmt.string_to_symbol(tsb_tag_key)
        tag.value = pmt.from_long(len(packet) // vlen)
        tags.append(tag)
        offset = offset + len(packet)
    return data, tags
