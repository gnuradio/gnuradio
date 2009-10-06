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

from gnuradio import eng_notation
import math

class abstract_converter(object):
	def external_to_internal(self, v):
		"""
		Convert from user specified value to value acceptable to underlying primitive.
		The underlying primitive usually expects strings.
		"""
		raise NotImplementedError
	def internal_to_external(self, s):
		"""
		Convert from underlying primitive value to user specified value.
		The underlying primitive usually expects strings.
		"""
		raise NotImplementedError
	def help(self):
		return "Any string is acceptable"

class identity_converter(abstract_converter):
	def external_to_internal(self,v):
		return v
	def internal_to_external(self, s):
		return s

########################################################################
# Commonly used converters
########################################################################
class chooser_converter(abstract_converter):
	"""
	Convert between a set of possible choices and an index.
	Used in the chooser base and all sub-classes.
	"""
	def __init__(self, choices):
		#choices must be a list because tuple does not have .index() in python2.5
		self._choices = list(choices)
	def external_to_internal(self, choice):
		return self._choices.index(choice)
	def internal_to_external(self, index):
		return self._choices[index]
	def help(self):
		return 'Enter a possible value in choices: "%s"'%str(self._choices)

class bool_converter(abstract_converter):
	"""
	The internal representation is boolean.
	The external representation is specified.
	Used in the check box form.
	"""
	def __init__(self, true, false):
		self._true = true
		self._false = false
	def external_to_internal(self, v):
		if v == self._true: return True
		if v == self._false: return False
		raise Exception, 'Value "%s" is not a possible option.'%v
	def internal_to_external(self, v):
		if v: return self._true
		else: return self._false
	def help(self):
		return "Value must be in (%s, %s)."%(self._true, self._false)

class eval_converter(abstract_converter):
	"""
	A catchall converter when int and float are not enough.
	Evaluate the internal representation with python's eval().
	Possible uses, set a complex number, constellation points.
	Used in text box.
	"""
	def __init__(self, formatter=lambda x: '%s'%(x)):
		self._formatter = formatter
	def external_to_internal(self, v):
		return self._formatter(v)
	def internal_to_external(self, s):
		return eval(s)
	def help(self):
		return "Value must be evaluatable by python's eval."

class str_converter(abstract_converter):
	def __init__(self, formatter=lambda x: '%s'%(x)):
		self._formatter = formatter
	def external_to_internal(self, v):
		return self._formatter(v)
	def internal_to_external(self, s):
		return str(s)

class int_converter(abstract_converter):
	def __init__(self, formatter=lambda x: '%d'%round(x)):
		self._formatter = formatter
	def external_to_internal(self, v):
		return self._formatter(v)
	def internal_to_external(self, s):
		return int(s, 0)
	def help(self):
		return "Enter an integer.  Leading 0x indicates hex"

class float_converter(abstract_converter):
	def __init__(self, formatter=eng_notation.num_to_str):
		self._formatter = formatter
	def external_to_internal(self, v):
		return self._formatter(v)
	def internal_to_external(self, s):
		return eng_notation.str_to_num(s)
	def help(self):
		return "Enter a float with optional scale suffix.  E.g., 100.1M"

class slider_converter(abstract_converter):
	"""
	Scale values to and from the slider.
	"""
	def __init__(self, minimum, maximum, num_steps, cast):
		assert minimum < maximum
		assert num_steps > 0
		self._offset = minimum
		self._scaler = float(maximum - minimum)/num_steps
		self._cast = cast
	def external_to_internal(self, v):
		return (v - self._offset)/self._scaler
	def internal_to_external(self, v):
		return self._cast(v*self._scaler + self._offset)
	def help(self):
		return "Value should be within slider range"

class log_slider_converter(slider_converter):
	def __init__(self, min_exp, max_exp, num_steps, base):
		assert min_exp < max_exp
		assert num_steps > 0
		self._base = base
		slider_converter.__init__(self, minimum=min_exp, maximum=max_exp, num_steps=num_steps, cast=float)
	def external_to_internal(self, v):
		return slider_converter.external_to_internal(self, math.log(v, self._base))
	def internal_to_external(self, v):
		return self._base**slider_converter.internal_to_external(self, v)
