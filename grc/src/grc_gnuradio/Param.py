"""
Copyright 2008 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""
##@package grc_gnuradio.Param
#Flow graph block parameters.
#@author Josh Blum

from utils import expr_utils
from grc.elements.Param import Param as _Param
import os

class Param(_Param):

	_init = False
	_hostage_cells = list()

	##possible param types
	TYPES = _Param.TYPES + [
		'complex', 'real', 'int',
		'complex_vector', 'real_vector', 'int_vector',
		'hex', 'string',
		'file_open', 'file_save',
		'id',
		'grid_pos', 'import',
	]

	def get_hide(self):
		"""!
		Get the hide value from the base class.
		If hide was empty, and this is a type controller, set hide to part.
		If hide was empty, and this is an id of a non variable, set hide to part.
		@return hide the hide property string
		"""
		hide = _Param.get_hide(self)
		#hide IO controlling params
		if not hide and self.get_key() in (
			'type', 'vlen', 'num_inputs', 'num_outputs'
		): hide = 'part'
		#hide ID in non variable blocks
		elif not hide and self.get_key() == 'id' and self.get_parent().get_key() not in (
			'variable', 'variable_slider', 'variable_chooser', 'variable_text_box', 'parameter', 'options'
		): hide = 'part'
		return hide

	def evaluate(self):
		"""!
		Evaluate the value.
		@return evaluated type
		"""
		self._lisitify_flag = False
		self._stringify_flag = False
		self._hostage_cells = list()
		def eval_string(v):
			try:
				e = self.get_parent().get_parent().evaluate(v)
				assert(isinstance(e, str))
				return e
			except:
				self._stringify_flag = True
				return v
		t = self.get_type()
		v = self.get_value()
		#########################
		# Enum Type
		#########################
		if self.is_enum(): return self.get_value()
		#########################
		# Numeric Types
		#########################
		elif t in ('raw', 'complex', 'real', 'int', 'complex_vector', 'real_vector', 'int_vector', 'hex'):
			#raise exception if python cannot evaluate this value
			try: e = self.get_parent().get_parent().evaluate(v)
			except:
				self._add_error_message('Value "%s" cannot be evaluated.'%v)
				raise Exception
			#raise an exception if the data is invalid
			if t == 'raw': return e
			elif t == 'complex':
				try: assert(isinstance(e, (complex, float, int, long)))
				except AssertionError:
					self._add_error_message('Expression "%s" is invalid for type complex.'%str(e))
					raise Exception
				return e
			elif t == 'real':
				try: assert(isinstance(e, (float, int, long)))
				except AssertionError:
					self._add_error_message('Expression "%s" is invalid for type real.'%str(e))
					raise Exception
				return e
			elif t == 'int':
				try: assert(isinstance(e, (int, long)))
				except AssertionError:
					self._add_error_message('Expression "%s" is invalid for type integer.'%str(e))
					raise Exception
				return e
			elif t == 'complex_vector':
				if not isinstance(e, (tuple, list, set)):
					self._lisitify_flag = True
					e = [e]
				try:
					for ei in e:
						assert(isinstance(ei, (complex, float, int, long)))
				except AssertionError:
					self._add_error_message('Expression "%s" is invalid for type complex vector.'%str(e))
					raise Exception
				return e
			elif t == 'real_vector':
				if not isinstance(e, (tuple, list, set)):
					self._lisitify_flag = True
					e = [e]
				try:
					for ei in e:
						assert(isinstance(ei, (float, int, long)))
				except AssertionError:
					self._add_error_message('Expression "%s" is invalid for type real vector.'%str(e))
					raise Exception
				return e
			elif t == 'int_vector':
				if not isinstance(e, (tuple, list, set)):
					self._lisitify_flag = True
					e = [e]
				try:
					for ei in e:
						assert(isinstance(ei, (int, long)))
				except AssertionError:
					self._add_error_message('Expression "%s" is invalid for type integer vector.'%str(e))
					raise Exception
				return e
			elif t == 'hex':
				return hex(e)
			else: raise TypeError, 'Type "%s" not handled'%t
		#########################
		# String Types
		#########################
		elif t in ('string', 'file_open', 'file_save'):
			#do not check if file/directory exists, that is a runtime issue
			e = eval_string(v)
			return str(e)
		#########################
		# Unique ID Type
		#########################
		elif t == 'id':
			#can python use this as a variable?
			try:
				assert(len(v) > 0)
				assert(v[0].isalpha())
				for c in v: assert(c.isalnum() or c in ('_',))
			except AssertionError:
				self._add_error_message('ID "%s" must be alpha-numeric or underscored, and begin with a letter.'%v)
				raise Exception
			params = self.get_all_params('id')
			keys = [param.get_value() for param in params]
			try: assert(len(keys) == len(set(keys)))
			except:
				self._add_error_message('ID "%s" is not unique.'%v)
				raise Exception
			return v
		#########################
		# Grid Position Type
		#########################
		elif t == 'grid_pos':
			if not v: return '' #allow for empty grid pos
			e = self.get_parent().get_parent().evaluate(v)
			try:
				assert(isinstance(e, (list, tuple)) and len(e) == 4)
				for ei in e: assert(isinstance(ei, int))
			except AssertionError:
				self._add_error_message('A grid position must be a list of 4 integers.')
				raise Exception
			row, col, row_span, col_span = e
			#check row, col
			try: assert(row >= 0 and col >= 0)
			except AssertionError:
				self._add_error_message('Row and column must be non-negative.')
				raise Exception
			#check row span, col span
			try: assert(row_span > 0 and col_span > 0)
			except AssertionError:
				self._add_error_message('Row and column span must be greater than zero.')
				raise Exception
			#calculate hostage cells
			for r in range(row_span):
				for c in range(col_span):
					self._hostage_cells.append((row+r, col+c))
			#avoid collisions
			params = filter(lambda p: p is not self, self.get_all_params('grid_pos'))
			for param in params:
				for cell in param._hostage_cells:
					if cell in self._hostage_cells:
						self._add_error_message('Another graphical element is using cell "%s".'%str(cell))
						raise Exception
			return e
		#########################
		# Import Type
		#########################
		elif t == 'import':
			n = dict() #new namespace
			try: exec v in n
			except ImportError:
				self._add_error_message('Import "%s" failed.'%v)
				raise Exception
			except Exception:
				self._add_error_message('Bad import syntax: "%s".'%v)
				raise Exception
			return filter(lambda k: str(k) != '__builtins__', n.keys())
		#########################
		else: raise TypeError, 'Type "%s" not handled'%t

	def to_code(self):
		"""!
		Convert the value to code.
		@return a string representing the code
		"""
		#run init tasks in evaluate
		if not self._init:
			self.evaluate()
			self._init = True
		v = self.get_value()
		t = self.get_type()
		if t in ('string', 'file_open', 'file_save'): #string types
			if self._stringify_flag:
				return '"%s"'%v.replace('"', '\"')
			else:
				return v
		elif t in ('complex_vector', 'real_vector', 'int_vector'): #vector types
			if self._lisitify_flag:
				return '(%s, )'%v
			else:
				return '(%s)'%v
		else:
			return v

	def get_all_params(self, type):
		"""!
		Get all the params from the flowgraph that have the given type.
		@param type the specified type
		@return a list of params
		"""
		return sum([filter(lambda p: p.get_type() == type, block.get_params()) for block in self.get_parent().get_parent().get_blocks()], [])

