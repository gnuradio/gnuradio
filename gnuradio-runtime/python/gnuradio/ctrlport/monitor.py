#!/usr/bin/env python
#
# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


import sys
import subprocess
import re
import signal
import time
import atexit
import os
from gnuradio import gr


class monitor(object):
    def __init__(self, tool="gr-ctrlport-monitor"):
        print("ControlPort Monitor starting.")
        self.started = False
        self.tool = tool
        atexit.register(self.shutdown)

        # setup export prefs
        gr.prefs().set_bool("ControlPort", "on", True)
        gr.prefs().set_bool("PerfCounters", "on", True)
        gr.prefs().set_bool("PerfCounters", "export", True)
        if tool == "gr-perf-monitorx":
            gr.prefs().set_bool("ControlPort", "edges_list", True)

    def start(self):
        try:
            self.elem = gr.rpcmanager.get()
            ep = self.elem.endpoints()
            print(f"monitor::endpoints() =  {ep}")

            cmd = [
                self.tool,
                re.search(r"-h (\S+|\d+\.\d+\.\d+\.\d+)", ep[0]).group(1),
                re.search(r"-p (\d+)", ep[0]).group(1),
            ]
            print("running: %s" % (str(cmd)))
            self.proc = subprocess.Popen(cmd)
            self.started = True
        except:
            self.proc = None
            print(
                "Failed to to connect to ControlPort. Please make sure that you have Thrift installed and check your firewall rules."
            )

    def stop(self):
        if self.proc:
            if self.proc.returncode == None:
                print("\tcalling stop on shutdown")
                self.proc.terminate()
        else:
            print("\tno proc to shut down, exiting")

    def shutdown(self):
        print("ctrlport monitor received shutdown signal")
        if self.started:
            self.stop()
