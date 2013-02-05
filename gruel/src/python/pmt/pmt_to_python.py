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
    v = pmt.make_vector(len(p), pmt.PMT_NIL)
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

def numpy_to_blob(p):
    p = p.view(numpy.uint8)
    b = pmt.make_blob(len(p))
    pmt.blob_data(b)[:] = p
    return b

THE_TABLE = ( #python type, check pmt type, to python, from python
    (None, pmt.is_null, lambda x: None, lambda x: pmt.PMT_NIL),
    (bool, pmt.is_bool, pmt.to_bool, pmt.from_bool),
    (str, pmt.is_symbol, pmt.symbol_to_string, pmt.string_to_symbol),
    (int, pmt.is_integer, pmt.to_long, pmt.from_long),
    (long, pmt.is_uint64, lambda x: long(pmt.to_uint64(x)), pmt.from_uint64),
    (float, pmt.is_real, pmt.to_double, pmt.from_double),
    (complex, pmt.is_complex, pmt.to_complex, pmt.from_complex),
    (tuple, pmt.is_tuple, pmt_to_tuple, pmt_from_tuple),
    (list, pmt.is_vector, pmt_to_vector, pmt_from_vector),
    (dict, pmt.is_dict, pmt_to_dict, pmt_from_dict),
    (numpy.ndarray, pmt.is_blob, pmt.blob_data, numpy_to_blob),
)

def pmt_to_python(p):
    for python_type, pmt_check, to_python, from_python in THE_TABLE:
        if pmt_check(p): return to_python(p)
    return p #give up, we return the same

def python_to_pmt(p):
    for python_type, pmt_check, to_python, from_python in THE_TABLE:
        if python_type is None:
            if p == None: return from_python(p)
        elif isinstance(p, python_type): return from_python(p)
    return p #give up, we return the same

