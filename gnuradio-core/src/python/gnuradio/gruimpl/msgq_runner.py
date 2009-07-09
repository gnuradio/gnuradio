#
# Copyright 2009 Free Software Foundation, Inc.
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

"""
Convenience class for dequeuing messages from a gr.msg_queue and
invoking a callback.

Creates a Python thread that does a blocking read on the supplied
gr.msg_queue, then invokes callback each time a msg is received.

If the msg type is not 0, then it is treated as a signal to exit
its loop.

If the callback raises an exception, and the runner was created
with 'exit_on_error' equal to True, then the runner will store the
exception and exit its loop, otherwise the exception is ignored.

To get the exception that the callback raised, if any, call
exit_error() on the object.

To manually stop the runner, call stop() on the object.

To determine if the runner has exited, call exited() on the object.
"""

from gnuradio import gr
import gnuradio.gr.gr_threading as _threading

class msgq_runner(_threading.Thread):

    def __init__(self, msgq, callback, exit_on_error=False):
        _threading.Thread.__init__(self)

        self._msgq = msgq
        self._callback = callback
        self._exit_on_error = exit_on_error
        self._done = False
        self._exited = False
        self._exit_error = None
        self.setDaemon(1)
        self.start()

    def run(self):
        while not self._done:
            msg = self._msgq.delete_head()
            if msg.type() != 0:
                self.stop()
            else:
                try:
                    self._callback(msg)
                except Exception, e:
                    if self._exit_on_error:
                        self._exit_error = e
                        self.stop()
        self._exited = True

    def stop(self):
        self._done = True

    def exited(self):
        return self._exited

    def exit_error(self):
        return self._exit_error
