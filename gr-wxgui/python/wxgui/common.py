#
# Copyright 2008, 2009 Free Software Foundation, Inc.
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

##################################################
# conditional disconnections of wx flow graph
##################################################
import wx
from gnuradio import gr
from gnuradio import blocks

RUN_ALWAYS = gr.prefs().get_bool('wxgui', 'run_always', False)


class wxgui_hb(object):
    """
    The wxgui hier block helper/wrapper class:
    A hier block should inherit from this class to make use of the wxgui connect method.
    To use, call wxgui_connect in place of regular connect; self.win must be defined.
    The implementation will conditionally enable the copy block after the source (self).
    This condition depends on weather or not the window is visible with the parent notebooks.
    This condition will be re-checked on every ui update event.
    """

    def wxgui_connect(self, *points):
        """
        Use wxgui connect when the first point is the self source of the hb.
        The win property of this object should be set to the wx window.
        When this method tries to connect self to the next point,
        it will conditionally make this connection based on the visibility state.
        All other points will be connected normally.
        """
        try:
            assert points[0] == self or points[0][0] == self
            copy = blocks.copy(self.input_signature().sizeof_stream_item(0))
            handler = self._handler_factory(copy.set_enabled)
            if not RUN_ALWAYS:
                handler(False)  # initially disable the copy block
            else:
                handler(True)  # initially enable the copy block
            self._bind_to_visible_event(win=self.win, handler=handler)
            points = list(points)
            points.insert(1, copy) # insert the copy block into the chain
        except (AssertionError, IndexError): pass
        self.connect(*points)  # actually connect the blocks

    @staticmethod
    def _handler_factory(handler):
        """
        Create a function that will cache the visibility flag,
        and only call the handler when that flag changes.

        Args:
            handler: the function to call on a change

        Returns:
            a function of 1 argument
        """
        cache = [None]

        def callback(visible):
            if cache[0] == visible: return
            cache[0] = visible
            # print visible, handler
            if not RUN_ALWAYS:
                handler(visible)
            else:
                handler(True)
        return callback

    @staticmethod
    def _bind_to_visible_event(win, handler):
        """
        Bind a handler to a window when its visibility changes.
        Specifically, call the handler when the window visibility changes.
        This condition is checked on every update ui event.

        Args:
            win: the wx window
            handler: a function of 1 param
        """
        # is the window visible in the hierarchy
        def is_wx_window_visible(my_win):
            while True:
                parent = my_win.GetParent()
                if not parent: return True  # reached the top of the hierarchy
                # if we are hidden, then finish, otherwise keep traversing up
                if isinstance(parent, wx.Notebook) and parent.GetCurrentPage() != my_win: return False
                my_win = parent
        # call the handler, the arg is shown or not
        def handler_factory(my_win, my_handler):
            def callback(evt):
                my_handler(is_wx_window_visible(my_win))
                evt.Skip()  # skip so all bound handlers are called
            return callback
        handler = handler_factory(win, handler)
        # bind the handler to all the parent notebooks
        win.Bind(wx.EVT_UPDATE_UI, handler)

##################################################
# Helpful Functions
##################################################

# A macro to apply an index to a key
index_key = lambda key, i: "%s_%d" % (key, i+1)


def _register_access_method(destination, controller, key):
    """
    Helper function for register access methods.
    This helper creates distinct set and get methods for each key
    and adds them to the destination object.
    """
    def set(value): controller[key] = value
    setattr(destination, 'set_'+key, set)
    def get(): return controller[key]
    setattr(destination, 'get_'+key, get)


def register_access_methods(destination, controller):
    """
    Register setter and getter functions in the destination object for all keys in the controller.

    Args:
        destination: the object to get new setter and getter methods
        controller: the pubsub controller
    """
    for key in controller.keys(): _register_access_method(destination, controller, key)

##################################################
# Input Watcher Thread
##################################################
from gnuradio import gru


class input_watcher(gru.msgq_runner):
    """
    Input watcher thread runs forever.
    Read messages from the message queue.
    Forward messages to the message handler.
    """
    def __init__ (self, msgq, controller, msg_key, arg1_key='', arg2_key=''):
        self._controller = controller
        self._msg_key = msg_key
        self._arg1_key = arg1_key
        self._arg2_key = arg2_key
        gru.msgq_runner.__init__(self, msgq, self.handle_msg)

    def handle_msg(self, msg):
        if self._arg1_key: self._controller[self._arg1_key] = msg.arg1()
        if self._arg2_key: self._controller[self._arg2_key] = msg.arg2()
        self._controller[self._msg_key] = msg.to_string()


##################################################
# Shared Functions
##################################################
import numpy
import math


def get_exp(num):
    """
    Get the exponent of the number in base 10.

    Args:
        num: the floating point number

    Returns:
        the exponent as an integer
    """
    if num == 0: return 0
    return int(math.floor(math.log10(abs(num))))


def get_clean_num(num):
    """
    Get the closest clean number match to num with bases 1, 2, 5.

    Args:
        num: the number

    Returns:
        the closest number
    """
    if num == 0: return 0
    sign = num > 0 and 1 or -1
    exp = get_exp(num)
    nums = numpy.array((1, 2, 5, 10))*(10**exp)
    return sign*nums[numpy.argmin(numpy.abs(nums - abs(num)))]


def get_clean_incr(num):
    """
    Get the next higher clean number with bases 1, 2, 5.

    Args:
        num: the number

    Returns:
        the next higher number
    """
    num = get_clean_num(num)
    exp = get_exp(num)
    coeff = int(round(num/10**exp))
    return {
        -5: -2,
        -2: -1,
        -1: -.5,
        1: 2,
        2: 5,
        5: 10,
    }[coeff]*(10**exp)


def get_clean_decr(num):
    """
    Get the next lower clean number with bases 1, 2, 5.

    Args:
        num: the number

    Returns:
        the next lower number
    """
    num = get_clean_num(num)
    exp = get_exp(num)
    coeff = int(round(num/10**exp))
    return {
        -5: -10,
        -2: -5,
        -1: -2,
        1: .5,
        2: 1,
        5: 2,
    }[coeff]*(10**exp)


def get_min_max(samples):
    """
    Get the minimum and maximum bounds for an array of samples.

    Args:
        samples: the array of real values

    Returns:
        a tuple of min, max
    """
    factor = 2.0
    mean = numpy.average(samples)
    std = numpy.std(samples)
    fft = numpy.abs(numpy.fft.fft(samples - mean))
    envelope = 2*numpy.max(fft)/len(samples)
    ampl = max(std, envelope) or 0.1
    return mean - factor*ampl, mean + factor*ampl


def get_min_max_fft(fft_samps):
    """
    Get the minimum and maximum bounds for an array of fft samples.

    Args:
        samples: the array of real values

    Returns:
        a tuple of min, max
    """
    # get the peak level (max of the samples)
    peak_level = numpy.max(fft_samps)
    # separate noise samples
    noise_samps = numpy.sort(fft_samps)[:len(fft_samps)/2]
    # get the noise floor
    noise_floor = numpy.average(noise_samps)
    # get the noise deviation
    noise_dev = numpy.std(noise_samps)
    # determine the maximum and minimum levels
    max_level = peak_level
    min_level = noise_floor - abs(2*noise_dev)
    return min_level, max_level
