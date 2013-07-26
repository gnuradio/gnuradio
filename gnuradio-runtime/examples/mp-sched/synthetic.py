#!/usr/bin/env python
#
# Copyright 2008,2013 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#

from gnuradio import gr, eng_notation
from gnuradio import blocks, filter
from gnuradio.eng_option import eng_option
from optparse import OptionParser
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
        taps = ntaps*[1.0/ntaps]
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
        parser=OptionParser(option_class=eng_option)
        parser.add_option("-p", "--npipelines", type="intx", default=1,
                          metavar="NPIPES", help="the number of pipelines to create (default=%default)")
        parser.add_option("-s", "--nstages", type="intx", default=1,
                          metavar="NSTAGES", help="the number of stages in each pipeline (default=%default)")
        parser.add_option("-N", "--nsamples", type="eng_float", default=default_nsamples,
                          help=("the number of samples to run through the graph (default=%s)" %
                                (eng_notation.num_to_str(default_nsamples))))
        parser.add_option("-m", "--machine-readable", action="store_true", default=False,
                          help="enable machine readable output")

        (options, args) = parser.parse_args()
        if len(args) != 0:
            parser.print_help()
            raise SystemExit, 1

        self.npipes = options.npipelines
        self.nstages = options.nstages
        self.nsamples = options.nsamples
        self.machine_readable = options.machine_readable

        ntaps = 256

        # Something vaguely like floating point ops
        self.flop = 2 * ntaps * options.npipelines * options.nstages * options.nsamples

        src = blocks.null_source(gr.sizeof_float)
        head = blocks.head(gr.sizeof_float, int(options.nsamples))
        self.connect(src, head)

        for n in range(options.npipelines):
            self.connect(head, pipeline(options.nstages, ntaps))


def time_it(tb):
    start = os.times()
    tb.run()
    stop = os.times()
    delta = map((lambda a, b: a-b), stop, start)
    user, sys, childrens_user, childrens_sys, real = delta
    total_user = user + childrens_user
    total_sys  = sys + childrens_sys
    if tb.machine_readable:
        print "%3d %3d %.3e %7.3f %7.3f %7.3f %7.3f %.6e %.3e" % (
            tb.npipes, tb.nstages, tb.nsamples, real, total_user, total_sys, (total_user+total_sys)/real, tb.flop, tb.flop/real)
    else:
        print "npipes           %7d"   % (tb.npipes,)
        print "nstages          %7d"   % (tb.nstages,)
        print "nsamples         %s"    % (eng_notation.num_to_str(tb.nsamples),)
        print "real             %7.3f" % (real,)
        print "user             %7.3f" % (total_user,)
        print "sys              %7.3f" % (total_sys,)
        print "(user+sys)/real  %7.3f" % ((total_user + total_sys)/real,)
        print "pseudo_flop      %s"    % (eng_notation.num_to_str(tb.flop),)
        print "pseudo_flop/real %s"    % (eng_notation.num_to_str(tb.flop/real),)


if __name__ == "__main__":
    try:
        tb = top()
        time_it(tb)
    except KeyboardInterrupt:
        raise SystemExit, 128




