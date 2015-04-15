#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
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
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
#

import sys, subprocess, re, signal, time, atexit, os
from gnuradio import gr

class monitor:
    def __init__(self,tool="gr-ctrlport-monitor"):
        print "ControlPort Monitor running."
        self.started = False
        self.tool = tool
        atexit.register(self.shutdown)

        try:
            # setup export prefs
            gr.prefs().singleton().set_bool("ControlPort","on",True);
            if(tool == "gr-perf-monitorx"):
                gr.prefs().singleton().set_bool("ControlPort","edges_list",True);
                gr.prefs().singleton().set_bool("PerfCounters","on",True);
                gr.prefs().singleton().set_bool("PerfCounters","export",True);
        except:
            print "no support for gr.prefs setting"

    def __del__(self):
        if(self.started):
            self.stop()

    def start(self):
        print "monitor::endpoints() = %s" % (gr.rpcmanager_get().endpoints())
        try:
            cmd = map(lambda a: [self.tool,
                                 re.search("-h (\S+|\d+\.\d+\.\d+\.\d+)",a).group(1),
                                 re.search("-p (\d+)",a).group(1)],
                      gr.rpcmanager_get().endpoints())[0]
            print "running: %s"%(str(cmd))
            self.proc = subprocess.Popen(cmd);
            self.started = True
        except:
            self.proc = None
            print "failed to to start ControlPort Monitor on specified port"

    def stop(self):
        if(self.proc):
            if(self.proc.returncode == None):
                print "\tcalling stop on shutdown"
                self.proc.terminate()
        else:
            print "\tno proc to shut down, exiting"

    def shutdown(self):
        print "ctrlport.monitor received shutdown signal"
        if(self.started):
            self.stop()
