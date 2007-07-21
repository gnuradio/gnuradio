#
# Copyright 2004 Free Software Foundation, Inc.
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

from gnuradio.gr.exceptions import *
from gnuradio_swig_python import single_threaded_scheduler, sts_pyrun
import gr_threading as _threading
#import threading as _threading

class scheduler_thread(_threading.Thread):
    def __init__(self, sts):
        _threading.Thread.__init__(self)
        self.sts = sts
    def run(self):
        # Invoke the single threaded scheduler's run method
        #
        # Note that we're in a new thread, and that sts_pyrun
        # releases the global interpreter lock.  This has the
        # effect of evaluating the graph in parallel to the
        # main line control code.
        sts_pyrun(self.sts)  
        self.sts = None
        
class scheduler(object):
    def __init__(self, fg):
        graphs = fg.partition_graph(fg.blocks)
        # print "@@@ # graphs = %d" % (len(graphs))
        
        self.state = []
        
        for g in graphs:
            list_of_blocks = [x.block() for x in g]
            sts = single_threaded_scheduler(list_of_blocks)
            thread = scheduler_thread(sts)
            thread.setDaemon(1)
            self.state.append((sts, thread))

    def start(self):
        for (sts, thread) in self.state:
            thread.start()

    def stop(self):
        for (sts, thread) in self.state:
            sts.stop()
        self.wait()

    def wait(self):
        for (sts, thread) in self.state:
            timeout = 0.100
            while True:
                thread.join(timeout)
                if not thread.isAlive():
                    break
