#!/usr/bin/env python

import sys, math
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
                        default='volk_results.db',
                        help='Database file to read data from [default: %(default)s]')
    parser.add_argument('-E', '--errorbars',
                        action='store_true', default=False,
                        help='Show error bars (1 standard dev.)')
    parser.add_argument('-P', '--plot', type=str,
                        choices=['mean', 'min', 'max'],
                        default='mean',
                        help='Set the type of plot to produce [default: %(default)s]')
    parser.add_argument('-%', '--percent', type=str,
                        default=None, metavar="table",
                        help='Show percent difference to the given type [default: %(default)s]')
    args = parser.parse_args()

    # Set up global plotting properties
    matplotlib.rcParams['figure.subplot.bottom'] = 0.2
    matplotlib.rcParams['figure.subplot.top'] = 0.95
    matplotlib.rcParams['figure.subplot.right'] = 0.98
    matplotlib.rcParams['ytick.labelsize'] = 16
    matplotlib.rcParams['xtick.labelsize'] = 16
    matplotlib.rcParams['legend.fontsize'] = 18

    # Get list of tables to compare
    conn = create_connection(args.database)
    tables = list_tables(conn)
    M = len(tables)

    # Colors to distinguish each table in the bar graph
    # More than 5 tables will wrap around to the start.
    colors = ['b', 'r', 'g', 'm', 'k']

    # Set up figure for plotting
    f0 = plt.figure(0, facecolor='w', figsize=(14,10))
    s0 = f0.add_subplot(1,1,1)

    # Create a register of names that exist in all tables
    tmp_regs = []
    for table in tables:
        # Get results from the next table
        res = get_results(conn, table[0])

        tmp_regs.append(list())
        for r in res:
            try:
                tmp_regs[-1].index(r['kernel'])
            except ValueError:
                tmp_regs[-1].append(r['kernel'])

    # Get only those names that are common in all tables
    name_reg = tmp_regs[0]
    for t in tmp_regs[1:]:
        name_reg = list(set(name_reg) & set(t))
    name_reg.sort()

    # Pull the data out for each table into a dictionary
    # we can ref the table by it's name and the data associated
    # with a given kernel in name_reg by it's name.
    # This ensures there is no sorting issue with the data in the
    # dictionary, so the kernels are plotted against each other.
    table_data = dict()
    for i,table in enumerate(tables):
        # Get results from the next table
        res = get_results(conn, table[0])

        data = dict()
        for r in res:
            data[r['kernel']] = r

        table_data[table[0]] = data

    if args.percent is not None:
        for i,t in enumerate(table_data):
            if args.percent == t:
                norm_data = []
                for name in name_reg:
                    if(args.plot == 'max'):
                        norm_data.append(table_data[t][name]['max'])
                    elif(args.plot == 'min'):
                        norm_data.append(table_data[t][name]['min'])
                    elif(args.plot == 'mean'):
                        norm_data.append(table_data[t][name]['avg'])


    # Plot the results
    x0 = xrange(len(name_reg))
    i = 0
    for t in (table_data):
        ydata = []
        stds = []
        for name in name_reg:
            stds.append(math.sqrt(table_data[t][name]['var']))
            if(args.plot == 'max'):
                ydata.append(table_data[t][name]['max'])
            elif(args.plot == 'min'):
                ydata.append(table_data[t][name]['min'])
            elif(args.plot == 'mean'):
                ydata.append(table_data[t][name]['avg'])

        if args.percent is not None:
            ydata = [-100*(y-n)/y for y,n in zip(ydata,norm_data)]
            if(args.percent != t):
                # makes x values for this data set placement
                # width of bars depends on number of comparisons
                wdth = 0.80/(M-1)
                x1 = [x + i*wdth for x in x0]
                i += 1

                s0.bar(x1, ydata, width=wdth,
                       color=colors[(i-1)%M], label=t,
                       edgecolor='k', linewidth=2)

        else:
            # makes x values for this data set placement
            # width of bars depends on number of comparisons
            wdth = 0.80/M
            x1 = [x + i*wdth for x in x0]
            i += 1

            if(args.errorbars is False):
                s0.bar(x1, ydata, width=wdth,
                       color=colors[(i-1)%M], label=t,
                       edgecolor='k', linewidth=2)
            else:
                s0.bar(x1, ydata, width=wdth,
                       yerr=stds,
                       color=colors[i%M], label=t,
                       edgecolor='k', linewidth=2,
                       error_kw={"ecolor": 'k', "capsize":5,
                                 "linewidth":2})

    nitems = res[0]['nitems']
    if args.percent is None:
        s0.set_ylabel("Processing time (sec) [{0:G} items]".format(nitems),
                      fontsize=22, fontweight='bold',
                      horizontalalignment='center')
    else:
        s0.set_ylabel("% Improvement over {0} [{1:G} items]".format(
                args.percent, nitems),
                      fontsize=22, fontweight='bold')

    s0.legend()
    s0.set_xticks(x0)
    s0.set_xticklabels(name_reg)
    for label in s0.xaxis.get_ticklabels():
        label.set_rotation(45)
        label.set_fontsize(16)

    plt.show()

if __name__ == "__main__":
    main()
