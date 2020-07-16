#!/usr/bin/env python
#
# Copyright 2014,2019 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

"""
TXs a waveform (either from a file, or a sinusoid) in a frequency-hopping manner.
"""

import argparse
import numpy
import pmt
from gnuradio import gr
from gnuradio import blocks
from gnuradio import uhd

def setup_parser():
    """ Setup the parser for the frequency hopper. """
    parser = argparse.ArgumentParser(
        description="Transmit a signal in a frequency-hopping manner, using tx_freq tags."
    )
    parser.add_argument(
        '-i', '--input-file',
        help="File with samples to transmit. If left out, will transmit a sinusoid.")
    parser.add_argument(
        "-a", "--args", default="",
        help="UHD device address args.")
    parser.add_argument(
        "--spec", default="",
        help="UHD subdev spec.")
    parser.add_argument(
        "--antenna", default="",
        help="UHD antenna settings.")
    parser.add_argument(
        "--gain", default=None, type=float,
        help="USRP gain (defaults to mid-point in dB).")
    parser.add_argument(
        "-r", "--rate", type=float, default=1e6,
        help="Sampling rate")
    parser.add_argument(
        "-N", "--samp-per-burst", type=int, default=10000,
        help="Samples per burst")
    parser.add_argument(
        "-t", "--hop-time", type=float, default=1000,
        help="Time between hops in milliseconds. This must be larger than or "
             "equal to the burst duration as set by --samp-per-burst")
    parser.add_argument(
        "-f", "--freq", type=float, default=2.45e9,
        help="Base frequency. This is the middle channel frequency at which "
             "the USRP will Tx.")
    parser.add_argument(
        "--dsp", action='store_true',
        help="DSP tuning only.")
    parser.add_argument(
        "-d", "--freq-delta", type=float, default=1e6,
        help="Channel spacing.")
    parser.add_argument(
        "-c", "--num-channels", type=int, default=5,
        help="Number of channels.")
    parser.add_argument(
        "-B", "--num-bursts", type=int, default=30,
        help="Number of bursts to transmit before terminating.")
    parser.add_argument(
        "-p", "--post-tuning", action='count',
        help="Tune after transmitting. Default is to tune immediately before "
             "transmitting.")
    parser.add_argument(
        "-v", "--verbose", action='count',
        help="Print more information. The morer the printier.")
    return parser


class FrequencyHopperSrc(gr.hier_block2):
    """ Provides tags for frequency hopping """
    def __init__(
            self,
            n_bursts, n_channels,
            freq_delta, base_freq, dsp_tuning,
            burst_length, base_time, hop_time,
            post_tuning=False,
            tx_gain=0,
            verbose=False
        ):
        gr.hier_block2.__init__(
            self, "FrequencyHopperSrc",
            gr.io_signature(1, 1, gr.sizeof_gr_complex),
            gr.io_signature(1, 1, gr.sizeof_gr_complex),
        )
        n_samples_total = n_bursts * burst_length
        lowest_frequency = base_freq - numpy.floor(n_channels/2) * freq_delta
        self.hop_sequence = [lowest_frequency + n * freq_delta for n in range(n_channels)]
        numpy.random.shuffle(self.hop_sequence)
        # Repeat that:
        self.hop_sequence = [self.hop_sequence[x % n_channels] for x in range(n_bursts)]
        if verbose:
            print("Hop Frequencies  | Hop Pattern")
            print("=================|================================")
            for f in self.hop_sequence:
                print("{:6.3f} MHz      |  ".format(f/1e6), end='')
                if n_channels < 50:
                    print(" " * int((f - base_freq) / freq_delta) + "#")
                else:
                    print("\n")
            print("=================|================================")
        # There's no real point in setting the gain via tag for this application,
        # but this is an example to show you how to do it.
        gain_tag = gr.tag_t()
        gain_tag.offset = 0
        gain_tag.key = pmt.string_to_symbol('tx_command')
        gain_tag.value = pmt.to_pmt({'gain': tx_gain})
        tag_list = [gain_tag,]
        for i in range(len(self.hop_sequence)):
            time = pmt.cons(
                pmt.from_uint64(int(base_time + i * hop_time+0.01)),
                pmt.from_double((base_time + i * hop_time+0.01) % 1),
            )
            tune_tag = gr.tag_t()
            tune_tag.offset = i * burst_length
            # TODO dsp_tuning should also be able to do post_tuning
            if i > 0 and post_tuning and not dsp_tuning:
                tune_tag.offset -= 1 # Move it to last sample of previous burst
            if dsp_tuning:
                tune_tag.key = pmt.string_to_symbol('tx_command')
                tune_tag.value = pmt.to_pmt({'lo_freq': base_freq, 'dsp_freq': base_freq - self.hop_sequence[i]})
                tune_tag.value = pmt.dict_add(tune_tag.value, pmt.intern("time"),time)
            else:
                tune_tag.key = pmt.string_to_symbol('tx_command')
                tune_tag.value = pmt.to_pmt({'freq': self.hop_sequence[i]})
                tune_tag.value = pmt.dict_add(tune_tag.value, pmt.intern('time'), time)
            tag_list.append(tune_tag)
            length_tag = gr.tag_t()
            length_tag.offset = i * burst_length
            length_tag.key = pmt.string_to_symbol('packet_len')
            length_tag.value = pmt.from_long(burst_length)
            tag_list.append(length_tag)
            time_tag = gr.tag_t()
            time_tag.offset = i * burst_length
            time_tag.key = pmt.string_to_symbol('tx_time')
            time_tag.value = pmt.make_tuple(
                    pmt.car(time),
                    pmt.cdr(time)
            )
            tag_list.append(time_tag)
        tag_source = blocks.vector_source_c((1.0,) * n_samples_total, repeat=False, tags=tag_list)
        mult = blocks.multiply_cc()
        self.connect(self, mult, self)
        self.connect(tag_source, (mult, 1))


class FlowGraph(gr.top_block):
    """ Flow graph that does the frequency hopping. """
    def __init__(self, args):
        gr.top_block.__init__(self)
        if args.input_file is not None:
            src = blocks.file_source(gr.sizeof_gr_complex, args.filename, repeat=True)
        else:
            src = blocks.vector_source_c((.5,) * int(1e6) * 2, repeat=True)
        # Setup USRP
        self.usrp = uhd.usrp_sink(
            args.args,
            uhd.stream_args('fc32'),
            "packet_len"
        )
        if args.spec:
            self.usrp.set_subdev_spec(args.spec, 0)
        if args.antenna:
            self.usrp.set_antenna(args.antenna, 0)
        self.usrp.set_samp_rate(args.rate)
        # Gain is set in the hopper block
        if not args.gain:
            gain_range = self.usrp.get_gain_range()
            args.gain = float(gain_range.start() + gain_range.stop()) / 2.0
        print("-- Setting gain to {} dB".format(args.gain))
        if not self.usrp.set_center_freq(args.freq):
            print('[ERROR] Failed to set base frequency.')
            exit(1)
        hopper_block = FrequencyHopperSrc(
            args.num_bursts, args.num_channels,
            args.freq_delta, args.freq, args.dsp,
            args.samp_per_burst, 1.0, args.hop_time / 1000.,
            args.post_tuning,
            args.gain,
            args.verbose,
        )
        self.connect(src, hopper_block, self.usrp)

def print_hopper_stats(args):
    """ Nothing to do with Grace Hopper """
    print("""
Parameter          | Value
===================+=========================
Hop Interval       | {hop_time} ms
Burst duration     | {hop_duration} ms
Lowest Frequency   | {lowest_freq:6.3f} MHz
Highest Frequency  | {highest_freq:6.3f} MHz
Frequency spacing  | {freq_delta:6.4f} MHz
Number of channels | {num_channels}
Sampling rate      | {rate} Msps
Transmit Gain      | {gain} dB
===================+=========================
    """.format(
        hop_time=args.hop_time,
        hop_duration=1000.0/args.rate*args.samp_per_burst,
        gain=args.gain if args.gain else "(midpoint)",
        lowest_freq=args.freq/1e6,
        highest_freq=(args.freq + (args.num_channels-1) * args.freq_delta)/1e6,
        freq_delta=args.freq_delta/1e6,
        num_channels=args.num_channels,
        rate=args.rate/1e6,
        ))

def main():
    """ Go, go, go! """
    args = setup_parser().parse_args()
    if (1.0 * args.samp_per_burst / args.rate) > args.hop_time * 1e-3:
        print("Burst duration must be smaller than hop time.")
        exit(1)
    if args.verbose:
        print_hopper_stats(args)
    top_block = FlowGraph(args)
    print("Starting to hop, skip and jump... press Ctrl+C to exit.")
    top_block.usrp.set_time_now(uhd.time_spec(0.0))
    top_block.run()

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass
