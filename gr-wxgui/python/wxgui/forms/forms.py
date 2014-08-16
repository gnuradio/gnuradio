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
The forms module contains general purpose wx-gui forms for gnuradio apps.

The forms follow a layered model:
  * internal layer
    * deals with the wxgui objects directly
    * implemented in event handler and update methods
  * translation layer
    * translates the between the external and internal layers
    * handles parsing errors between layers
  * external layer
    * provided external access to the user
    * set_value, get_value, and optional callback
    * set and get through optional pubsub and key

Known problems:
  * An empty label in the radio box still consumes space.
  * The static text cannot resize the parent at runtime.
"""

EXT_KEY = 'external'
INT_KEY = 'internal'

import wx
import sys
from gnuradio.gr.pubsub import pubsub
import converters

EVT_DATA = wx.PyEventBinder(wx.NewEventType())
class DataEvent(wx.PyEvent):
	def __init__(self, data):
		wx.PyEvent.__init__(self, wx.NewId(), EVT_DATA.typeId)
		self.data = data

def make_bold(widget):
	font = widget.GetFont()
	font.SetWeight(wx.FONTWEIGHT_BOLD)
	widget.SetFont(font)

########################################################################
# Base Class Form
########################################################################
class _form_base(pubsub, wx.BoxSizer):
	def __init__(self, parent=None, sizer=None, proportion=0, flag=wx.EXPAND, ps=None, key='', value=None, callback=None, converter=converters.identity_converter()):
		pubsub.__init__(self)
		wx.BoxSizer.__init__(self, wx.HORIZONTAL)
		self._parent = parent
		self._key = key
		self._converter = converter
		self._callback = callback
		self._widgets = list()
		#add to the sizer if provided
		if sizer: sizer.Add(self, proportion, flag)
		#proxy the pubsub and key into this form
		if ps is not None:
			assert key
			self.proxy(EXT_KEY, ps, key)
		#no pubsub passed, must set initial value
		else: self.set_value(value)

	def __str__(self):
		return "Form: %s -> %s"%(self.__class__, self._key)

	def _add_widget(self, widget, label='', flag=0, label_prop=0, widget_prop=1):
		"""
		Add the main widget to this object sizer.
		If label is passed, add a label as well.
		Register the widget and the label in the widgets list (for enable/disable).
		Bind the update handler to the widget for data events.
		This ensures that the gui thread handles updating widgets.
		Setup the pusub triggers for external and internal.

                Args:
		    widget: the main widget
		    label: the optional label
		    flag: additional flags for widget
		    label_prop: the proportion for the label
		    widget_prop: the proportion for the widget
		"""
		#setup data event
		widget.Bind(EVT_DATA, lambda x: self._update(x.data))
		update = lambda x: wx.PostEvent(widget, DataEvent(x))
		#register widget
		self._widgets.append(widget)
		#create optional label
		if not label: self.Add(widget, widget_prop, wx.ALIGN_CENTER_VERTICAL | flag)
		else:
			label_text = wx.StaticText(self._parent, label='%s: '%label)
			self._widgets.append(label_text)
			self.Add(label_text, label_prop, wx.ALIGN_CENTER_VERTICAL | wx.ALIGN_LEFT)
			self.Add(widget, widget_prop, wx.ALIGN_CENTER_VERTICAL | wx.ALIGN_RIGHT | flag)
		#initialize without triggering pubsubs
		self._translate_external_to_internal(self[EXT_KEY])
		update(self[INT_KEY])
		#subscribe all the functions
		self.subscribe(INT_KEY, update)
		self.subscribe(INT_KEY, self._translate_internal_to_external)
		self.subscribe(EXT_KEY, self._translate_external_to_internal)

	def _translate_external_to_internal(self, external):
		try:
			internal = self._converter.external_to_internal(external)
			#prevent infinite loop between internal and external pubsub keys by only setting if changed
			if self[INT_KEY] != internal: self[INT_KEY] = internal
		except Exception, e:
			self._err_msg(external, e)
			self[INT_KEY] = self[INT_KEY] #reset to last good setting

	def _translate_internal_to_external(self, internal):
		try:
			external = self._converter.internal_to_external(internal)
			#prevent infinite loop between internal and external pubsub keys by only setting if changed
			if self[EXT_KEY] != external: self[EXT_KEY] = external
		except Exception, e:
			self._err_msg(internal, e)
			self[EXT_KEY] = self[EXT_KEY] #reset to last good setting
		if self._callback: self._callback(self[EXT_KEY])

	def _err_msg(self, value, e):
		print >> sys.stderr, self, 'Error translating value: "%s"\n\t%s\n\t%s'%(value, e, self._converter.help())

	#override in subclasses to handle the wxgui object
	def _update(self, value): raise NotImplementedError
	def _handle(self, event): raise NotImplementedError

	#provide a set/get interface for this form
	def get_value(self): return self[EXT_KEY]
	def set_value(self, value): self[EXT_KEY] = value

	def Disable(self, disable=True): self.Enable(not disable)
	def Enable(self, enable=True):
		if enable:
			for widget in self._widgets: widget.Enable()
		else:
			for widget in self._widgets: widget.Disable()

########################################################################
# Base Class Chooser Form
########################################################################
class _chooser_base(_form_base):
	def __init__(self, choices=[], labels=None, **kwargs):
		_form_base.__init__(self, converter=converters.chooser_converter(choices), **kwargs)
		self._choices = choices
		self._labels = map(str, labels or choices)

########################################################################
# Base Class Slider Form
########################################################################
class _slider_base(_form_base):
	def __init__(self, label='', length=-1, converter=None, num_steps=100, style=wx.SL_HORIZONTAL, **kwargs):
		_form_base.__init__(self, converter=converter, **kwargs)
		if style & wx.SL_HORIZONTAL: slider_size = wx.Size(length, -1)
		elif style & wx.SL_VERTICAL: slider_size = wx.Size(-1, length)
		else: raise NotImplementedError
		self._slider = wx.Slider(self._parent, minValue=0, maxValue=num_steps, size=slider_size, style=style)
		self._slider.Bind(wx.EVT_SCROLL, self._handle)
		self._add_widget(self._slider, label, flag=wx.EXPAND)

	def _handle(self, event): self[INT_KEY] = self._slider.GetValue()
	def _update(self, value): self._slider.SetValue(int(round(value)))

########################################################################
# Static Text Form
########################################################################
class static_text(_form_base):
	"""
	A text box form.

        Args:
	    parent: the parent widget
	    sizer: add this widget to sizer if provided (optional)
	    proportion: the proportion when added to the sizer (default=0)
	    flag: the flag argument when added to the sizer (default=wx.EXPAND)
	    ps: the pubsub object (optional)
	    key: the pubsub key (optional)
	    value: the default value (optional)
	    label: title label for this widget (optional)
	    width: the width of the form in px
	    bold: true to bold-ify the text (default=False)
	    units: a suffix to add after the text
	    converter: forms.str_converter(), int_converter(), float_converter()...
	"""
	def __init__(self, label='', width=-1, bold=False, units='', converter=converters.str_converter(), **kwargs):
		self._units = units
		_form_base.__init__(self, converter=converter, **kwargs)
		self._static_text = wx.StaticText(self._parent, size=wx.Size(width, -1))
		if bold: make_bold(self._static_text)
		self._add_widget(self._static_text, label)

	def _update(self, label):
			if self._units: label += ' ' + self._units
			self._static_text.SetLabel(label); self._parent.Layout()

########################################################################
# Text Box Form
########################################################################
class text_box(_form_base):
	"""
	A text box form.

        Args:
	    parent: the parent widget
	    sizer: add this widget to sizer if provided (optional)
	    proportion: the proportion when added to the sizer (default=0)
	    flag: the flag argument when added to the sizer (default=wx.EXPAND)
	    ps: the pubsub object (optional)
	    key: the pubsub key (optional)
	    value: the default value (optional)
	    label: title label for this widget (optional)
	    width: the width of the form in px
	    converter: forms.str_converter(), int_converter(), float_converter()...
	"""
	def __init__(self, label='', width=-1, converter=converters.eval_converter(), **kwargs):
		_form_base.__init__(self, converter=converter, **kwargs)
		self._text_box = wx.TextCtrl(self._parent, size=wx.Size(width, -1), style=wx.TE_PROCESS_ENTER)
		self._default_bg_colour = self._text_box.GetBackgroundColour()
		self._text_box.Bind(wx.EVT_TEXT_ENTER, self._handle)
		self._text_box.Bind(wx.EVT_TEXT, self._update_color)
		self._add_widget(self._text_box, label)

	def _update_color(self, *args):
		if self._text_box.GetValue() == self[INT_KEY]:
			self._text_box.SetBackgroundColour(self._default_bg_colour)
		else: self._text_box.SetBackgroundColour('#EEDDDD')

	def _handle(self, event): self[INT_KEY] = self._text_box.GetValue()
	def _update(self, value): self._text_box.SetValue(value); self._update_color()

########################################################################
# Slider Form
#  Linear Slider
#  Logarithmic Slider
########################################################################
class slider(_slider_base):
	"""
	A generic linear slider.

        Args:
	    parent: the parent widget
	    sizer: add this widget to sizer if provided (optional)
	    proportion: the proportion when added to the sizer (default=0)
	    flag: the flag argument when added to the sizer (default=wx.EXPAND)
	    ps: the pubsub object (optional)
	    key: the pubsub key (optional)
	    value: the default value (optional)
	    label: title label for this widget (optional)
	    length: the length of the slider in px (optional)
	    style: wx.SL_HORIZONTAL or wx.SL_VERTICAL (default=horizontal)
	    minimum: the minimum value
	    maximum: the maximum value
	    num_steps: the number of slider steps (or specify step_size)
	    step_size: the step between slider jumps (or specify num_steps)
	    cast: a cast function, int, or float (default=float)
	"""
	def __init__(self, minimum=-100, maximum=100, num_steps=100, step_size=None, cast=float, **kwargs):
		assert step_size or num_steps
		if step_size is not None: num_steps = (maximum - minimum)/step_size
		converter = converters.slider_converter(minimum=minimum, maximum=maximum, num_steps=num_steps, cast=cast)
		_slider_base.__init__(self, converter=converter, num_steps=num_steps, **kwargs)

class log_slider(_slider_base):
	"""
	A generic logarithmic slider.
	The sliders min and max values are base**min_exp and base**max_exp.

        Args:
	    parent: the parent widget
	    sizer: add this widget to sizer if provided (optional)
	    proportion: the proportion when added to the sizer (default=0)
	    flag: the flag argument when added to the sizer (default=wx.EXPAND)
	    ps: the pubsub object (optional)
	    key: the pubsub key (optional)
	    value: the default value (optional)
	    label: title label for this widget (optional)
	    length: the length of the slider in px (optional)
	    style: wx.SL_HORIZONTAL or wx.SL_VERTICAL (default=horizontal)
	    min_exp: the minimum exponent
	    max_exp: the maximum exponent
	    base: the exponent base in base**exp
	    num_steps: the number of slider steps (or specify step_size)
	    step_size: the exponent step size (or specify num_steps)
	"""
	def __init__(self, min_exp=0, max_exp=1, base=10, num_steps=100, step_size=None, **kwargs):
		assert step_size or num_steps
		if step_size is not None: num_steps = (max_exp - min_exp)/step_size
		converter = converters.log_slider_converter(min_exp=min_exp, max_exp=max_exp, num_steps=num_steps, base=base)
		_slider_base.__init__(self, converter=converter, num_steps=num_steps, **kwargs)

########################################################################
# Gauge Form
########################################################################
class gauge(_form_base):
	"""
	A gauge bar.
	The gauge displays floating point values between the minimum and maximum.

        Args:
	    parent: the parent widget
	    sizer: add this widget to sizer if provided (optional)
	    proportion: the proportion when added to the sizer (default=0)
	    flag: the flag argument when added to the sizer (default=wx.EXPAND)
	    ps: the pubsub object (optional)
	    key: the pubsub key (optional)
	    value: the default value (optional)
	    label: title label for this widget (optional)
	    length: the length of the slider in px (optional)
	    style: wx.GA_HORIZONTAL or wx.GA_VERTICAL (default=horizontal)
	    minimum: the minimum value
	    maximum: the maximum value
	    num_steps: the number of slider steps (or specify step_size)
	    step_size: the step between slider jumps (or specify num_steps)
	"""
	def __init__(self, label='', length=-1, minimum=-100, maximum=100, num_steps=100, step_size=None, style=wx.GA_HORIZONTAL, **kwargs):
		assert step_size or num_steps
		if step_size is not None: num_steps = (maximum - minimum)/step_size
		converter = converters.slider_converter(minimum=minimum, maximum=maximum, num_steps=num_steps, cast=float)
		_form_base.__init__(self, converter=converter, **kwargs)
		if style & wx.SL_HORIZONTAL: gauge_size = wx.Size(length, -1)
		elif style & wx.SL_VERTICAL: gauge_size = wx.Size(-1, length)
		else: raise NotImplementedError
		self._gauge = wx.Gauge(self._parent, range=num_steps, size=gauge_size, style=style)
		self._add_widget(self._gauge, label, flag=wx.EXPAND)

	def _update(self, value): self._gauge.SetValue(value)

########################################################################
# Check Box Form
########################################################################
class check_box(_form_base):
	"""
	Create a check box form.

        Args:
	    parent: the parent widget
	    sizer: add this widget to sizer if provided (optional)
	    proportion: the proportion when added to the sizer (default=0)
	    flag: the flag argument when added to the sizer (default=wx.EXPAND)
	    ps: the pubsub object (optional)
	    key: the pubsub key (optional)
	    value: the default value (optional)
	    true: the value for form when checked (default=True)
	    false: the value for form when unchecked (default=False)
	    label: title label for this widget (optional)
	"""
	def __init__(self, label='', true=True, false=False, **kwargs):
		_form_base.__init__(self, converter=converters.bool_converter(true=true, false=false), **kwargs)
		self._check_box = wx.CheckBox(self._parent, style=wx.CHK_2STATE, label=label)
		self._check_box.Bind(wx.EVT_CHECKBOX, self._handle)
		self._add_widget(self._check_box)

	def _handle(self, event): self[INT_KEY] = self._check_box.IsChecked()
	def _update(self, checked): self._check_box.SetValue(checked)

########################################################################
# Drop Down Chooser Form
########################################################################
class drop_down(_chooser_base):
	"""
	Create a drop down menu form.

        Args:
	    parent: the parent widget
	    sizer: add this widget to sizer if provided (optional)
	    proportion: the proportion when added to the sizer (default=0)
	    flag: the flag argument when added to the sizer (default=wx.EXPAND)
	    ps: the pubsub object (optional)
	    key: the pubsub key (optional)
	    value: the default value (optional)
	    choices: list of possible values
	    labels: list of labels for each choice (default=choices)
	    label: title label for this widget (optional)
	    width: the form width in px (optional)
	"""
	def __init__(self, label='', width=-1, **kwargs):
		_chooser_base.__init__(self, **kwargs)
		self._drop_down = wx.Choice(self._parent, choices=self._labels, size=wx.Size(width, -1))
		self._drop_down.Bind(wx.EVT_CHOICE, self._handle)
		self._add_widget(self._drop_down, label, widget_prop=0, label_prop=1)

	def _handle(self, event): self[INT_KEY] = self._drop_down.GetSelection()
	def _update(self, i): self._drop_down.SetSelection(i)

########################################################################
# Button Chooser Form
#  Circularly move through the choices with each click.
#  Can be a single-click button with one choice.
#  Can be a 2-state button with two choices.
########################################################################
class button(_chooser_base):
	"""
	Create a multi-state button.
	    parent the parent widget
	    sizer add this widget to sizer if provided (optional)
	    proportion the proportion when added to the sizer (default=0)
	    flag the flag argument when added to the sizer (default=wx.EXPAND)
	    ps the pubsub object (optional)
	    key the pubsub key (optional)
	    value the default value (optional)
	    choices list of possible values
	    labels list of labels for each choice (default=choices)
	    width the width of the button in pixels (optional)
	    style style arguments (optional)
	    label title label for this widget (optional)
	"""
	def __init__(self, label='', style=0, width=-1, **kwargs):
		_chooser_base.__init__(self, **kwargs)
		self._button = wx.Button(self._parent, size=wx.Size(width, -1), style=style)
		self._button.Bind(wx.EVT_BUTTON, self._handle)
		self._add_widget(self._button, label, widget_prop=((not style&wx.BU_EXACTFIT) and 1 or 0))

	def _handle(self, event): self[INT_KEY] = (self[INT_KEY] + 1)%len(self._choices) #circularly increment index
	def _update(self, i): self._button.SetLabel(self._labels[i]); self.Layout()

class toggle_button(button):
	"""
	Create a dual-state button.
	This button will alternate between True and False when clicked.

        Args:
	    parent: the parent widget
	    sizer: add this widget to sizer if provided (optional)
	    proportion: the proportion when added to the sizer (default=0)
	    flag: the flag argument when added to the sizer (default=wx.EXPAND)
	    ps: the pubsub object (optional)
	    key: the pubsub key (optional)
	    value: the default value (optional)
	    width: the width of the button in pixels (optional)
	    style: style arguments (optional)
	    true_label: the button's label in the true state
	    false_label: the button's label in the false state
	"""
	def __init__(self, true_label='On (click to stop)', false_label='Off (click to start)', **kwargs):
		button.__init__(self, choices=[True, False], labels=[true_label, false_label], **kwargs)

class single_button(toggle_button):
	"""
	Create a single state button.
	This button will callback() when clicked.
	For use when state holding is not important.

        Args:
	    parent: the parent widget
	    sizer: add this widget to sizer if provided (optional)
	    proportion: the proportion when added to the sizer (default=0)
	    flag: the flag argument when added to the sizer (default=wx.EXPAND)
	    ps: the pubsub object (optional)
	    key: the pubsub key (optional)
	    value: the default value (optional)
	    width: the width of the button in pixels (optional)
	    style: style arguments (optional)
	    label: the button's label
	"""
	def __init__(self, label='click for callback', **kwargs):
		toggle_button.__init__(self, true_label=label, false_label=label, value=True, **kwargs)

########################################################################
# Radio Buttons Chooser Form
########################################################################
class radio_buttons(_chooser_base):
	"""
	Create a radio button form.

        Args:
	    parent: the parent widget
	    sizer: add this widget to sizer if provided (optional)
	    proportion: the proportion when added to the sizer (default=0)
	    flag: the flag argument when added to the sizer (default=wx.EXPAND)
	    ps: the pubsub object (optional)
	    key: the pubsub key (optional)
	    value the default value (optional)
	    choices: list of possible values
	    labels: list of labels for each choice (default=choices)
	    major_dimension: the number of rows/cols (default=auto)
	    label: title label for this widget (optional)
	    style: useful style args: wx.RA_HORIZONTAL, wx.RA_VERTICAL, wx.NO_BORDER (default=wx.RA_HORIZONTAL)
	"""
	def __init__(self, style=wx.RA_HORIZONTAL, label='', major_dimension=0, **kwargs):
		_chooser_base.__init__(self, **kwargs)
		#create radio buttons
		self._radio_buttons = wx.RadioBox(self._parent, choices=self._labels, style=style, label=label, majorDimension=major_dimension)
		self._radio_buttons.Bind(wx.EVT_RADIOBOX, self._handle)
		self._add_widget(self._radio_buttons)

	def _handle(self, event): self[INT_KEY] = self._radio_buttons.GetSelection()
	def _update(self, i): self._radio_buttons.SetSelection(i)

########################################################################
# Notebook Chooser Form
#  The notebook pages/tabs are for selecting between choices.
#  A page must be added to the notebook for each choice.
########################################################################
class notebook(_chooser_base):
	def __init__(self, pages, notebook, **kwargs):
		_chooser_base.__init__(self, **kwargs)
		assert len(pages) == len(self._choices)
		self._notebook = notebook
		self._notebook.Bind(wx.EVT_NOTEBOOK_PAGE_CHANGING, self._handle)
		#add pages, setting the label on each tab
		for i, page in enumerate(pages):
			self._notebook.AddPage(page, self._labels[i])
		self._add_widget(self._notebook)

	def _handle(self, event): self[INT_KEY] = event.GetSelection()
	# SetSelection triggers a page change event (deprecated, breaks on Windows) and ChangeSelection does not
	def _update(self, i): self._notebook.ChangeSelection(i)

# ----------------------------------------------------------------
# Stand-alone test application
# ----------------------------------------------------------------

import wx
from gnuradio.wxgui import gui

class app_gui (object):
    def __init__(self, frame, panel, vbox, top_block, options, args):

        def callback(v): print v

        radio_buttons(
            sizer=vbox,
            parent=panel,
            choices=[2, 4, 8, 16],
            labels=['two', 'four', 'eight', 'sixteen'],
            value=4,
            style=wx.RA_HORIZONTAL,
            label='test radio long string',
            callback=callback,
            #major_dimension = 2,
        )

        radio_buttons(
            sizer=vbox,
            parent=panel,
            choices=[2, 4, 8, 16],
            labels=['two', 'four', 'eight', 'sixteen'],
            value=4,
            style=wx.RA_VERTICAL,
            label='test radio long string',
            callback=callback,
            #major_dimension = 2,
        )

        radio_buttons(
            sizer=vbox,
            parent=panel,
            choices=[2, 4, 8, 16],
            labels=['two', 'four', 'eight', 'sixteen'],
            value=4,
            style=wx.RA_VERTICAL | wx.NO_BORDER,
            callback=callback,
            #major_dimension = 2,
        )

        button(
            sizer=vbox,
            parent=panel,
            choices=[2, 4, 8, 16],
            labels=['two', 'four', 'eight', 'sixteen'],
            value=2,
            label='button value',
            callback=callback,
            #width=100,
        )


        drop_down(
            sizer=vbox,
            parent=panel,
            choices=[2, 4, 8, 16],
            value=2,
            label='Choose One',
            callback=callback,
        )
        check_box(
            sizer=vbox,
            parent=panel,
            value=False,
            label='check me',
            callback=callback,
        )
        text_box(
            sizer=vbox,
            parent=panel,
            value=3,
            label='text box',
            callback=callback,
            width=200,
        )

        static_text(
            sizer=vbox,
            parent=panel,
            value='bob',
            label='static text',
            width=-1,
            bold=True,
        )

        slider(
            sizer=vbox,
            parent=panel,
            value=12,
            label='slider',
            callback=callback,
        )

        log_slider(
            sizer=vbox,
            parent=panel,
            value=12,
            label='slider',
            callback=callback,
        )

        slider(
            sizer=vbox,
            parent=panel,
            value=12,
            label='slider',
            callback=callback,
            style=wx.SL_VERTICAL,
            length=30,
        )

        toggle_button(
            sizer=vbox,
            parent=panel,
            value=True,
            label='toggle it',
            callback=callback,
        )

        single_button(
            sizer=vbox,
            parent=panel,
            label='sig test',
            callback=callback,
        )

if __name__ == "__main__":
    try:

        # Create the GUI application
        app = gui.app(
                      gui=app_gui,                     # User interface class
                      title="Test Forms",  # Top window title
                      )

        # And run it
        app.MainLoop()

    except RuntimeError, e:
        print e
        sys.exit(1)
