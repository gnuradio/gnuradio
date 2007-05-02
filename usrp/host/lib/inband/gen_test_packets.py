#!/usr/bin/env python

import random
import struct
from pprint import pprint
from usb_packet import *

MAX_PAYLOAD = 504
TIME_NOW = 0xffffffff


class sequence_generator(object):
    def __init__(self):
        self.i = 0
    
    def __call__(self):
        t = self.i
        self.i += 1
        return t

def gen_shuffled_lengths():
    valid_lengths = range(0, MAX_PAYLOAD+1, 4)  # [0, 4, 8, ... 504]
    random.shuffle(valid_lengths)
    return valid_lengths


class packet_sequence_generator(object):
    def __init__(self, channel, lengths):
        self.next = sequence_generator()
        self.channel = channel
        self.lengths = lengths

    def __call__(self, output_file):
        gen_packet(output_file, self.channel, self.next, self.lengths[0])
        del self.lengths[0]


def gen_packet(output_file, channel, content_generator, payload_len):
    assert (payload_len % 4) == 0
    payload = []
    n_iq = payload_len // 4
    for n in range(n_iq):
        payload.append(content_generator())  # I
        payload.append(content_generator())  # Q
    for n in range(MAX_PAYLOAD // 4 - n_iq):
        payload.append(0x0000)
        payload.append(0xffff)

    assert (len(payload) == MAX_PAYLOAD // 2)

    #print "\npayload_len =", payload_len
    #pprint(payload)

    output_file.write(make_header(FL_START_OF_BURST|FL_END_OF_BURST,
                                  channel, payload_len, TIME_NOW))
    output_file.write(struct.pack('<252h', *payload))


def gen_all_valid_packet_lengths_1_channel(output_file):
    lengths = gen_shuffled_lengths()
    npkts = len(lengths)                # number of packets we'll generator on each stream
    pkt_gen_0 = packet_sequence_generator(0, lengths)
    for i in range(npkts):
        pkt_gen_0(output_file)
    
    assert pkt_gen_0.next() == 16002    # 2*sum(1, 2, ..., 126) == 126 * 127


def gen_all_valid_packet_lengths_2_channels(output_file):
    lengths = gen_shuffled_lengths()
    npkts = len(lengths)                # number of packets we'll generator on each stream
    pkt_gen_0 = packet_sequence_generator(0, lengths)
    pkt_gen_1 = packet_sequence_generator(1, gen_shuffled_lengths())
    pkt_gen = (pkt_gen_0, pkt_gen_1)
    
    which_gen = (npkts * [0]) + (npkts * [1])
    random.shuffle(which_gen)
    
    for i in which_gen:
        pkt_gen[i](output_file)
    
    assert pkt_gen_0.next() == 16002    # 2*sum(1, 2, ..., 126) == 126 * 127
    assert pkt_gen_1.next() == 16002    # 2*sum(1, 2, ..., 126) == 126 * 127

if __name__ == '__main__':
    gen_all_valid_packet_lengths_1_channel(open("all_valid_packet_lengths_1_channel.dat", "w"))
    gen_all_valid_packet_lengths_2_channels(open("all_valid_packet_lengths_2_channels.dat", "w"))
