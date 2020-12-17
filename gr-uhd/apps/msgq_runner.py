#
# Copyright 2009 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
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
import threading

class msgq_runner(threading.Thread):

    def __init__(self, msgq, callback, exit_on_error=False):
        threading.Thread.__init__(self)

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
                except Exception as e:
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
