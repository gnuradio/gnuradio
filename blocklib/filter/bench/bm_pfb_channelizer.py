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
from gnuradio.kernel.fft import window
from gnuradio.schedulers import nbt
import sys
import signal
from argparse import ArgumentParser
import time
from gnuradio import filter

class benchmark_pfb_channelizer(gr.flowgraph):

    def __init__(self, args):
        gr.flowgraph.__init__(self)

        ##################################################
        # Variables
        ##################################################
        nsamples = args.samples
        nchans = args.nchans
        attn = args.attenuation
        bufsize = args.buffer_size
                
        taps = firdes.low_pass_2(1, nchans, 1 / 2, 1 / 10,  attenuation_dB=attn,window=window.WIN_BLACKMAN_hARRIS)

        ##################################################
        # Blocks
        ##################################################
        self.nsrc = blocks.null_source()
        self.nsnk = blocks.null_sink(nports=nchans)
        self.hd = streamops.head(int(nsamples))

        if not args.cuda:
            self.channelizer = filter.pfb_channelizer_cc(
                nchans,
                taps,
                1.0)
            # self.s2ss = streamops.stream_to_streams(gr.sizeof_gr_complex, nchans)

            ##################################################
            # Connections
            ##################################################
            # nsnks = []
            for ii in range(nchans):
                # nsnks.append(blocks.null_sink(gr.sizeof_gr_complex*1))
                self.connect(self.channelizer, ii, self.nsnk, ii)
                # self.connect(self.s2ss, ii, self.channelizer, ii)
            # self.connect(self.channelizer, self.nsnk)
            # self.connect(self.hd, 0, self.s2ss, 0)      
            self.connect([self.nsrc, self.hd, self.channelizer])  
            # self.connect(self.nsrc, 0, self.hd, 0)
        else:
            self.channelizer = filter.pfb_channelizer_cc(
                nchans,
                taps,
                1.0,
                impl=filter.pfb_channelizer_cc.cuda)
            # self.s2ss = streamops.stream_to_streams(gr.sizeof_gr_complex, nchans, impl=streamops.stream_to_streams.cuda)

            ##################################################
            # Connections
            ##################################################
            # nsnks = []
            for ii in range(nchans):
                # blk = blocks.null_sink(gr.sizeof_gr_complex*1)
                # nsnks.append(blk)
                self.connect(self.channelizer, ii, self.nsnk, ii).set_custom_buffer(gr.buffer_cuda_properties.make(gr.buffer_cuda_type.D2H).set_buffer_size(bufsize))
                # self.connect(self.s2ss, ii, self.channelizer, ii).set_custom_buffer(gr.buffer_cuda_properties.make(gr.buffer_cuda_type.D2D).set_buffer_size(bufsize))
                # self.connect(self.channelizer, ii, nsnks[ii], 0).set_custom_buffer(gr.buffer_cuda_pinned_properties.make().set_buffer_size(bufsize))
                # self.connect(self.s2ss, ii, self.channelizer, ii).set_custom_buffer(gr.buffer_cuda_pinned_properties.make().set_buffer_size(bufsize))

            # self.connect(self.channelizer, self.nsnk)
            # self.connect(self.hd, 0, self.s2ss, 0).set_custom_buffer(gr.buffer_cuda_properties.make(gr.buffer_cuda_type.H2D).set_buffer_size(bufsize))   
            self.connect(self.hd, 0, self.channelizer, 0).set_custom_buffer(gr.buffer_cuda_properties.make(gr.buffer_cuda_type.H2D).set_buffer_size(bufsize))   
            # self.connect(self.hd, 0, self.s2ss, 0).set_custom_buffer(gr.buffer_cuda_pinned_properties.make().set_buffer_size(bufsize))               
            self.connect(self.nsrc, 0, self.hd, 0).set_custom_buffer(gr.buffer_cpu_vmcirc_properties.make(gr.buffer_cpu_vmcirc_type.AUTO).set_buffer_size(bufsize))

        # sched = nbt.scheduler_nbt("nbtsched")
        # self.add_scheduler(sched)
        # sched.add_block_group([x.base() for x in nsnks])

        # self.validate()


def main(top_block_cls=benchmark_pfb_channelizer, options=None):

    parser = ArgumentParser(description='Run a flowgraph iterating over parameters for benchmarking')
    parser.add_argument('--rt_prio', help='enable realtime scheduling', action='store_true')
    parser.add_argument('--samples', type=int, default=1e8)
    parser.add_argument('--nchans', type=int, default=4)
    parser.add_argument('--attenuation', type=float, default=70)
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
