#!/usr/bin/env python

from gnuradio import gr
from gnuradio import blocks
import math, sys, os, time

try:
    import scipy
except ImportError:
    sys.stderr.write("Unable to import Scipy (www.scipy.org)\n")
    sys.exit(1)

try:
    import sqlite3
except ImportError:
    sys.stderr.write("Unable to import sqlite3: requires Python 2.5\n")
    sys.exit(1)

def execute(conn, cmd):
    '''
    Executes the command cmd to the database opened in connection conn.
    '''
    c = conn.cursor()
    c.execute(cmd)
    conn.commit()
    c.close()

def create_connection(database):
    '''
    Returns a connection object to the SQLite database.
    '''
    return sqlite3.connect(database)

def new_table(conn, tablename):
    '''
    Create a new table for results.
    All results are in the form: [kernel | nitems | iters | avg. time | variance | max time | min time ]
    Each table is meant as a different setting (e.g., volk_aligned, volk_unaligned, etc.)
    '''
    cols = "kernel text, nitems int, iters int, avg real, var real, max real, min real"
    cmd = "create table if not exists {0} ({1})".format(
        tablename, cols)
    execute(conn, cmd)

def replace_results(conn, tablename, nitems, iters, res):
    '''
    Inserts or replaces the results 'res' dictionary values into the table.
    This deletes all old entries of the kernel in this table.
    '''
    cmd = "DELETE FROM {0} where kernel='{1}'".format(tablename, res["kernel"])
    execute(conn, cmd)
    insert_results(conn, tablename, nitems, iters, res)

def insert_results(conn, tablename, nitems, iters, res):
    '''
    Inserts the results dictionary values into the table.
    '''
    cols = "kernel, nitems, iters, avg, var, max, min"
    cmd = "INSERT INTO {0} ({1}) VALUES ('{2}', {3}, {4}, {5}, {6}, {7}, {8})".format(
        tablename, cols, res["kernel"], nitems, iters,
        res["avg"], res["var"], res["max"], res["min"])
    execute(conn, cmd)

def list_tables(conn):
    '''
    Returns a list of all tables in the database.
    '''
    cmd = "SELECT name FROM sqlite_master WHERE type='table' ORDER BY name"
    c = conn.cursor()
    c.execute(cmd)
    t = c.fetchall()
    c.close()

    return t

def get_results(conn, tablename):
    '''
    Gets all results in tablename.
    '''
    cmd = "SELECT * FROM {0}".format(tablename)
    c = conn.cursor()
    c.execute(cmd)
    fetched = c.fetchall()
    c.close()

    res = list()
    for f in fetched:
        r = dict()
        r['kernel'] = f[0]
        r['nitems'] = f[1]
        r['iters']  = f[2]
        r['avg'] = f[3]
        r['var'] = f[4]
        r['min'] = f[5]
        r['max'] = f[6]
        res.append(r)

    return res


class helper(gr.top_block):
    '''
    Helper function to run the tests. The parameters are:
      N: number of items to process (int)
      op: The GR block/hier_block to test
      isizeof: the sizeof the input type
      osizeof: the sizeof the output type
      nsrcs: number of inputs to the op
      nsnks: number of outputs of the op

    This function can only handle blocks where all inputs are the same
    datatype and all outputs are the same data type
    '''
    def __init__(self, N, op,
                 isizeof=gr.sizeof_gr_complex,
                 osizeof=gr.sizeof_gr_complex,
                 nsrcs=1, nsnks=1):
        gr.top_block.__init__(self, "helper")

        self.op = op
        self.srcs = []
        self.snks = []
        self.head = blocks.head(isizeof, N)

        for n in xrange(nsrcs):
            self.srcs.append(blocks.null_source(isizeof))

        for n in xrange(nsnks):
            self.snks.append(blocks.null_sink(osizeof))

        self.connect(self.srcs[0], self.head, (self.op,0))

        for n in xrange(1, nsrcs):
            self.connect(self.srcs[n], (self.op,n))

        for n in xrange(nsnks):
            self.connect((self.op,n), self.snks[n])

def timeit(tb, iterations):
    '''
    Given a top block, this function times it for a number of
    iterations and stores the time in a list that is returned.
    '''
    r = gr.enable_realtime_scheduling()
    if r != gr.RT_OK:
        print "Warning: failed to enable realtime scheduling"

    times = []
    for i in xrange(iterations):
        start_time = time.time()
        tb.run()
        end_time = time.time()
        tb.head.reset()

        times.append(end_time - start_time)

    return times

def format_results(kernel, times):
    '''
    Convenience function to convert the results of the timeit function
    into a dictionary.
    '''
    res = dict()
    res["kernel"] = kernel
    res["avg"] = scipy.mean(times)
    res["var"] = scipy.var(times)
    res["max"] = max(times)
    res["min"] = min(times)
    return res


