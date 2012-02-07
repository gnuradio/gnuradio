#!/usr/bin/env python

import sys
import argparse
from volk_test_funcs import *

try:
    import matplotlib
    import matplotlib.pyplot as plt
except ImportError:
    sys.stderr.write("Could not import Matplotlib (http://matplotlib.sourceforge.net/)\n")
    sys.exit(1)

def main():
    desc='Plot Volk performance results from a SQLite database. ' + \
        'Run one of the volk tests first (e.g, volk_math.py)'
    parser = argparse.ArgumentParser(description=desc)
    parser.add_argument('-D', '--database', type=str,
                        default="volk_results.db",
                        help='Database file to read data from [default: %(default)s]')
    args = parser.parse_args()
    
    # Set up global plotting properties
    matplotlib.rcParams['figure.subplot.bottom'] = 0.2
    matplotlib.rcParams['figure.subplot.top'] = 0.95
    matplotlib.rcParams['ytick.labelsize'] = 16
    matplotlib.rcParams['xtick.labelsize'] = 16
    matplotlib.rcParams['legend.fontsize'] = 18
    
    # Get list of tables to compare
    conn = create_connection(args.database)
    tables = list_tables(conn)
    M = len(tables)

    # width of bars depends on number of comparisons
    wdth = 0.80/M

    colors = ['b', 'r', 'g', 'm', 'k']

    # Set up figure for plotting
    f0 = plt.figure(0, facecolor='w', figsize=(14,10))
    s0 = f0.add_subplot(1,1,1)

    for i,table in enumerate(tables):
        # Get results from the next table
        res = get_results(conn, table[0])
    
        xlabels = []
        averages = []
        variances = []
        maxes = []
        mins = []
        for r in res:
            xlabels.append(r['kernel'])
            averages.append(r['avg'])
            variances.append(r['var'])
            maxes.append(r['max'])
            mins.append(r['min'])

        # makes x values for this data set placement
        x0 = xrange(len(res))
        x1 = [x + i*wdth for x in x0]

        s0.bar(x1, averages, width=wdth,
               #yerr=variances,
               color=colors[i%M], label=table[0],
               edgecolor='k', linewidth=2)

    s0.legend()
    s0.set_ylabel("Processing time (sec) [{0:G} items]".format(res[0]['nitems']),
                  fontsize=22, fontweight='bold')
    s0.set_xticks(x0)
    s0.set_xticklabels(xlabels)
    for label in s0.xaxis.get_ticklabels():
        label.set_rotation(45)
        label.set_fontsize(16)

    plt.show()

if __name__ == "__main__":
    main()
