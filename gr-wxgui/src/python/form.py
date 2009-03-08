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

import wx
from gnuradio import eng_notation

# ----------------------------------------------------------------
#                   Wrappers for certain widgets
# ----------------------------------------------------------------

def button_with_callback(parent, label, callback):
    new_id = wx.NewId()
    btn = wx.Button(parent, new_id, label)
    wx.EVT_BUTTON(parent, new_id, lambda evt: callback())
    return btn
    

# ----------------------------------------------------------------
#                        Format converters
# ----------------------------------------------------------------

class abstract_converter(object):
    def value_to_prim(self, v):
        """
        Convert from user specified value to value acceptable to underlying primitive.
        The underlying primitive usually expects strings.
        """
        raise NotImplementedError
    def prim_to_value(self, s):
        """
        Convert from underlying primitive value to user specified value.
        The underlying primitive usually expects strings.
        """
        raise NotImplementedError
    def help(self):
        return "Any string is acceptable"

class identity_converter(abstract_converter):
    def value_to_prim(self,v):
        return v
    def prim_to_value(self, s):
        return s

class int_converter(abstract_converter):
    def value_to_prim(self, v):
        return str(v)
    def prim_to_value(self, s):
        return int(s, 0)
    def help(self):
        return "Enter an integer.  Leading 0x indicates hex"

class float_converter(abstract_converter):
    def value_to_prim(self, v):
        return eng_notation.num_to_str(v)
    def prim_to_value(self, s):
        return eng_notation.str_to_num(s)
    def help(self):
        return "Enter a float with optional scale suffix.  E.g., 100.1M"


# ----------------------------------------------------------------
#               Various types of data entry fields
# ----------------------------------------------------------------

class field(object):
    """
    A field in a form.
    """
    def __init__(self, converter, value):
        self.converter = converter
        if value is not None:
            self.set_value(value)

    def set_value(self, v):
        self._set_prim_value(self.converter.value_to_prim(v))

    def get_value(self):
        return self.converter.prim_to_value(self._get_prim_value())

    def get_value_with_check(self):
        """
        Returns (value, error_msg), where error_msg is not None if there was problem
        """
        try:
            return (self.get_value(), None)
        except:
            return (None, self._error_msg())

    def _set_prim_value(self, v):
        raise NotImplementedError

    def _get_prim_value(self):
        raise NotImplementedError

    def _pair_with_label(self, widget, parent=None, sizer=None, label=None, weight=1):
        self.label = label
        if label is None:
            sizer.Add (widget, weight, wx.EXPAND)
            return widget
        elif 0:
            hbox = wx.BoxSizer(wx.HORIZONTAL)
            label_widget = wx.StaticText(parent, -1, label + ': ')
            hbox.Add(label_widget, 0, wx.EXPAND)
            hbox.Add(widget, 1, wx.EXPAND)
            sizer.Add(hbox, weight, wx.EXPAND)
            return widget
        else:
            label_widget = wx.StaticText(parent, -1, label + ': ')
            sizer.Add(label_widget, 0, wx.EXPAND)
            sizer.Add(widget, weight, wx.EXPAND)
            return widget
    
    def _error_msg(self):
        prefix = ''
        if self.label:
            prefix = self.label + ': '
        return "%s%s is invalid. %s" % (prefix, self._get_prim_value(),
                                        self.converter.help())

# static (display-only) text fields

class static_text_field(field):
    def __init__(self, parent=None, sizer=None, label=None, value=None,
                 converter=identity_converter(), weight=0):
        self.f = self._pair_with_label(wx.StaticText(parent, -1, ""),
                                       parent=parent, sizer=sizer, label=label, weight=weight)
        field.__init__(self, converter, value)

    def _get_prim_value(self):
        return self.f.GetLabel()

    def _set_prim_value(self, v):
        self.f.SetLabel(v)


class static_int_field(static_text_field):
    def __init__(self, parent=None, sizer=None, label=None, value=None, weight=0):
        static_text_field.__init__(self, parent, sizer, label, value, int_converter(), weight)

class static_float_field(static_text_field):
    def __init__(self, parent=None, sizer=None, label=None, value=None, weight=0):
        static_text_field.__init__(self, parent, sizer, label, value, float_converter(), weight)


# editable text fields

class text_field(field):
    def __init__(self, parent=None, sizer=None, label=None, value=None,
                 converter=identity_converter(), callback=None, weight=1):
        style = 0
        if callback:
            style = wx.TE_PROCESS_ENTER

        new_id = wx.NewId()
        w = wx.TextCtrl(parent, new_id, "", style=style)
        self.f = self._pair_with_label(w, parent=parent, sizer=sizer, label=label, weight=weight)
        if callback:
            wx.EVT_TEXT_ENTER(w, new_id, lambda evt: callback())
        field.__init__(self, converter, value)

    def _get_prim_value(self):
        return self.f.GetValue()

    def _set_prim_value(self, v):
        self.f.SetValue(v)


class int_field(text_field):
    def __init__(self, parent=None, sizer=None, label=None, value=None,
                 callback=None, weight=1):
        text_field.__init__(self, parent, sizer, label, value, int_converter(), callback, weight)

class float_field(text_field):
    def __init__(self, parent=None, sizer=None, label=None, value=None,
                 callback=None, weight=1):
        text_field.__init__(self, parent, sizer, label, value, float_converter(), callback, weight)

# other fields

class slider_field(field):
    def __init__(self, parent=None, sizer=None, label=None, value=None,
                 converter=identity_converter(), callback=None, min=0, max=100, weight=1):
        new_id = wx.NewId()
        w = wx.Slider(parent, new_id, (max+min)/2, min, max,
                      size=wx.Size(250, -1), style=wx.SL_HORIZONTAL | wx.SL_LABELS)
        self.f = self._pair_with_label(w, parent=parent, sizer=sizer, label=label, weight=weight)
        if callback:
            wx.EVT_COMMAND_SCROLL(w, new_id, lambda evt: callback(evt.GetInt()))
        field.__init__(self, converter, value)

    def _get_prim_value(self):
        return self.f.GetValue()

    def _set_prim_value(self, v):
        self.f.SetValue(int(v))

class quantized_slider_field(field):
    def __init__(self, parent=None, sizer=None, label=None, value=None,
                 converter=identity_converter(), callback=None, range=None, weight=1):
        if not isinstance(range, (tuple, list)) or len(range) != 3:
            raise ValueError, range

        self.min = range[0]
        self.max = range[1]
        self.step_size = float(range[2])
        nsteps = int((self.max-self.min)/self.step_size)
        
        new_id = wx.NewId()
        w = wx.Slider(parent, new_id, 0, 0, nsteps,
                      size=wx.Size(250, -1), style=wx.SL_HORIZONTAL)
        self.f = self._pair_with_label(w, parent=parent, sizer=sizer, label=label, weight=weight)
        if callback:
            wx.EVT_COMMAND_SCROLL(w, new_id,
                                  lambda evt: callback(self._map_out(evt.GetInt())))
        field.__init__(self, converter, value)

    def _get_prim_value(self):
        return self._map_out(self.f.GetValue())

    def _set_prim_value(self, v):
        self.f.SetValue(self._map_in(v))

    def _map_in(self, x):
        return int((x-self.min) / self.step_size)

    def _map_out(self, x):
        return x * self.step_size + self.min

class checkbox_field(field):
    def __init__(self, parent=None, sizer=None, label=None, value=None,
                 converter=identity_converter(), callback=None, weight=1):
        new_id = wx.NewId()
        w = wx.CheckBox(parent, new_id, label, style=wx.CHK_2STATE)
        self.f = self._pair_with_label(w, parent=parent, sizer=sizer, label=None, weight=weight)
        if callback:
            wx.EVT_CHECKBOX(w, new_id, lambda evt: callback(evt.GetInt()))
        field.__init__(self, converter, value)

    def _get_prim_value(self):
        return self.f.GetValue()

    def _set_prim_value(self, v):
        self.f.SetValue(int(v))


class radiobox_field(field):
    def __init__(self, parent=None, sizer=None, label=None, value=None,
                 converter=identity_converter(), callback=None, weight=1,
                 choices=None, major_dimension=1, specify_rows=False):
        new_id = wx.NewId()

        if specify_rows:
            style=wx.RA_SPECIFY_ROWS | wx.RA_HORIZONTAL
        else:
            style=wx.RA_SPECIFY_COLS | wx.RA_HORIZONTAL
            
        w = wx.RadioBox(parent, new_id, label=label, style=style, majorDimension=major_dimension,
                        choices=choices)
        self.f = self._pair_with_label(w, parent=parent, sizer=sizer, label=None, weight=weight)
        if callback:
            wx.EVT_RADIOBOX(w, new_id, lambda evt: callback(evt.GetString()))
        field.__init__(self, converter, value)

    def _get_prim_value(self):
        return self.f.GetStringSelection()

    def _set_prim_value(self, v):
        self.f.SetStringSelection(str(v))

# ----------------------------------------------------------------
#                         the form class
# ----------------------------------------------------------------

class form(dict):
    def __init__(self):
        dict.__init__(self)

    def check_input_for_errors(self):
        """
        Returns list of error messages if there's trouble,
        else empty list.
        """
        vals = [f.get_value_with_check() for f in self.values()]
        return [t[1] for t in vals if t[1] is not None]
        
    def get_key_vals(self):
        d = {}
        for (key, f) in self.items():
            d[key] = f.get_value()
        return d


    def _nop(*args): pass
    
    def check_input_and_call(self, callback, status_handler=_nop):
        """
        Return a function that checks the form for errors, and then if it's OK,
        invokes the user specified callback, passing it the form key/value dictionary.
        status_handler is called with a string indicating results.
        """
        def doit_callback(*ignore):
            errors = self.check_input_for_errors()
            if errors:
                status_handler(errors[0])
                #print '\n'.join(tuple(errors))
            else:
                kv = self.get_key_vals()
                if callback(kv):
                    status_handler("OK")
                else:
                    status_handler("Failed")

        return doit_callback



# ----------------------------------------------------------------
#                    Stand-alone example code
# ----------------------------------------------------------------

import sys
from gnuradio.wxgui import stdgui2

class demo_app_flow_graph (stdgui2.std_top_block):
    def __init__(self, frame, panel, vbox, argv):
        stdgui2.std_top_block.__init__ (self, frame, panel, vbox, argv)

        self.frame = frame
        self.panel = panel

        def _print_kv(kv):
            print "kv =", kv
            return True

        self.form = form()
        
        self.form['static1'] = \
            static_text_field(parent=panel, sizer=vbox,
                              label="Static Text",
                              value="The Static Value")

        self.form['text1'] = \
            text_field(parent=panel, sizer=vbox,
                       label="TextCtrl",
                       value="The Editable Value")

        self.form['int1'] = \
            int_field(parent=panel, sizer=vbox,
                      label="Int Field",
                      value=1234)

        self.form['float1'] = \
            float_field(parent=panel, sizer=vbox,
                      label="Float Field",
                      value=3.14159)

        self.doit = button_with_callback(
            panel, "Do It!",
            self.form.check_input_and_call(_print_kv, self._set_status_msg))

        vbox.Add(self.doit, 0, wx.CENTER)

    def _set_status_msg(self, msg):
        self.frame.GetStatusBar().SetStatusText(msg, 0)

            
def main ():
    app = stdgui2.stdapp(demo_app_flow_graph, "wxgui form demo", nstatus=1)
    app.MainLoop ()

if __name__ == '__main__':
    main ()
