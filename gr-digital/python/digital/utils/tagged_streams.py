#!/usr/bin/env python
#
# Copyright 2013 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

# DEPRECATED -- Marked for removal in 3.8

from gnuradio import gr
import pmt

def make_lengthtags(lengths, offsets, tagname='length', vlen=1):
    tags = []
    assert(len(offsets) == len(lengths))
    for offset, length in zip(offsets, lengths):
        tag = gr.tag_t()
        tag.offset = offset/vlen
        tag.key = pmt.string_to_symbol(tagname)
        tag.value = pmt.from_long(length/vlen)
        tags.append(tag)
    return tags

def string_to_vector(string):
    v = []
    for s in string:
        v.append(ord(s))
    return v

def strings_to_vectors(strings, lengthtagname):
    vs = [string_to_vector(string) for string in strings]
    return packets_to_vectors(vs, lengthtagname)

def vector_to_string(v):
    s = []
    for d in v:
        s.append(chr(d))
    return ''.join(s)

def vectors_to_strings(data, tags, lengthtagname):
    packets = vectors_to_packets(data, tags, lengthtagname)
    return [vector_to_string(packet) for packet in packets]

def count_bursts(data, tags, lengthtagname, vlen=1):
    lengthtags = [t for t in tags
                  if pmt.symbol_to_string(t.key) == lengthtagname]
    lengths = {}
    for tag in lengthtags:
        if tag.offset in lengths:
            raise ValueError(
                "More than one tags with key {0} with the same offset={1}."
                .format(lengthtagname, tag.offset))
        lengths[tag.offset] = pmt.to_long(tag.value)*vlen
    in_burst = False
    in_packet = False
    packet_length = None
    packet_pos = None
    burst_count = 0
    for pos in range(len(data)):
        if pos in lengths:
            if in_packet:
                print("Got tag at pos {0} current packet_pos is {1}".format(pos, packet_pos))
                raise StandardError("Received packet tag while in packet.")
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
            if packet_pos == packet_length-1:
                in_packet = False
                packet_pos = None
    return burst_count

def vectors_to_packets(data, tags, lengthtagname, vlen=1):
    lengthtags = [t for t in tags
                  if pmt.symbol_to_string(t.key) == lengthtagname]
    lengths = {}
    for tag in lengthtags:
        if tag.offset in lengths:
            raise ValueError(
                "More than one tags with key {0} with the same offset={1}."
                .format(lengthtagname, tag.offset))
        lengths[tag.offset] = pmt.to_long(tag.value)*vlen
    if 0 not in lengths:
        raise ValueError("There is no tag with key {0} and an offset of 0"
                         .format(lengthtagname))
    pos = 0
    packets = []
    while pos < len(data):
        if pos not in lengths:
            raise ValueError("There is no tag with key {0} and an offset of {1}."
                             "We were expecting one."
                             .format(lengthtagname, pos))
        length = lengths[pos]
        if length == 0:
            raise ValueError("Packets cannot have zero length.")
        if pos+length > len(data):
            raise ValueError("The final packet is incomplete.")
        packets.append(data[pos: pos+length])
        pos += length
    return packets

def packets_to_vectors(packets, lengthtagname, vlen=1):
    tags = []
    data = []
    offset = 0
    for packet in packets:
        data.extend(packet)
        tag = gr.tag_t()
        tag.offset = offset/vlen
        tag.key = pmt.string_to_symbol(lengthtagname)
        tag.value = pmt.from_long(len(packet)/vlen)
        tags.append(tag)
        offset = offset + len(packet)
    return data, tags
