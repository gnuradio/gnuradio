#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Not titled yet
# GNU Radio version: 3.9.0.0-git

from gnuradio import gr, blocks, streamops, fft
from gnuradio.kernel.filter import firdes
from gnuradio.schedulers import nbt
import sys
import signal
from argparse import ArgumentParser
from gnuradio.fft import window
import time
from gnuradio import filter


class benchmark_cuda_fft(gr.flowgraph):

    def __init__(self, args):
        gr.flowgraph.__init__(self)

        ##################################################
        # Variables
        ##################################################
        nsamples = args.samples
        nblocks = args.nblocks
        fftsize = args.fft_size
        bufsize = args.buffer_size

        ##################################################
        # Blocks
        ##################################################
        self.nsrc = blocks.null_source(gr.sizeof_gr_complex*fftsize)
        self.nsnk = blocks.null_sink(gr.sizeof_gr_complex*fftsize)
        self.hd = streamops.head(gr.sizeof_gr_complex *
                              fftsize, int(nsamples) // fftsize)

        blks = []
        if not args.cuda:
            for ii in range(nblocks):
                if ii % 2 == 0:
                    blks.append(fft.fft_cc_fwd(fftsize, [], False))
                else:
                    blks.append(fft.fft_cc_rev(fftsize, [], False))
                if (ii > 0):
                    self.connect(blks[ii-1], 0, blks[ii], 0)

            self.connect(self.hd, 0, blks[0], 0)
            self.connect(self.nsrc, 0, self.hd, 0)
            self.connect(blks[nblocks-1], 0, self.nsnk, 0)
        else:
            for ii in range(nblocks):
                if ii % 2 == 0:
                    blks.append(fft.fft_cc_fwd(fftsize, [], False))
                else:
                    blks.append(fft.fft_cc_rev(fftsize, [], False))
                if (ii > 0):
                    self.connect(blks[ii-1], 0, blks[ii], 0).set_custom_buffer(
                        gr.buffer_cuda_properties.make(gr.buffer_cuda_type.D2D).set_buffer_size(bufsize))

            self.connect(self.hd, 0, self.blks[0], 0)  .set_custom_buffer(
                gr.buffer_cuda_properties.make(gr.buffer_cuda_type.D2H).set_buffer_size(bufsize))
            self.connect(self.nsrc, 0, self.hd, 0).set_custom_buffer(
                gr.buffer_cuda_properties.make(gr.buffer_cuda_type.H2D).set_buffer_size(bufsize))
            self.connect(blks[nblocks-1], 0, self.nsnk).set_custom_buffer(
                gr.buffer_cuda_properties.make(gr.buffer_cuda_type.H2D).set_buffer_size(bufsize))

        # sched = nbt.scheduler_nbt("nbtsched")
        # self.add_scheduler(sched)
        # sched.add_block_group([x.base() for x in nsnks])

        # self.validate()


def main(top_block_cls=benchmark_cuda_fft, options=None):

    parser = ArgumentParser(
        description='Run a flowgraph iterating over parameters for benchmarking')
    parser.add_argument(
        '--rt_prio', help='enable realtime scheduling', action='store_true')
    parser.add_argument('--samples', type=int, default=1e8)
    parser.add_argument('--nblocks', type=int, default=4)
    parser.add_argument('--fft_size', type=int, default=1024)
    parser.add_argument('--buffer_size', type=int, default=8192)
    parser.add_argument('--cuda', action='store_true')

    args = parser.parse_args()
    print(args)

    if args.rt_prio and gr.enable_realtime_scheduling() != gr.RT_OK:
        print("Error: failed to enable real-time scheduling.")

    tb = top_block_cls(args)

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()
        sys.exit(0)

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    print("starting ...")
    startt = time.time()
    tb.start()

    tb.wait()
    endt = time.time()
    print(f'[PROFILE_TIME]{endt-startt}[PROFILE_TIME]')


if __name__ == '__main__':
    main()
