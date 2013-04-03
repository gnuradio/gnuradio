#!/usr/bin/env python

from gnuradio import gr
from gnuradio import blocks
import argparse
from volk_test_funcs import *

try:
    from gnuradio import blocks
except ImportError:
    sys.stderr.write("This example requires gr-blocks.\n")

def multiply_const_cc(N):
    k = 3.3
    op = blocks.multiply_const_cc(k)
    tb = helper(N, op, gr.sizeof_gr_complex, gr.sizeof_gr_complex, 1, 1)
    return tb

######################################################################

def multiply_const_ff(N):
    k = 3.3
    op = blocks.multiply_const_ff(k)
    tb = helper(N, op, gr.sizeof_float, gr.sizeof_float, 1, 1)
    return tb

######################################################################

def multiply_cc(N):
    op = blocks.multiply_cc(1)
    tb = helper(N, op, gr.sizeof_gr_complex, gr.sizeof_gr_complex, 2, 1)
    return tb

######################################################################

def multiply_ff(N):
    op = blocks.multiply_ff()
    tb = helper(N, op, gr.sizeof_float, gr.sizeof_float, 2, 1)
    return tb

######################################################################

def add_ff(N):
    op = blocks.add_ff()
    tb = helper(N, op, gr.sizeof_float, gr.sizeof_float, 2, 1)
    return tb

######################################################################

def conjugate_cc(N):
    op = blocks.conjugate_cc()
    tb = helper(N, op, gr.sizeof_gr_complex, gr.sizeof_gr_complex, 1, 1)
    return tb

######################################################################

def multiply_conjugate_cc(N):
    try:
        op = blocks.multiply_conjugate_cc()
        tb = helper(N, op, gr.sizeof_gr_complex, gr.sizeof_gr_complex, 2, 1)
        return tb

    except AttributeError:
        class s(gr.hier_block2):
            def __init__(self):
                gr.hier_block2.__init__(self, "s",
                                        gr.io_signature(2, 2, gr.sizeof_gr_complex),
                                        gr.io_signature(1, 1, gr.sizeof_gr_complex))
                conj = blocks.conjugate_cc()
                mult = blocks.multiply_cc()
                self.connect((self,0), (mult,0))
                self.connect((self,1), conj, (mult,1))
                self.connect(mult, self)

        op = s()
        tb = helper(N, op, gr.sizeof_gr_complex, gr.sizeof_gr_complex, 2, 1)
        return tb


######################################################################

def run_tests(func, N, iters):
    print("Running Test: {0}".format(func.__name__))
    try:
        tb = func(N)
        t = timeit(tb, iters)
        res = format_results(func.__name__, t)
        return res
    except AttributeError:
        print "\tCould not run test. Skipping."
        return None

def main():
    avail_tests = [multiply_const_cc,
                   multiply_const_ff,
                   multiply_cc,
                   multiply_ff,
                   add_ff,
                   conjugate_cc,
                   multiply_conjugate_cc]

    desc='Time an operation to compare with other implementations. \
          This program runs a simple GNU Radio flowgraph to test a \
          particular math function, mostly to compare the  \
          Volk-optimized implementation versus a regular \
          implementation. The results are stored to an SQLite database \
          that can then be read by volk_plot.py to plot the differences.'
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument('-L', '--label', type=str,
                        required=True, default=None,
                        help='Label of database table [default: %(default)s]')
    parser.add_argument('-D', '--database', type=str,
                        default="volk_results.db",
                        help='Database file to store data in [default: %(default)s]')
    parser.add_argument('-N', '--nitems', type=float,
                        default=1e9,
                        help='Number of items per iterations [default: %(default)s]')
    parser.add_argument('-I', '--iterations', type=int,
                        default=20,
                        help='Number of iterations [default: %(default)s]')
    parser.add_argument('--tests', type=int, nargs='*',
                        choices=xrange(len(avail_tests)),
                        help='A list of tests to run; can be a single test or a \
                              space-separated list.')
    parser.add_argument('--list', action='store_true',
                        help='List the available tests')
    parser.add_argument('--all', action='store_true',
                        help='Run all tests')
    args = parser.parse_args()

    if(args.list):
        print "Available Tests to Run:"
        print "\n".join(["\t{0}: {1}".format(i,f.__name__) for i,f in enumerate(avail_tests)])
        sys.exit(0)

    N = int(args.nitems)
    iters = args.iterations
    label = args.label

    conn = create_connection(args.database)
    new_table(conn, label)

    if args.all:
        tests = xrange(len(avail_tests))
    else:
        tests = args.tests

    for test in tests:
        res = run_tests(avail_tests[test], N, iters)
        if res is not None:
            replace_results(conn, label, N, iters, res)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        pass
