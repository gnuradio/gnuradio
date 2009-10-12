#!/usr/bin/env python

import sys
import time
import os

from optparse import OptionParser


def get_svn_rev():
    try:
        f = os.popen("svn info", "r")
        lines = f.readlines()
        f.close()
    except:
        return "unk"

    svn_rev = "unk"
    for l in lines:
        if l.startswith('Revision:'):
            t = l.rstrip()
            svn_rev = t.split()[-1]
    return svn_rev
    
def is_ps3():
    try:
        f = open("/proc/cpuinfo")
        s = f.read()
    except:
        return False

    return s.find('PS3') != -1


def main():

    def make_fname(suffix):
        return basename + '.' + suffix

    max_spes_default = 16
    if is_ps3():
        max_spes_default = 6
        
    parser = OptionParser()
    parser.add_option("-m", "--max-spes", type="int", default=max_spes_default,
                      metavar="NSPES",
                      help="maximum number of SPEs to use [default=%default]")
    parser.add_option("", "--min-spes", type="int", default=1,
                      metavar="NSPES",
                      help="minimum number of SPEs to use [default=%default]")
    parser.add_option("-p", "--oprofile", action="store_true", default=False,
                      help="emit oprofile commands")
    parser.add_option("-t", "--tag", default=None,
                      help="additional goodie in generated filenames")
    (options, args) = parser.parse_args()
    if len(args) != 0:
        parser.print_help()
        sys.exit(1)

    #FIXME to use git now
    svn_rev = get_svn_rev()

    os.environ['TZ'] = 'PST8PDT'        # always pacific
    time.tzset()

    tag = ''
    if options.tag:
        tag = '-' + options.tag
        
    basename = 'R-%s%s-%s' % (svn_rev, tag, time.strftime('%Y%m%d-%H%M'))

    base_njobs = int(500e3)
    njobs = {
         1 : base_njobs,
        10 : base_njobs,
        50 : base_njobs,
       100 : base_njobs, 
       200 : int(base_njobs/2),
       250 : int(base_njobs/2.5),
       300 : int(base_njobs/3),
       400 : int(base_njobs/4),
       500 : int(base_njobs/5)
        }

    
    f_results = make_fname('results')
    f_opreport = make_fname('opreport')
    f_avg = make_fname('avg')
    f_png = make_fname('png')

    f = sys.stdout
    f.write("#!/bin/bash\n")

    if options.oprofile:
        f.write("opcontrol --stop\n")
        f.write("opcontrol --reset\n")
        f.write("opcontrol --start\n")

    f.write("(\n")

    for udelay in (10, 50, 100, 200, 300):
        for nspes in range(options.min_spes, options.max_spes+1):
            cmd = "./benchmark_nop -n %d -u %d -N %d\n" % (nspes, udelay, njobs[udelay])
            f.write(cmd)
            f.write(cmd)

    f.write(") | tee %s\n" % (f_results,))

    if options.oprofile:
        f.write("opcontrol --dump\n")
        f.write("opcontrol --stop\n")
        f.write("opreport -l | head -100 > %s\n" % (f_opreport,))

    f.write("./split_and_avg_results.py %s > %s\n" % (f_results, f_avg))
    f.write("./plot_speedup.py %s -o %s\n" % (f_avg, f_png))


if __name__ == '__main__':
    main()
