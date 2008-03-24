#!/usr/bin/env python
#
# Copyright 2007,2008 Free Software Foundation, Inc.
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

"""
input file looks like this:

nspes:  1  udelay:   10  elapsed_time:   6.842  njobs: 500000  speedup:  0.731
nspes:  2  udelay:   10  elapsed_time:   4.093  njobs: 500000  speedup:  1.221
"""

import sys
from optparse import OptionParser
from pprint import pprint

class data(object):
    def __init__(self, nspes, work_per_job, elapsed_time, njobs):
        self.nspes = nspes
        self.work_per_job = work_per_job  # seconds
        self.elapsed_time = elapsed_time  # seconds
        self.njobs = njobs
        self.speedup = work_per_job * njobs / elapsed_time

    def __repr__(self):
        return "<data nspes=%d work_per_job=%s elapsed_time=%s njobs=%s speedup=%s>" % (
            self.nspes, (self.work_per_job),
            (self.elapsed_time),
            (self.njobs),
            (self.speedup))

def cmp_data(x, y):
    t = x.nspes - y.nspes
    if t == 0:
        t = x.work_per_job - y.work_per_job
        if t < 0:
            return -1
        elif t > 0:
            return +1
        else:
            return 0
    return t

def main():
    usage = "usage: %prog [options] input_filename"
    parser = OptionParser(usage=usage)
    (options, args) = parser.parse_args()
    if len(args) != 1:
        parser.print_help()
        raise SystemExit, 1
    input_filename = args[0]

    
    m = {}
    for line in open(input_filename, "r"):
        s = line.split()
        nspes = int(s[1])
        work_per_job = int(s[3]) * 1e-6
        elapsed_time = float(s[5])
        njobs = float(s[7])
        d = data(nspes, work_per_job, elapsed_time, njobs)

        # collect lists that have the same values for nspes and work_per_job
        # so we can generate an average elapsed_time from all observations
        key = (nspes, work_per_job)
        v = m.get(key, [])
        v.append(d)
        m[key] = v

    r = []
    for k, v in m.iteritems():
        total_elapsed_time = sum([x.elapsed_time for x in v])
        r.append(data(v[0].nspes,
                      v[0].work_per_job,
                      total_elapsed_time/len(v),
                      v[0].njobs))

    r.sort(cmp_data)

    #pprint(r)
    for t in r:
        print t.nspes, t.work_per_job, t.elapsed_time, t.njobs, t.speedup

if __name__ == '__main__':
    main()
