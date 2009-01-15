#!/usr/bin/env python

from optparse import OptionParser
from pylab import *
from pprint import pprint
import os.path


def add_entry(d, nspes, speedup, work_incr):
    if d.has_key(work_incr):
        d[work_incr].append((nspes, speedup))
    else:
        d[work_incr] = [(nspes, speedup)]
    
def parse_file(f):
    d = {}
    for line in f:
        items = [float(x) for x in line.split()]
        # print "items =", items
        nspes = items[0]
        work_incr = int(1e6 * items[1])
        speedup = items[4]
        add_entry(d, nspes, speedup, work_incr)
    return d


class plot_data(object):
    def __init__(self, filenames, output_filename):
        self.fig = figure(1, figsize=(8, 6), facecolor='w')
        self.sp = self.fig.add_subplot(1,1,1)
        self.sp.set_xlabel("nspes", fontweight="bold")
        self.sp.set_ylabel("speedup", fontweight="bold")
        self.sp.grid(True)
        # print 'rcParams["legend.fontsize"] =', rcParams["legend.fontsize"]
        rcParams["legend.fontsize"] = 10


        self.markers = {
              5 : 'x',
             10 : 'o',
             50 : 's',
            100 : '^',
            200 : 'D',
            300 : 'v',
            400 : '>',
            500 : 'h'
            }
        
        if len(filenames) == 1:
            f = filenames[0]
            d = parse_file(open(f))
            self.make_single_plot(d, f)
            
        else:
            for f in filenames:
                d = parse_file(open(f))
                self.make_plot(d, f, f == filenames[0])

        if output_filename:
            savefig(output_filename)
        else:
            show()


    def make_single_plot(self, d, filename):
        def style(k):
            return self.markers[k]

        tag, ext = os.path.splitext(os.path.basename(filename))
        title(tag)
        keys = d.keys()
        keys.sort()
        keys.reverse()
        for k in keys:
            vlist = d[k]         # list of 2-tuples
            xs = [v[0] for v in vlist]
            ys = [v[1] for v in vlist]
            plot(xs, ys, style(k), label="%d us" % (k,))

        x = legend(loc=2)

    def make_plot(self, d, filename, first):
        def style(k):
            if first:
                return self.markers[k]
            else:
                return 'k' + self.markers[k]

        tag, ext = os.path.splitext(os.path.basename(filename))
        keys = d.keys()
        keys.sort()
        keys.reverse()
        for k in keys:
            vlist = d[k]         # list of 2-tuples
            xs = [v[0] for v in vlist]
            ys = [v[1] for v in vlist]
            plot(xs, ys, style(k), label="%s %d us" % (tag, k))

        x = legend(loc=2)

def main():
    usage="%prog: [options] input_filename..."
    description = "Plot R*.avg files from benchmark_nop.py"
    parser = OptionParser(usage=usage, description=description)
    parser.add_option('-o', '--output', default=None,  metavar="FILE",
                      help="generate .png file")
    (options, args) = parser.parse_args()
    if len(args) < 1:
        parser.print_help()
        raise SystemExit, 1

    filenames = args
    dc = plot_data(filenames, options.output)


        
if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        pass

