#
# Copyright 2007,2014 Free Software Foundation, Inc.
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

from runtime_swig import top_block_swig, \
    top_block_wait_unlocked, top_block_run_unlocked, \
    top_block_start_unlocked, top_block_stop_unlocked, \
    top_block_unlock_unlocked, dot_graph_tb

#import gnuradio.gr.gr_threading as _threading
import gr_threading as _threading

from hier_block2 import hier_block2

class _top_block_waiter(_threading.Thread):
    """
    This kludge allows ^C to interrupt top_block.run and top_block.wait

    The problem that we are working around is that Python only services
    signals (e.g., KeyboardInterrupt) in its main thread.  If the main
    thread is blocked in our C++ version of wait, even though Python's
    SIGINT handler fires, and even though there may be other python
    threads running, no one will know.  Thus instead of directly waiting
    in the thread that calls wait (which is likely to be the Python main
    thread), we create a separate thread that does the blocking wait,
    and then use the thread that called wait to do a slow poll of an
    event queue.  That thread, which is executing "wait" below is
    interruptable, and if it sees a KeyboardInterrupt, executes a stop
    on the top_block, then goes back to waiting for it to complete.
    This ensures that the unlocked wait that was in progress (in the
    _top_block_waiter thread) can complete, release its mutex and back
    out.  If we don't do that, we are never able to clean up, and nasty
    things occur like leaving the USRP transmitter sending a carrier.

    See also top_block.wait (below), which uses this class to implement
    the interruptable wait.
    """
    def __init__(self, tb):
        _threading.Thread.__init__(self)
        self.setDaemon(1)
        self.tb = tb
        self.event = _threading.Event()
        self.start()

    def run(self):
        top_block_wait_unlocked(self.tb)
        self.event.set()

    def wait(self, handle_sigint=True):
        try:
            while not self.event.wait(0.1):
                pass
        except KeyboardInterrupt:
            if not handle_sigint:
                raise
            self.tb.stop()
            self.wait()


#
# This makes a 'has-a' relationship to look like an 'is-a' one.
#
# It allows Python classes to subclass this one, while passing through
# method calls to the C++ class shared pointer from SWIG.
#
# It also allows us to intercept method calls if needed.
#
# This allows the 'run_locked' methods, which are defined in gr_top_block.i,
# to release the Python global interpreter lock before calling the actual
# method in gr_top_block
#
class top_block(hier_block2):
    """
    Top-level hierarchical block representing a flow-graph.

    This is a python wrapper around the C++ implementation to allow
    python subclassing.
    """

    def __init__(self, name="top_block"):
        """
        Create a top block with a given name.
        """
        # not calling hier_block2.__init__, we set our own _impl
        self._impl = top_block_swig(name)
        self.handle_sigint = True

    def start(self, max_noutput_items=10000000):
        """
        Start the flowgraph with the given number of output items and return.
        """
        top_block_start_unlocked(self._impl, max_noutput_items)

    def stop(self):
        """
        Stop the flowgraph
        """
        top_block_stop_unlocked(self._impl)

    def run(self, max_noutput_items=10000000):
        """
        Start the flowgraph with the given number of output items and wait.
        """
        self.start(max_noutput_items)
        self.wait()

    def unlock(self):
        """
        Release lock and continue execution of flow-graph.
        """
        top_block_unlock_unlocked(self._impl)

    def wait(self):
        """
        Wait for the flowgraph to finish running
        """
        _top_block_waiter(self._impl).wait(self.handle_sigint)

    def dot_graph(self):
        """
        Return graph representation in dot language
        """
        return dot_graph_tb(self._impl)
