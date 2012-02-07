#
# Copyright 2007 Free Software Foundation, Inc.
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

from gnuradio_core import top_block_swig, \
    top_block_wait_unlocked, top_block_run_unlocked

#import gnuradio.gr.gr_threading as _threading
import gr_threading as _threading


#
# There is no problem that can't be solved with an additional
# level of indirection...
#
# This kludge allows ^C to interrupt top_block.run and top_block.wait
#
# The problem that we are working around is that Python only services
# signals (e.g., KeyboardInterrupt) in its main thread.  If the main
# thread is blocked in our C++ version of wait, even though Python's
# SIGINT handler fires, and even though there may be other python
# threads running, no one will know.  Thus instead of directly waiting
# in the thread that calls wait (which is likely to be the Python main
# thread), we create a separate thread that does the blocking wait,
# and then use the thread that called wait to do a slow poll of an
# event queue.  That thread, which is executing "wait" below is
# interruptable, and if it sees a KeyboardInterrupt, executes a stop
# on the top_block, then goes back to waiting for it to complete.
# This ensures that the unlocked wait that was in progress (in the
# _top_block_waiter thread) can complete, release its mutex and back
# out.  If we don't do that, we are never able to clean up, and nasty
# things occur like leaving the USRP transmitter sending a carrier.
#
# See also top_block.wait (below), which uses this class to implement
# the interruptable wait.
#
class _top_block_waiter(_threading.Thread):
    def __init__(self, tb):
        _threading.Thread.__init__(self)
        self.setDaemon(1)
        self.tb = tb
        self.event = _threading.Event()
        self.start()

    def run(self):
        top_block_wait_unlocked(self.tb)
        self.event.set()

    def wait(self):
        try:
            while not self.event.isSet():
                self.event.wait(0.100)
        except KeyboardInterrupt:
            self.tb.stop()
            self.wait()


#
# This hack forces a 'has-a' relationship to look like an 'is-a' one.
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
class top_block(object):
    def __init__(self, name="top_block"):
	self._tb = top_block_swig(name)

    def __getattr__(self, name):
        if not hasattr(self, "_tb"):
            raise RuntimeError("top_block: invalid state--did you forget to call gr.top_block.__init__ in a derived class?")
	return getattr(self._tb, name)

    def start(self, max_noutput_items=100000):
    	self._tb.start(max_noutput_items)
	
    def stop(self):
    	self._tb.stop()

    def run(self, max_noutput_items=100000):
        self.start(max_noutput_items)
        self.wait()

    def wait(self):
        _top_block_waiter(self._tb).wait()


    # FIXME: these are duplicated from hier_block2.py; they should really be implemented
    # in the original C++ class (gr_hier_block2), then they would all be inherited here

    def connect(self, *points):
        '''connect requires one or more arguments that can be coerced to endpoints.
        If more than two arguments are provided, they are connected together successively.
        '''
        if len (points) < 1:
            raise ValueError, ("connect requires at least one endpoint; %d provided." % (len (points),))
	else:
	    if len(points) == 1:
		self._tb.primitive_connect(points[0].to_basic_block())
	    else:
		for i in range (1, len (points)):
        	    self._connect(points[i-1], points[i])

    def _connect(self, src, dst):
        (src_block, src_port) = self._coerce_endpoint(src)
        (dst_block, dst_port) = self._coerce_endpoint(dst)
        self._tb.primitive_connect(src_block.to_basic_block(), src_port,
                                   dst_block.to_basic_block(), dst_port)

    def _coerce_endpoint(self, endp):
        if hasattr(endp, 'to_basic_block'):
            return (endp, 0)
        else:
            if hasattr(endp, "__getitem__") and len(endp) == 2:
                return endp # Assume user put (block, port)
            else:
                raise ValueError("unable to coerce endpoint")

    def disconnect(self, *points):
        '''disconnect requires one or more arguments that can be coerced to endpoints.
        If more than two arguments are provided, they are disconnected successively.
        '''
        if len (points) < 1:
            raise ValueError, ("disconnect requires at least one endpoint; %d provided." % (len (points),))
        else:
            if len(points) == 1:
                self._tb.primitive_disconnect(points[0].to_basic_block())
            else:
                for i in range (1, len (points)):
                    self._disconnect(points[i-1], points[i])

    def _disconnect(self, src, dst):
        (src_block, src_port) = self._coerce_endpoint(src)
        (dst_block, dst_port) = self._coerce_endpoint(dst)
        self._tb.primitive_disconnect(src_block.to_basic_block(), src_port,
                                      dst_block.to_basic_block(), dst_port)

