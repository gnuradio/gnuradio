# Copyright 2012,2013 Free Software Foundation, Inc.
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

try: import pmt_swig as pmt
except: import pmt
import numpy

# SWIG isn't taking in the #define PMT_NIL;
# getting the singleton locally.
PMT_NIL = pmt.get_PMT_NIL()

#define missing
def pmt_to_tuple(p):
    elems = list()
    for i in range(pmt.length(p)):
        elem = pmt.tuple_ref(p, i)
        elems.append(pmt_to_python(elem))
    return tuple(elems)

def pmt_from_tuple(p):
    args = map(python_to_pmt, p)
    return pmt.make_tuple(*args)

def pmt_to_vector(p):
    v = list()
    for i in range(pmt.length(p)):
        elem = pmt.vector_ref(p, i)
        v.append(pmt_to_python(elem))
    return v

def pmt_from_vector(p):
    v = pmt.make_vector(len(p), PMT_NIL)
    for i, elem in enumerate(p):
        pmt.vector_set(v, i, python_to_pmt(elem))
    return v

def pmt_to_dict(p):
    d = dict()
    items = pmt.dict_items(p)
    for i in range(pmt.length(items)):
        pair = pmt.nth(i, items)
        k = pmt.car(pair)
        v = pmt.cdr(pair)
        d[pmt_to_python(k)] = pmt_to_python(v)
    return d

def pmt_from_dict(p):
    d = pmt.make_dict()
    for k, v in p.iteritems():
        #dict is immutable -> therefore pmt_dict_add returns the new dict
        d = pmt.dict_add(d, python_to_pmt(k), python_to_pmt(v))
    return d

numpy_mappings = {
    numpy.dtype(numpy.float32): (pmt.init_f32vector, float, pmt.f32vector_elements, pmt.is_f32vector),
    numpy.dtype(numpy.float64): (pmt.init_f64vector, float, pmt.f64vector_elements, pmt.is_f64vector),
    numpy.dtype(numpy.complex64): (pmt.init_c32vector, complex, pmt.c32vector_elements, pmt.is_c32vector),
    numpy.dtype(numpy.complex128): (pmt.init_c64vector, complex, pmt.c64vector_elements, pmt.is_c64vector),
    numpy.dtype(numpy.int8): (pmt.init_s8vector, int, pmt.s8vector_elements, pmt.is_s8vector),
    numpy.dtype(numpy.int16): (pmt.init_s16vector, int, pmt.s16vector_elements, pmt.is_s16vector),
    numpy.dtype(numpy.int32): (pmt.init_s32vector, int, pmt.s32vector_elements, pmt.is_s32vector),
#    numpy.dtype(numpy.int64): (pmt.init_s64vector, int, pmt.s64vector_elements, pmt.is_s64vector),
    numpy.dtype(numpy.uint8): (pmt.init_u8vector, int, pmt.u8vector_elements, pmt.is_u8vector),
    numpy.dtype(numpy.uint16): (pmt.init_u16vector, int, pmt.u16vector_elements, pmt.is_u16vector),
    numpy.dtype(numpy.uint32): (pmt.init_u32vector, int, pmt.u32vector_elements, pmt.is_u32vector),
#    numpy.dtype(numpy.uint64): (pmt.init_u64vector, int, pmt.u64vector_elements, pmt.is_u64vector),
    numpy.dtype(numpy.byte): (pmt.init_u8vector, int, pmt.u8vector_elements, pmt.is_u8vector),
}

uvector_mappings = dict([ (numpy_mappings[key][3], (numpy_mappings[key][2], key)) for key in numpy_mappings ])

def numpy_to_uvector(numpy_array):
    try:
        mapping = numpy_mappings[numpy_array.dtype]
        pc = map(mapping[1], numpy.ravel(numpy_array))
        return mapping[0](numpy_array.size, pc)
    except KeyError:
        raise ValueError("unsupported numpy array dtype for conversion to pmt %s"%(numpy_array.dtype))

def uvector_to_numpy(uvector):
	match = None
	for test_func in uvector_mappings.keys():
		if test_func(uvector):
			match = uvector_mappings[test_func]
			return numpy.array(match[0](uvector), dtype = match[1])
	else:
		raise ValueError("unsupported uvector data type for conversion to numpy array %s"%(uvector))

type_mappings = ( #python type, check pmt type, to python, from python
    (None, pmt.is_null, lambda x: None, lambda x: PMT_NIL),
    (bool, pmt.is_bool, pmt.to_bool, pmt.from_bool),
    (str, pmt.is_symbol, pmt.symbol_to_string, pmt.string_to_symbol),
    (unicode, lambda x: False, None, lambda x: pmt.string_to_symbol(x.encode('utf-8'))),
    (int, pmt.is_integer, pmt.to_long, pmt.from_long),
    (long, pmt.is_uint64, lambda x: long(pmt.to_uint64(x)), pmt.from_uint64),
    (float, pmt.is_real, pmt.to_double, pmt.from_double),
    (complex, pmt.is_complex, pmt.to_complex, pmt.from_complex),
    (tuple, pmt.is_tuple, pmt_to_tuple, pmt_from_tuple),
    (list, pmt.is_vector, pmt_to_vector, pmt_from_vector),
    (dict, pmt.is_dict, pmt_to_dict, pmt_from_dict),
    (tuple, pmt.is_pair, lambda x: (pmt_to_python(pmt.car(x)), pmt_to_python(pmt.cdr(x))), lambda x: pmt.cons(python_to_pmt(x[0]), python_to_pmt(x[1]))),
    (numpy.ndarray, pmt.is_uniform_vector, uvector_to_numpy, numpy_to_uvector),
)

def pmt_to_python(p):
    for python_type, pmt_check, to_python, from_python in type_mappings:
        if pmt_check(p):
            try:
                return to_python(p)
            except:
                pass
    raise ValueError("can't convert %s type to pmt (%s)"%(type(p),p))

def python_to_pmt(p):
    for python_type, pmt_check, to_python, from_python in type_mappings:
        if python_type is None:
            if p is None: return from_python(p)
        elif isinstance(p, python_type): return from_python(p)
    raise ValueError("can't convert %s type to pmt (%s)"%(type(p),p))
