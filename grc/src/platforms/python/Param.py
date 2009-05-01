"""
Copyright 2008, 2009 Free Software Foundation, Inc.
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

from utils import expr_utils
from .. base.Param import Param as _Param, EntryParam
import Constants
import numpy
import os
import pygtk
pygtk.require('2.0')
import gtk
from gnuradio import eng_notation

class FileParam(EntryParam):
	"""Provide an entry box for filename and a button to browse for a file."""

	def __init__(self, *args, **kwargs):
		EntryParam.__init__(self, *args, **kwargs)
		input = gtk.Button('...')
		input.connect('clicked', self._handle_clicked)
		self.pack_start(input, False)

	def _handle_clicked(self, widget=None):
		"""
		If the button was clicked, open a file dialog in open/save format.
		Replace the text in the entry with the new filename from the file dialog.
		"""
		#get the paths
		file_path = self.param.is_valid() and self.param.evaluate() or ''
		(dirname, basename) = os.path.isfile(file_path) and os.path.split(file_path) or (file_path, '')
		if not os.path.exists(dirname): dirname = os.getcwd() #fix bad paths
		#build the dialog
		if self.param.get_type() == 'file_open':
			file_dialog = gtk.FileChooserDialog('Open a Data File...', None,
				gtk.FILE_CHOOSER_ACTION_OPEN, ('gtk-cancel',gtk.RESPONSE_CANCEL,'gtk-open',gtk.RESPONSE_OK))
		elif self.param.get_type() == 'file_save':
			file_dialog = gtk.FileChooserDialog('Save a Data File...', None,
				gtk.FILE_CHOOSER_ACTION_SAVE, ('gtk-cancel',gtk.RESPONSE_CANCEL, 'gtk-save',gtk.RESPONSE_OK))
			file_dialog.set_do_overwrite_confirmation(True)
			file_dialog.set_current_name(basename) #show the current filename
		file_dialog.set_current_folder(dirname) #current directory
		file_dialog.set_select_multiple(False)
		file_dialog.set_local_only(True)
		if gtk.RESPONSE_OK == file_dialog.run(): #run the dialog
			file_path = file_dialog.get_filename() #get the file path
			self.entry.set_text(file_path)
			self._handle_changed()
		file_dialog.destroy() #destroy the dialog

#define types, native python + numpy
VECTOR_TYPES = (tuple, list, set, numpy.ndarray)
COMPLEX_TYPES = [complex, numpy.complex, numpy.complex64, numpy.complex128]
REAL_TYPES = [float, numpy.float, numpy.float32, numpy.float64]
INT_TYPES = [int, long, numpy.int, numpy.int8, numpy.int16, numpy.int32, numpy.uint64,
	numpy.uint, numpy.uint8, numpy.uint16, numpy.uint32, numpy.uint64]
#cast to tuple for isinstance, concat subtypes
COMPLEX_TYPES = tuple(COMPLEX_TYPES + REAL_TYPES + INT_TYPES)
REAL_TYPES = tuple(REAL_TYPES + INT_TYPES)
INT_TYPES = tuple(INT_TYPES)

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

	def __repr__(self):
		"""
		Get the repr (nice string format) for this param.
		@return the string representation
		"""
		if self.get_value() in self.get_option_keys(): return self.get_option(self.get_value()).get_name()
		##################################################
		# display logic for numbers
		##################################################
		def num_to_str(num):
			if isinstance(num, COMPLEX_TYPES):
				num = complex(num) #cast to python complex
				if num == 0: return '0' #value is zero
				elif num.imag == 0: return '%s'%eng_notation.num_to_str(num.real) #value is real
				elif num.real == 0: return '%sj'%eng_notation.num_to_str(num.imag) #value is imaginary
				elif num.imag < 0: return '%s-%sj'%(eng_notation.num_to_str(num.real), eng_notation.num_to_str(abs(num.imag)))
				else: return '%s+%sj'%(eng_notation.num_to_str(num.real), eng_notation.num_to_str(num.imag))
			else: return str(num)
		##################################################
		# split up formatting by type
		##################################################
		truncate = 0 #default center truncate
		max_len = max(27 - len(self.get_name()), 3)
		e = self.evaluate()
		t = self.get_type()
		if isinstance(e, COMPLEX_TYPES): dt_str = num_to_str(e)
		elif isinstance(e, VECTOR_TYPES): #vector types
			if len(e) > 8:
				dt_str = self.get_value() #large vectors use code
				truncate = 1
			else: dt_str = ', '.join(map(num_to_str, e)) #small vectors use eval
		elif t in ('file_open', 'file_save'):
			dt_str = self.get_value()
			truncate = -1
		else: dt_str = str(e) #other types
		##################################################
		# truncate
		##################################################
		if len(dt_str) > max_len:
			if truncate < 0: #front truncate
				dt_str = '...' + dt_str[3-max_len:]
			elif truncate == 0: #center truncate
				dt_str = dt_str[:max_len/2 -3] + '...' + dt_str[-max_len/2:]
			elif truncate > 0: #rear truncate
				dt_str = dt_str[:max_len-3] + '...'
		return dt_str

	def get_input_class(self):
		if self.get_type() in ('file_open', 'file_save'): return FileParam
		return _Param.get_input_class(self)

	def get_color(self):
		"""
		Get the color that represents this param's type.
		@return a hex color code.
		"""
		try:
			return {
				#number types
				'complex': Constants.COMPLEX_COLOR_SPEC,
				'real': Constants.FLOAT_COLOR_SPEC,
				'int': Constants.INT_COLOR_SPEC,
				#vector types
				'complex_vector': Constants.COMPLEX_VECTOR_COLOR_SPEC,
				'real_vector': Constants.FLOAT_VECTOR_COLOR_SPEC,
				'int_vector': Constants.INT_VECTOR_COLOR_SPEC,
				#special
				'hex': Constants.INT_COLOR_SPEC,
				'string': Constants.BYTE_VECTOR_COLOR_SPEC,
				'id': Constants.ID_COLOR_SPEC,
				'grid_pos': Constants.INT_VECTOR_COLOR_SPEC,
			}[self.get_type()]
		except: return _Param.get_color(self)

	def get_hide(self):
		"""
		Get the hide value from the base class.
		Hide the ID parameter for most blocks. Exceptions below.
		If the parameter controls a port type, vlen, or nports, return part.
		If the parameter is an empty grid position, return part.
		These parameters are redundant to display in the flow graph view.
		@return hide the hide property string
		"""
		hide = _Param.get_hide(self)
		if hide: return hide
		#hide ID in non variable blocks
		if self.get_key() == 'id' and self.get_parent().get_key() not in (
			'variable', 'variable_slider', 'variable_chooser', 'variable_text_box', 'parameter', 'options'
		): return 'part'
		#hide port controllers for type and nports
		if self.get_key() in ' '.join(map(
			lambda p: ' '.join([p._type, p._nports]), self.get_parent().get_ports())
		): return 'part'
		#hide port controllers for vlen, when == 1
		if self.get_key() in ' '.join(map(
			lambda p: p._vlen, self.get_parent().get_ports())
		):
			try:
				assert int(self.evaluate()) == 1
				return 'part'
			except: pass
		#hide empty grid positions
		if self.get_key() == 'grid_pos' and not self.get_value(): return 'part'
		return hide

	def evaluate(self):
		"""
		Evaluate the value.
		@return evaluated type
		"""
		self._lisitify_flag = False
		self._stringify_flag = False
		self._hostage_cells = list()
		def eval_string(v):
			try:
				e = self.get_parent().get_parent().evaluate(v)
				assert isinstance(e, str)
				return e
			except:
				self._stringify_flag = True
				return v
		t = self.get_type()
		v = self.get_value()
		#########################
		# Enum Type
		#########################
		if self.is_enum(): return v
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
				try: assert(isinstance(e, COMPLEX_TYPES))
				except AssertionError:
					self._add_error_message('Expression "%s" is invalid for type complex.'%str(e))
					raise Exception
				return e
			elif t == 'real':
				try: assert(isinstance(e, REAL_TYPES))
				except AssertionError:
					self._add_error_message('Expression "%s" is invalid for type real.'%str(e))
					raise Exception
				return e
			elif t == 'int':
				try: assert(isinstance(e, INT_TYPES))
				except AssertionError:
					self._add_error_message('Expression "%s" is invalid for type integer.'%str(e))
					raise Exception
				return e
			#########################
			# Numeric Vector Types
			#########################
			elif t == 'complex_vector':
				if not isinstance(e, VECTOR_TYPES):
					self._lisitify_flag = True
					e = [e]
				try:
					for ei in e:
						assert(isinstance(ei, COMPLEX_TYPES))
				except AssertionError:
					self._add_error_message('Expression "%s" is invalid for type complex vector.'%str(e))
					raise Exception
				return e
			elif t == 'real_vector':
				if not isinstance(e, VECTOR_TYPES):
					self._lisitify_flag = True
					e = [e]
				try:
					for ei in e:
						assert(isinstance(ei, REAL_TYPES))
				except AssertionError:
					self._add_error_message('Expression "%s" is invalid for type real vector.'%str(e))
					raise Exception
				return e
			elif t == 'int_vector':
				if not isinstance(e, VECTOR_TYPES):
					self._lisitify_flag = True
					e = [e]
				try:
					for ei in e:
						assert(isinstance(ei, INT_TYPES))
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
			try: assert keys.count(v) <= 1 #id should only appear once, or zero times if block is disabled
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
		"""
		Convert the value to code.
		@return a string representing the code
		"""
		#run init tasks in evaluate
		#such as setting flags
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
		"""
		Get all the params from the flowgraph that have the given type.
		@param type the specified type
		@return a list of params
		"""
		return sum([filter(lambda p: p.get_type() == type, block.get_params()) for block in self.get_parent().get_parent().get_enabled_blocks()], [])
