"""
Copyright 2008-2015 Free Software Foundation, Inc.
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

import ast

import re
from gnuradio import eng_notation
from gnuradio import gr

import Constants
from Constants import VECTOR_TYPES, COMPLEX_TYPES, REAL_TYPES, INT_TYPES


_check_id_matcher = re.compile('^[a-z|A-Z]\w*$')
_show_id_matcher = re.compile('^(variable\w*|parameter|options|notebook|epy_module)$')

from .odict import odict
from .Element import Element

#blacklist certain ids, its not complete, but should help
import __builtin__
ID_BLACKLIST = ['self', 'options', 'gr', 'blks2', 'wxgui', 'wx', 'math', 'forms', 'firdes'] + \
    filter(lambda x: not x.startswith('_'), dir(gr.top_block())) + dir(__builtin__)

def _get_keys(lst): return [elem.get_key() for elem in lst]
def _get_elem(lst, key):
    try: return lst[_get_keys(lst).index(key)]
    except ValueError: raise ValueError, 'Key "%s" not found in %s.'%(key, _get_keys(lst))

def num_to_str(num):
    """ Display logic for numbers """
    if isinstance(num, COMPLEX_TYPES):
        num = complex(num) #cast to python complex
        if num == 0: return '0' #value is zero
        elif num.imag == 0: return '%s'%eng_notation.num_to_str(num.real) #value is real
        elif num.real == 0: return '%sj'%eng_notation.num_to_str(num.imag) #value is imaginary
        elif num.imag < 0: return '%s-%sj'%(eng_notation.num_to_str(num.real), eng_notation.num_to_str(abs(num.imag)))
        else: return '%s+%sj'%(eng_notation.num_to_str(num.real), eng_notation.num_to_str(num.imag))
    else: return str(num)

class Option(Element):

    def __init__(self, param, n):
        Element.__init__(self, param)
        self._name = n.find('name')
        self._key = n.find('key')
        self._opts = dict()
        opts = n.findall('opt')
        #test against opts when non enum
        if not self.get_parent().is_enum() and opts:
            raise Exception, 'Options for non-enum types cannot have sub-options'
        #extract opts
        for opt in opts:
            #separate the key:value
            try: key, value = opt.split(':')
            except: raise Exception, 'Error separating "%s" into key:value'%opt
            #test against repeated keys
            if self._opts.has_key(key):
                raise Exception, 'Key "%s" already exists in option'%key
            #store the option
            self._opts[key] = value

    def __str__(self): return 'Option %s(%s)'%(self.get_name(), self.get_key())
    def get_name(self): return self._name
    def get_key(self): return self._key

    ##############################################
    # Access Opts
    ##############################################
    def get_opt_keys(self): return self._opts.keys()
    def get_opt(self, key): return self._opts[key]
    def get_opts(self): return self._opts.values()


class Param(Element):

    def __init__(self, block, n):
        """
        Make a new param from nested data.

        Args:
            block: the parent element
            n: the nested odict
        """
        # if the base key is a valid param key, copy its data and overlay this params data
        base_key = n.find('base_key')
        if base_key and base_key in block.get_param_keys():
            n_expanded = block.get_param(base_key)._n.copy()
            n_expanded.update(n)
            n = n_expanded
        # save odict in case this param will be base for another
        self._n = n
        # parse the data
        self._name = n.find('name')
        self._key = n.find('key')
        value = n.find('value') or ''
        self._type = n.find('type') or 'raw'
        self._hide = n.find('hide') or ''
        self._tab_label = n.find('tab') or block.get_param_tab_labels()[0]
        if not self._tab_label in block.get_param_tab_labels():
            block.get_param_tab_labels().append(self._tab_label)
        #build the param
        Element.__init__(self, block)
        #create the Option objects from the n data
        self._options = list()
        self._evaluated = None
        for option in map(lambda o: Option(param=self, n=o), n.findall('option')):
            key = option.get_key()
            #test against repeated keys
            if key in self.get_option_keys():
                raise Exception, 'Key "%s" already exists in options'%key
            #store the option
            self.get_options().append(option)
        #test the enum options
        if self.is_enum():
            #test against options with identical keys
            if len(set(self.get_option_keys())) != len(self.get_options()):
                raise Exception, 'Options keys "%s" are not unique.'%self.get_option_keys()
            #test against inconsistent keys in options
            opt_keys = self.get_options()[0].get_opt_keys()
            for option in self.get_options():
                if set(opt_keys) != set(option.get_opt_keys()):
                    raise Exception, 'Opt keys "%s" are not identical across all options.'%opt_keys
            #if a value is specified, it must be in the options keys
            self._value = value if value or value in self.get_option_keys() else self.get_option_keys()[0]
            if self.get_value() not in self.get_option_keys():
                raise Exception, 'The value "%s" is not in the possible values of "%s".'%(self.get_value(), self.get_option_keys())
        else:
            self._value = value or ''
        self._default = value
        self._init = False
        self._hostage_cells = list()

    def get_types(self):
        return (
            'raw', 'enum',
            'complex', 'real', 'float', 'int',
            'complex_vector', 'real_vector', 'float_vector', 'int_vector',
            'hex', 'string', 'bool',
            'file_open', 'file_save', '_multiline', '_multiline_python_external',
            'id', 'stream_id',
            'grid_pos', 'notebook', 'gui_hint',
            'import',
        )

    def __repr__(self):
        """
        Get the repr (nice string format) for this param.

        Returns:
            the string representation
        """
        ##################################################
        # truncate helper method
        ##################################################
        def _truncate(string, style=0):
            max_len = max(27 - len(self.get_name()), 3)
            if len(string) > max_len:
                if style < 0: #front truncate
                    string = '...' + string[3-max_len:]
                elif style == 0: #center truncate
                    string = string[:max_len/2 -3] + '...' + string[-max_len/2:]
                elif style > 0: #rear truncate
                    string = string[:max_len-3] + '...'
            return string
        ##################################################
        # simple conditions
        ##################################################
        if not self.is_valid(): return _truncate(self.get_value())
        if self.get_value() in self.get_option_keys(): return self.get_option(self.get_value()).get_name()

        ##################################################
        # split up formatting by type
        ##################################################
        truncate = 0 #default center truncate
        e = self.get_evaluated()
        t = self.get_type()
        if isinstance(e, bool): return str(e)
        elif isinstance(e, COMPLEX_TYPES): dt_str = num_to_str(e)
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
        # done
        ##################################################
        return _truncate(dt_str, truncate)

    def __repr2__(self):
        """
        Get the repr (nice string format) for this param.

        Returns:
            the string representation
        """
        if self.is_enum():
            return self.get_option(self.get_value()).get_name()
        return self.get_value()

    def __str__(self): return 'Param - %s(%s)'%(self.get_name(), self.get_key())

    def get_color(self):
        """
        Get the color that represents this param's type.

        Returns:
            a hex color code.
        """
        try:
            return {
                #number types
                'complex': Constants.COMPLEX_COLOR_SPEC,
                'real': Constants.FLOAT_COLOR_SPEC,
                'float': Constants.FLOAT_COLOR_SPEC,
                'int': Constants.INT_COLOR_SPEC,
                #vector types
                'complex_vector': Constants.COMPLEX_VECTOR_COLOR_SPEC,
                'real_vector': Constants.FLOAT_VECTOR_COLOR_SPEC,
                                'float_vector': Constants.FLOAT_VECTOR_COLOR_SPEC,
                'int_vector': Constants.INT_VECTOR_COLOR_SPEC,
                #special
                'bool': Constants.INT_COLOR_SPEC,
                'hex': Constants.INT_COLOR_SPEC,
                'string': Constants.BYTE_VECTOR_COLOR_SPEC,
                'id': Constants.ID_COLOR_SPEC,
                'stream_id': Constants.ID_COLOR_SPEC,
                'grid_pos': Constants.INT_VECTOR_COLOR_SPEC,
                'notebook': Constants.INT_VECTOR_COLOR_SPEC,
                'raw': Constants.WILDCARD_COLOR_SPEC,
            }[self.get_type()]
        except:
            return '#FFFFFF'

    def get_hide(self):
        """
        Get the hide value from the base class.
        Hide the ID parameter for most blocks. Exceptions below.
        If the parameter controls a port type, vlen, or nports, return part.
        If the parameter is an empty grid position, return part.
        These parameters are redundant to display in the flow graph view.

        Returns:
            hide the hide property string
        """
        hide = self.get_parent().resolve_dependencies(self._hide).strip()
        if hide: return hide
        #hide ID in non variable blocks
        if self.get_key() == 'id' and not _show_id_matcher.match(self.get_parent().get_key()): return 'part'
        #hide port controllers for type and nports
        if self.get_key() in ' '.join(map(
            lambda p: ' '.join([p._type, p._nports]), self.get_parent().get_ports())
        ): return 'part'
        #hide port controllers for vlen, when == 1
        if self.get_key() in ' '.join(map(
            lambda p: p._vlen, self.get_parent().get_ports())
        ):
            try:
                if int(self.get_evaluated()) == 1: return 'part'
            except: pass
        #hide empty grid positions
        if self.get_key() in ('grid_pos', 'notebook') and not self.get_value(): return 'part'
        return hide

    def validate(self):
        """
        Validate the param.
        The value must be evaluated and type must a possible type.
        """
        Element.validate(self)
        if self.get_type() not in self.get_types():
            self.add_error_message('Type "%s" is not a possible type.'%self.get_type())

        self._evaluated = None
        try: self._evaluated = self.evaluate()
        except Exception, e: self.add_error_message(str(e))

    def get_evaluated(self): return self._evaluated

    def evaluate(self):
        """
        Evaluate the value.

        Returns:
            evaluated type
        """
        self._init = True
        self._lisitify_flag = False
        self._stringify_flag = False
        self._hostage_cells = list()
        t = self.get_type()
        v = self.get_value()
        #########################
        # Enum Type
        #########################
        if self.is_enum(): return v
        #########################
        # Numeric Types
        #########################
        elif t in ('raw', 'complex', 'real', 'float', 'int', 'hex', 'bool'):
            #raise exception if python cannot evaluate this value
            try: e = self.get_parent().get_parent().evaluate(v)
            except Exception, e: raise Exception, 'Value "%s" cannot be evaluated:\n%s'%(v, e)
            #raise an exception if the data is invalid
            if t == 'raw': return e
            elif t == 'complex':
                if not isinstance(e, COMPLEX_TYPES):
                    raise Exception, 'Expression "%s" is invalid for type complex.'%str(e)
                return e
            elif t == 'real' or t == 'float':
                if not isinstance(e, REAL_TYPES):
                    raise Exception, 'Expression "%s" is invalid for type float.'%str(e)
                return e
            elif t == 'int':
                if not isinstance(e, INT_TYPES):
                    raise Exception, 'Expression "%s" is invalid for type integer.'%str(e)
                return e
            elif t == 'hex': return hex(e)
            elif t == 'bool':
                if not isinstance(e, bool):
                    raise Exception, 'Expression "%s" is invalid for type bool.'%str(e)
                return e
            else: raise TypeError, 'Type "%s" not handled'%t
        #########################
        # Numeric Vector Types
        #########################
        elif t in ('complex_vector', 'real_vector', 'float_vector', 'int_vector'):
            if not v: v = '()' #turn a blank string into an empty list, so it will eval
            #raise exception if python cannot evaluate this value
            try: e = self.get_parent().get_parent().evaluate(v)
            except Exception, e: raise Exception, 'Value "%s" cannot be evaluated:\n%s'%(v, e)
            #raise an exception if the data is invalid
            if t == 'complex_vector':
                if not isinstance(e, VECTOR_TYPES):
                    self._lisitify_flag = True
                    e = [e]
                if not all([isinstance(ei, COMPLEX_TYPES) for ei in e]):
                    raise Exception, 'Expression "%s" is invalid for type complex vector.'%str(e)
                return e
            elif t == 'real_vector' or t == 'float_vector':
                if not isinstance(e, VECTOR_TYPES):
                    self._lisitify_flag = True
                    e = [e]
                if not all([isinstance(ei, REAL_TYPES) for ei in e]):
                    raise Exception, 'Expression "%s" is invalid for type float vector.'%str(e)
                return e
            elif t == 'int_vector':
                if not isinstance(e, VECTOR_TYPES):
                    self._lisitify_flag = True
                    e = [e]
                if not all([isinstance(ei, INT_TYPES) for ei in e]):
                    raise Exception, 'Expression "%s" is invalid for type integer vector.'%str(e)
                return e
        #########################
        # String Types
        #########################
        elif t in ('string', 'file_open', 'file_save', '_multiline', '_multiline_python_external'):
            #do not check if file/directory exists, that is a runtime issue
            try:
                e = self.get_parent().get_parent().evaluate(v)
                if not isinstance(e, str):
                    raise Exception()
            except:
                self._stringify_flag = True
                e = str(v)
            if t == '_multiline_python_external':
                ast.parse(e)  # raises SyntaxError
            return e
        #########################
        # Unique ID Type
        #########################
        elif t == 'id':
            #can python use this as a variable?
            if not _check_id_matcher.match(v):
                raise Exception, 'ID "%s" must begin with a letter and may contain letters, numbers, and underscores.'%v
            ids = [param.get_value() for param in self.get_all_params(t)]
            if ids.count(v) > 1: #id should only appear once, or zero times if block is disabled
                raise Exception, 'ID "%s" is not unique.'%v
            if v in ID_BLACKLIST:
                raise Exception, 'ID "%s" is blacklisted.'%v
            return v
        #########################
        # Stream ID Type
        #########################
        elif t == 'stream_id':
            #get a list of all stream ids used in the virtual sinks
            ids = [param.get_value() for param in filter(
                lambda p: p.get_parent().is_virtual_sink(),
                self.get_all_params(t),
            )]
            #check that the virtual sink's stream id is unique
            if self.get_parent().is_virtual_sink():
                if ids.count(v) > 1: #id should only appear once, or zero times if block is disabled
                    raise Exception, 'Stream ID "%s" is not unique.'%v
            #check that the virtual source's steam id is found
            if self.get_parent().is_virtual_source():
                if v not in ids:
                    raise Exception, 'Stream ID "%s" is not found.'%v
            return v
        #########################
        # GUI Position/Hint
        #########################
        elif t == 'gui_hint':
            if ':' in v: tab, pos = v.split(':')
            elif '@' in v: tab, pos = v, ''
            else: tab, pos = '', v

            if '@' in tab: tab, index = tab.split('@')
            else: index = '?'

            widget_str = ({
                (True, True): 'self.%(tab)s_grid_layout_%(index)s.addWidget(%(widget)s, %(pos)s)',
                (True, False): 'self.%(tab)s_layout_%(index)s.addWidget(%(widget)s)',
                (False, True): 'self.top_grid_layout.addWidget(%(widget)s, %(pos)s)',
                (False, False): 'self.top_layout.addWidget(%(widget)s)',
            }[bool(tab), bool(pos)])%{'tab': tab, 'index': index, 'widget': '%s', 'pos': pos}

            # FIXME: Move replace(...) into the make template of the qtgui blocks and return a string here
            class GuiHint(object):
                def __init__(self, ws):
                    self._ws = ws

                def __call__(self, w):
                    return (self._ws.replace('addWidget', 'addLayout') if 'layout' in w else self._ws) % w

                def __str__(self):
                    return self._ws
            return GuiHint(widget_str)
        #########################
        # Grid Position Type
        #########################
        elif t == 'grid_pos':
            if not v: return '' #allow for empty grid pos
            e = self.get_parent().get_parent().evaluate(v)
            if not isinstance(e, (list, tuple)) or len(e) != 4 or not all([isinstance(ei, int) for ei in e]):
                raise Exception, 'A grid position must be a list of 4 integers.'
            row, col, row_span, col_span = e
            #check row, col
            if row < 0 or col < 0:
                raise Exception, 'Row and column must be non-negative.'
            #check row span, col span
            if row_span <= 0 or col_span <= 0:
                raise Exception, 'Row and column span must be greater than zero.'
            #get hostage cell parent
            try: my_parent = self.get_parent().get_param('notebook').evaluate()
            except: my_parent = ''
            #calculate hostage cells
            for r in range(row_span):
                for c in range(col_span):
                    self._hostage_cells.append((my_parent, (row+r, col+c)))
            #avoid collisions
            params = filter(lambda p: p is not self, self.get_all_params('grid_pos'))
            for param in params:
                for parent, cell in param._hostage_cells:
                    if (parent, cell) in self._hostage_cells:
                        raise Exception, 'Another graphical element is using parent "%s", cell "%s".'%(str(parent), str(cell))
            return e
        #########################
        # Notebook Page Type
        #########################
        elif t == 'notebook':
            if not v: return '' #allow for empty notebook
            #get a list of all notebooks
            notebook_blocks = filter(lambda b: b.get_key() == 'notebook', self.get_parent().get_parent().get_enabled_blocks())
            #check for notebook param syntax
            try: notebook_id, page_index = map(str.strip, v.split(','))
            except: raise Exception, 'Bad notebook page format.'
            #check that the notebook id is valid
            try: notebook_block = filter(lambda b: b.get_id() == notebook_id, notebook_blocks)[0]
            except: raise Exception, 'Notebook id "%s" is not an existing notebook id.'%notebook_id
            #check that page index exists
            if int(page_index) not in range(len(notebook_block.get_param('labels').evaluate())):
                raise Exception, 'Page index "%s" is not a valid index number.'%page_index
            return notebook_id, page_index
        #########################
        # Import Type
        #########################
        elif t == 'import':
            n = dict() #new namespace
            try: exec v in n
            except ImportError: raise Exception, 'Import "%s" failed.'%v
            except Exception: raise Exception, 'Bad import syntax: "%s".'%v
            return filter(lambda k: str(k) != '__builtins__', n.keys())
        #########################
        else: raise TypeError, 'Type "%s" not handled'%t

    def to_code(self):
        """
        Convert the value to code.
        For string and list types, check the init flag, call evaluate().
        This ensures that evaluate() was called to set the xxxify_flags.

        Returns:
            a string representing the code
        """
        v = self.get_value()
        t = self.get_type()
        if t in ('string', 'file_open', 'file_save', '_multiline', '_multiline_python_external'):  # string types
            if not self._init: self.evaluate()
            if self._stringify_flag: return '"%s"'%v.replace('"', '\"')
            else: return v
        elif t in ('complex_vector', 'real_vector', 'float_vector', 'int_vector'): #vector types
            if not self._init: self.evaluate()
            if self._lisitify_flag: return '(%s, )'%v
            else: return '(%s)'%v
        else: return v

    def get_all_params(self, type):
        """
        Get all the params from the flowgraph that have the given type.

        Args:
            type: the specified type

        Returns:
            a list of params
        """
        return sum([filter(lambda p: p.get_type() == type, block.get_params()) for block in self.get_parent().get_parent().get_enabled_blocks()], [])

    def is_enum(self): return self._type == 'enum'

    def get_value(self):
        value = self._value
        if self.is_enum() and value not in self.get_option_keys():
            value = self.get_option_keys()[0]
            self.set_value(value)
        return value

    def set_value(self, value): self._value = str(value) #must be a string

    def value_is_default(self):
        return self._default == self._value

    def get_type(self): return self.get_parent().resolve_dependencies(self._type)
    def get_tab_label(self): return self._tab_label

    def is_param(self): return True
    def get_name(self): return self.get_parent().resolve_dependencies(self._name).strip()
    def get_key(self): return self._key

    ##############################################
    # Access Options
    ##############################################
    def get_option_keys(self): return _get_keys(self.get_options())
    def get_option(self, key): return _get_elem(self.get_options(), key)
    def get_options(self): return self._options

    ##############################################
    # Access Opts
    ##############################################
    def get_opt_keys(self): return self.get_option(self.get_value()).get_opt_keys()
    def get_opt(self, key): return self.get_option(self.get_value()).get_opt(key)
    def get_opts(self): return self.get_option(self.get_value()).get_opts()

    ##############################################
    ## Import/Export Methods
    ##############################################
    def export_data(self):
        """
        Export this param's key/value.

        Returns:
            a nested data odict
        """
        n = odict()
        n['key'] = self.get_key()
        n['value'] = self.get_value()
        return n
