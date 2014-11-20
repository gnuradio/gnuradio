"""
Copyright 2014 Free Software Foundation, Inc.
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

from __future__ import absolute_import, division, print_function

import numpy as np

from . platform import Platform


class PortDType(object):

    def __init__(self, label, sizeof, color, names):
        self.label = label
        self.sizeof = sizeof
        self.color = color  # ToDo: remove color
        self.names = names if isinstance(names, (list, set)) else (names,)

    @classmethod
    def register(cls, *args, **kwargs):
        Platform.register_port_dtype(cls(*args, **kwargs))


# build-in types
PortDType.register('Complex Float 64',   16, '#CC8C69', 'fc64')
PortDType.register('Complex Float 32',    8, '#3399FF', ('fc64', 'complex', complex))

PortDType.register('Float 64',            8, '#66CCCC', 'f64')
PortDType.register('Float 32',            4, '#FF8C69', ('f32', 'float', float))

PortDType.register('Complex Integer 64', 16, '#66CC00', 'sc64')
PortDType.register('Complex Integer 32',  8, '#33cc66', 'sc32')
PortDType.register('Complex Integer 16',  4, '#cccc00', 'sc16')
PortDType.register('Complex Integer 8',   2, '#cc00cc', 'sc8')

PortDType.register('Integer 64',          8, '#99FF33', 's64')
PortDType.register('Integer 32',          4, '#00FF99', ('s32', 'int', int))
PortDType.register('Integer 16',          2, '#FFFF66', ('s16', 'short'))
PortDType.register('Integer 8',           1, '#FF66FF', ('s8',  'byte'))  # uint?


class ParamVType(object):

    def __init__(self, names, valid_types):
        self.names = names if isinstance(names, (list, set)) else (names,)
        self.valid_types = valid_types

    def parse(self, evaluated):
        return evaluated

    def validate(self, evaluated):
        if not isinstance(evaluated, self.valid_types):
            raise TypeError("Expression '{}' is invalid for type {}".format(
                str(evaluated), self.names[0]
            ))

    @classmethod
    def register(cls, *args, **kwargs):
        Platform.register_param_vtype(cls(*args, **kwargs))


class ParamStringVType(ParamVType):

    def parse(self, evaluated):
        if not isinstance(evaluated, str):
            '"{}"'.format(evaluated.replace('"', '\\"'))


class ParamNumericVType(ParamVType):
    pass


class ParamVectorVType(ParamNumericVType):

    valid_vector_types = (tuple, list, set, np.ndarray)

    def __init__(self, names, valid_types):
        super(ParamVectorVType, self).__init__(names, valid_types)

    def parse(self, evaluated):
        if not isinstance(evaluated, self.valid_vector_types):
            evaluated = (evaluated, )
        return evaluated

    def validate(self, evaluated):
        if not all(isinstance(value, self.valid_types) for value in evaluated):
            raise TypeError("Expression '{}' is invalid for type {}".format(
                str(evaluated), self.names[0]
            ))


COMPLEX_TYPES = [complex, np.complex, np.complex64, np.complex128]
REAL_TYPES = [float, np.float, np.float32, np.float64]
INT_TYPES = [int, long, np.int, np.int8, np.int16, np.int32, np.uint64,
    np.uint, np.uint8, np.uint16, np.uint32, np.uint64]
# cast to tuple for isinstance, concatenate subtypes
COMPLEX_TYPES = tuple(COMPLEX_TYPES + REAL_TYPES + INT_TYPES)
REAL_TYPES = tuple(REAL_TYPES + INT_TYPES)
INT_TYPES = tuple(INT_TYPES)


ParamVType.register('bool', bool)
ParamStringVType.register('string', str)

ParamNumericVType.register('complex', COMPLEX_TYPES)
ParamNumericVType.register(('real', 'float'), REAL_TYPES)
ParamNumericVType.register('int', INT_TYPES)

ParamVectorVType.register('complex_vector', COMPLEX_TYPES)
ParamVectorVType.register(('real_vector', 'float_vector'), REAL_TYPES)
ParamVectorVType.register('int_vector', INT_TYPES)

