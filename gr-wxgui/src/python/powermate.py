#!/usr/bin/env python
#
# Copyright 2005 Free Software Foundation, Inc.
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
Handler for Griffin PowerMate, Contour ShuttlePro & ShuttleXpress USB knobs

This is Linux and wxPython specific.
"""

import os
import sys
import struct
import exceptions
import threading
import wx
from gnuradio import gru

imported_ok = True

try:
    import select
    import fcntl
except ImportError:
    imported_ok = False


# First a little bit of background:
#
# The Griffin PowerMate has
#  * a single knob which rotates
#  * a single button (pressing the knob)
#
# The Contour ShuttleXpress (aka SpaceShuttle) has
#  * "Jog Wheel"  -- the knob (rotary encoder) on the inside
#  * "Shuttle Ring" -- the spring loaded rubber covered ring
#  * 5 buttons
#
# The Contour ShuttlePro has
#  * "Jog Wheel" -- the knob (rotary encoder) on the inside
#  * "Shuttle Ring" -- the spring loaded rubber covered ring
#  * 13 buttons
#
# The Contour ShuttlePro V2 has
#  *"Jog Wheel" -- the knob (rotary encoder) on the inside
#  * "Shuttle Ring" -- the spring loaded rubber covered ring
#  * 15 buttons

# We remap all the buttons on the devices so that they start at zero.

# For the ShuttleXpress the buttons are 0 to 4 (left to right)

# For the ShuttlePro, we number the buttons immediately above
# the ring 0 to 4 (left to right) so that they match our numbering
# on the ShuttleXpress.  The top row is 5, 6, 7, 8.  The first row below
# the ring is 9, 10, and the bottom row is 11, 12.

# For the ShuttlePro V2, buttons 13 & 14 are to the
# left and right of the wheel respectively.

# We generate 3 kinds of events:
#
#   button press/release (button_number, press/release)
#   knob rotation (relative_clicks)       # typically -1, +1
#   shuttle position (absolute_position)  # -7,-6,...,0,...,6,7

# ----------------------------------------------------------------
# Our ID's for the devices:
# Not to be confused with anything related to magic hardware numbers.

ID_POWERMATE         = 'powermate'
ID_SHUTTLE_XPRESS    = 'shuttle xpress'
ID_SHUTTLE_PRO       = 'shuttle pro'
ID_SHUTTLE_PRO_V2    = 'shuttle pro v2'

# ------------------------------------------------------------------------
# format of messages that we read from /dev/input/event*
# See /usr/include/linux/input.h for more info
#
#struct input_event {
#        struct timeval time; = {long seconds, long microseconds}
#        unsigned short type;
#        unsigned short code;
#        unsigned int value;
#};

input_event_struct = "@llHHi"
input_event_size = struct.calcsize(input_event_struct)

# ------------------------------------------------------------------------
# input_event types
# ------------------------------------------------------------------------

IET_SYN		  = 0x00   # aka RESET
IET_KEY		  = 0x01   # key or button press/release
IET_REL		  = 0x02   # relative movement (knob rotation)
IET_ABS		  = 0x03   # absolute position (graphics pad, etc)
IET_MSC		  = 0x04
IET_LED		  = 0x11
IET_SND		  = 0x12
IET_REP		  = 0x14
IET_FF		  = 0x15
IET_PWR		  = 0x16
IET_FF_STATUS	  = 0x17
IET_MAX		  = 0x1f

# ------------------------------------------------------------------------
# input_event codes (there are a zillion of them, we only define a few)
# ------------------------------------------------------------------------

# these are valid for IET_KEY

IEC_BTN_0	   = 0x100
IEC_BTN_1	   = 0x101
IEC_BTN_2	   = 0x102
IEC_BTN_3	   = 0x103
IEC_BTN_4	   = 0x104
IEC_BTN_5	   = 0x105
IEC_BTN_6	   = 0x106
IEC_BTN_7	   = 0x107
IEC_BTN_8	   = 0x108
IEC_BTN_9	   = 0x109
IEC_BTN_10	   = 0x10a
IEC_BTN_11	   = 0x10b
IEC_BTN_12	   = 0x10c
IEC_BTN_13	   = 0x10d
IEC_BTN_14	   = 0x10e
IEC_BTN_15	   = 0x10f

# these are valid for IET_REL (Relative axes)

IEC_REL_X	   = 0x00
IEC_REL_Y	   = 0x01
IEC_REL_Z	   = 0x02
IEC_REL_HWHEEL	   = 0x06
IEC_REL_DIAL	   = 0x07   # rotating the knob
IEC_REL_WHEEL	   = 0x08   # moving the shuttle ring
IEC_REL_MISC	   = 0x09
IEC_REL_MAX	   = 0x0f

# ------------------------------------------------------------------------

class powermate(threading.Thread):
    """
    Interface to Griffin PowerMate and Contour Shuttles
    """
    def __init__(self, event_receiver=None, filename=None, **kwargs):
        self.event_receiver = event_receiver
        self.handle = -1
        if not imported_ok:
            raise exceptions.RuntimeError, 'powermate not supported on this platform'

        if filename:
            if not self._open_device(filename):
                raise exceptions.RuntimeError, 'Unable to find powermate'
        else:
            ok = False
            for d in range(0, 16):
                if self._open_device("/dev/input/event%d" % d):
                    ok = True
                    break
            if not ok:
                raise exceptions.RuntimeError, 'Unable to find powermate'

        threading.Thread.__init__(self, **kwargs)
        self.setDaemon (1)
        self.keep_running = True
        self.start ()
        
    def __del__(self):
        self.keep_running = False
        if self.handle >= 0:
            os.close(self.handle)
            self.handle = -1

    def _open_device(self, filename):
        try:
            self.handle = os.open(filename, os.O_RDWR)
            if self.handle < 0:
                return False

            # read event device name
            name = fcntl.ioctl(self.handle, gru.hexint(0x80ff4506), chr(0) * 256)
            name = name.replace(chr(0), '')

            # do we see anything we recognize?
            if name == 'Griffin PowerMate' or name == 'Griffin SoundKnob':
                self.id = ID_POWERMATE
                self.mapper = _powermate_remapper()
            elif name == 'CAVS SpaceShuttle A/V' or name == 'Contour Design ShuttleXpress':
                self.id = ID_SHUTTLE_XPRESS
                self.mapper = _contour_remapper()
            elif name == 'Contour Design ShuttlePRO':
                self.id = ID_SHUTTLE_PRO
                self.mapper = _contour_remapper()
            elif name == 'Contour Design ShuttlePRO v2':
                self.id = ID_SHUTTLE_PRO_V2
                self.mapper = _contour_remapper()
            else:
                os.close(self.handle)
                self.handle = -1
                return False

            # get exclusive control of the device, using ioctl EVIOCGRAB
	    # there may be an issue with this on non x86 platforms and if
	    # the _IOW,_IOC,... macros in <asm/ioctl.h> are changed
            fcntl.ioctl(self.handle,gru.hexint(0x40044590), 1)
            return True
        except exceptions.OSError:
            return False

    
    def set_event_receiver(self, obj):
        self.event_receiver = obj


    def set_led_state(self, static_brightness, pulse_speed=0,
                      pulse_table=0, pulse_on_sleep=0, pulse_on_wake=0):
        """
        What do these magic values mean...
        """
        if self.id != ID_POWERMATE:
            return False
        
        static_brightness &= 0xff;
        if pulse_speed < 0:
            pulse_speed = 0
        if pulse_speed > 510:
            pulse_speed = 510
        if pulse_table < 0:
            pulse_table = 0
        if pulse_table > 2:
            pulse_table = 2
        pulse_on_sleep = not not pulse_on_sleep # not not = convert to 0/1
        pulse_on_wake  = not not pulse_on_wake
        magic = (static_brightness
                 | (pulse_speed << 8)
                 | (pulse_table << 17)
                 | (pulse_on_sleep << 19)
                 | (pulse_on_wake << 20))
        data = struct.pack(input_event_struct, 0, 0, 0x04, 0x01, magic)
        os.write(self.handle, data)
        return True

    def run (self):
        while (self.keep_running):
            s = os.read (self.handle, input_event_size)
            if not s:
                self.keep_running = False
                break

            raw_input_event = struct.unpack(input_event_struct,s)
            sec, usec, type, code, val = self.mapper(raw_input_event)
            
            if self.event_receiver is None:
                continue
            
            if type == IET_SYN:    # ignore
                pass
            elif type == IET_MSC:  # ignore (seems to be PowerMate reporting led brightness)
                pass                     
            elif type == IET_REL and code == IEC_REL_DIAL:
                #print "Dial: %d" % (val,)
                wx.PostEvent(self.event_receiver, PMRotateEvent(val))
            elif type == IET_REL and code == IEC_REL_WHEEL:
                #print "Shuttle: %d" % (val,)
                wx.PostEvent(self.event_receiver, PMShuttleEvent(val))
            elif type == IET_KEY:
                #print "Key: Btn%d %d" % (code - IEC_BTN_0, val)
                wx.PostEvent(self.event_receiver,
                             PMButtonEvent(code - IEC_BTN_0, val))
            else:
                print "powermate: unrecognized event: type = 0x%x  code = 0x%x  val = %d" % (type, code, val)


class _powermate_remapper(object):
    def __init__(self):
        pass
    def __call__(self, event):
        """
        Notice how nice and simple this is...
        """
        return event

class _contour_remapper(object):
    def __init__(self):
        self.prev = None
    def __call__(self, event):
        """
        ...and how screwed up this is
        """
        sec, usec, type, code, val = event
        if type == IET_REL and code == IEC_REL_WHEEL:
            # === Shuttle ring ===
            # First off, this really ought to be IET_ABS, not IET_REL!
            # They never generate a zero value so you can't
            # tell when the shuttle ring is back in the center.
            # We kludge around this by calling both -1 and 1 zero.
            if val == -1 or val == 1:
                return (sec, usec, type, code, 0)
            return event

        if type == IET_REL and code == IEC_REL_DIAL:
            # === Jog knob (rotary encoder) ===
            # Dim wits got it wrong again!  This one should return a
            # a relative value, e.g., -1, +1.  Instead they return
            # a total that runs modulo 256 (almost!).   For some
            # reason they count like this 253, 254, 255, 1, 2, 3

            if self.prev is None:                  # first time call
                self.prev = val
                return (sec, usec, IET_SYN, 0, 0)  # will be ignored above

            diff = val - self.prev
            if diff == 0:                          # sometimes it just sends stuff...
                return (sec, usec, IET_SYN, 0, 0)  # will be ignored above

            if abs(diff) > 100:      # crossed into the twilight zone
                if self.prev > val:  # we've wrapped going forward
                    self.prev = val
                    return (sec, usec, type, code, +1)
                else:                # we've wrapped going backward
                    self.prev = val
                    return (sec, usec, type, code, -1)

            self.prev = val
            return (sec, usec, type, code, diff)

        if type == IET_KEY:
            # remap keys so that all 3 gadgets have buttons 0 to 4 in common
            return (sec, usec, type, 
                    (IEC_BTN_5, IEC_BTN_6, IEC_BTN_7, IEC_BTN_8,
                     IEC_BTN_0, IEC_BTN_1, IEC_BTN_2, IEC_BTN_3, IEC_BTN_4,
                     IEC_BTN_9,  IEC_BTN_10,
                     IEC_BTN_11, IEC_BTN_12,
                     IEC_BTN_13, IEC_BTN_14)[code - IEC_BTN_0], val)
            
        return event

# ------------------------------------------------------------------------
# new wxPython event classes
# ------------------------------------------------------------------------

grEVT_POWERMATE_BUTTON  = wx.NewEventType()
grEVT_POWERMATE_ROTATE  = wx.NewEventType()
grEVT_POWERMATE_SHUTTLE = wx.NewEventType()

EVT_POWERMATE_BUTTON = wx.PyEventBinder(grEVT_POWERMATE_BUTTON, 0)
EVT_POWERMATE_ROTATE = wx.PyEventBinder(grEVT_POWERMATE_ROTATE, 0)
EVT_POWERMATE_SHUTTLE = wx.PyEventBinder(grEVT_POWERMATE_SHUTTLE, 0)

class PMButtonEvent(wx.PyEvent):
    def __init__(self, button, value):
        wx.PyEvent.__init__(self)
        self.SetEventType(grEVT_POWERMATE_BUTTON)
        self.button = button
        self.value = value

    def Clone (self): 
        self.__class__(self.GetId())
        

class PMRotateEvent(wx.PyEvent):
    def __init__(self, delta):
        wx.PyEvent.__init__(self)
        self.SetEventType (grEVT_POWERMATE_ROTATE)
        self.delta = delta

    def Clone (self): 
        self.__class__(self.GetId())


class PMShuttleEvent(wx.PyEvent):
    def __init__(self, position):
        wx.PyEvent.__init__(self)
        self.SetEventType (grEVT_POWERMATE_SHUTTLE)
        self.position = position

    def Clone (self): 
        self.__class__(self.GetId())
        
# ------------------------------------------------------------------------
#  Example usage
# ------------------------------------------------------------------------

if __name__ == '__main__':
    class Frame(wx.Frame):
        def __init__(self,parent=None,id=-1,title='Title',
                     pos=wx.DefaultPosition, size=(400,200)):
            wx.Frame.__init__(self,parent,id,title,pos,size)
            EVT_POWERMATE_BUTTON(self, self.on_button)
            EVT_POWERMATE_ROTATE(self, self.on_rotate)
            EVT_POWERMATE_SHUTTLE(self, self.on_shuttle)
            self.brightness = 128
            self.pulse_speed = 0
            
            try:
                self.pm = powermate(self)
            except:
                sys.stderr.write("Unable to find PowerMate or Contour Shuttle\n")
                sys.exit(1)

            self.pm.set_led_state(self.brightness, self.pulse_speed) 


        def on_button(self, evt):
            print "Button %d %s" % (evt.button,
                                    ("Released", "Pressed")[evt.value])

        def on_rotate(self, evt):
            print "Rotated %d" % (evt.delta,)
            if 0:
                new = max(0, min(255, self.brightness + evt.delta))
                if new != self.brightness:
                    self.brightness = new
                    self.pm.set_led_state(self.brightness, self.pulse_speed) 
        
        def on_shuttle(self, evt):
            print "Shuttle %d" % (evt.position,)
        
    class App(wx.App):
        def OnInit(self):
            title='PowerMate Demo'
            self.frame = Frame(parent=None,id=-1,title=title)
            self.frame.Show()
            self.SetTopWindow(self.frame)
            return True

    app = App()
    app.MainLoop ()
