#
# Copyright 2007,2014 Free Software Foundation, Inc.
# Copyright 2023 Marcus Müller
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
#


from .gr_python import (top_block_pb,
                        top_block_wait_unlocked, top_block_run_unlocked,
                        top_block_start_unlocked, top_block_stop_unlocked,
                        top_block_unlock_unlocked, logger)  # , dot_graph_tb)

from .hier_block2 import hier_block2
import threading


class _top_block_waiter(threading.Thread):
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
    interruptible, and if it sees a KeyboardInterrupt, executes a stop
    on the top_block, then goes back to waiting for it to complete.
    This ensures that the unlocked wait that was in progress (in the
    _top_block_waiter thread) can complete, release its mutex and back
    out.  If we don't do that, we are never able to clean up, and nasty
    things occur like leaving the USRP transmitter sending a carrier.

    See also top_block.wait (below), which uses this class to implement
    the interruptible wait.
    """

    def __init__(self, tb):
        threading.Thread.__init__(self)
        self.daemon = True
        self.tb = tb
        self.event = threading.Event()
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

    def __init__(self, name: str = "top_block", catch_exceptions: bool = True):
        """
        Create a top block with a given name.

        Wrap the methods of the underlying C++ `top_block_pb` in an impl object, and add the methods of that to this
        object.

        Add a python-side logger, to allow Python hierarchical blocks to do their own identifiable logging.
        """
        hier_block2.__init__(self, name, None, None, top_block_pb(name, catch_exceptions))
        self.logger = logger(f"Python Top Blk {name}")
        self.handle_sigint = True

    def __repr__(self):
        """
        Return a representation of the block useful for debugging
        """
        return f"<python top block {self.name()} wrapping GNU Radio top_block_pb object {id(self._impl):x}>"

    def __str__(self):
        """
        Return a string representation useful for human-aimed printing
        """
        return f"Python top block {self.name()}"

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

    # def dot_graph(self):
    #     """
    #     Return graph representation in dot language
    #     """
    #     return dot_graph_tb(self._impl)
