#!/usr/bin/env python
#
# Copyright 2008,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#

from gnuradio import gr, eng_notation
from gnuradio import blocks, filter
from gnuradio.eng_arg import eng_float, intx
from argparse import ArgumentParser
import os


class pipeline(gr.hier_block2):
    def __init__(self, nstages, ntaps=256):
        """
        Create a pipeline of nstages of filter.fir_filter_fff's connected in serial
        terminating in a blocks.null_sink.
        """
        gr.hier_block2.__init__(self, "pipeline",
                                gr.io_signature(1, 1, gr.sizeof_float),
                                gr.io_signature(0, 0, 0))
        taps = ntaps * [1.0 / ntaps]
        upstream = self
        for i in range(nstages):
            op = filter.fir_filter_fff(1, taps)
            self.connect(upstream, op)
            upstream = op

        self.connect(upstream, blocks.null_sink(gr.sizeof_float))


class top(gr.top_block):
    def __init__(self):
        gr.top_block.__init__(self)

        default_nsamples = 10e6
        parser = ArgumentParser()
        parser.add_argument("-p", "--npipelines", type=intx, default=1,
                            metavar="NPIPES", help="the number of pipelines to create (default=%(default)s)")
        parser.add_argument("-s", "--nstages", type=intx, default=1, metavar="NSTAGES",
                            help="the number of stages in each pipeline (default=%(default)s)")
        parser.add_argument("-N", "--nsamples", type=eng_float, default=default_nsamples,
                            help=("the number of samples to run through the graph (default=%s)" %
                                  (eng_notation.num_to_str(default_nsamples))))
        parser.add_argument("-m", "--machine-readable", action="store_true", default=False,
                            help="enable machine readable output")

        args = parser.parse_args()

        self.npipes = args.npipelines
        self.nstages = args.nstages
        self.nsamples = args.nsamples
        self.machine_readable = args.machine_readable

        ntaps = 256

        # Something vaguely like floating point ops
        self.flop = 2 * ntaps * args.npipelines * args.nstages * args.nsamples

        src = blocks.null_source(gr.sizeof_float)
        head = blocks.head(gr.sizeof_float, int(args.nsamples))
        self.connect(src, head)

        for n in range(args.npipelines):
            self.connect(head, pipeline(args.nstages, ntaps))


def time_it(tb):
    start = os.times()
    tb.run()
    stop = os.times()
    delta = list(map((lambda a, b: a - b), stop, start))
    user, sys, childrens_user, childrens_sys, real = delta
    total_user = user + childrens_user
    total_sys = sys + childrens_sys
    if tb.machine_readable:
        print("%3d %3d %.3e %7.3f %7.3f %7.3f %7.3f %.6e %.3e" % (
            tb.npipes, tb.nstages, tb.nsamples, real, total_user, total_sys, (total_user + total_sys) / real, tb.flop, tb.flop / real))
    else:
        print("npipes           %7d" % (tb.npipes,))
        print("nstages          %7d" % (tb.nstages,))
        print("nsamples         %s" % (eng_notation.num_to_str(tb.nsamples),))
        print("real             %7.3f" % (real,))
        print("user             %7.3f" % (total_user,))
        print("sys              %7.3f" % (total_sys,))
        print("(user+sys)/real  %7.3f" % ((total_user + total_sys) / real,))
        print("pseudo_flop      %s" % (eng_notation.num_to_str(tb.flop),))
        print("pseudo_flop/real %s" %
              (eng_notation.num_to_str(tb.flop / real),))


if __name__ == "__main__":
    try:
        tb = top()
        time_it(tb)
    except KeyboardInterrupt:
        raise SystemExit(128)
