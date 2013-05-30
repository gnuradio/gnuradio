pmt
===

.. automodule:: pmt

Boolean
-------

.. autosummary::
   :nosignatures:

   pmt.is_bool
   pmt.is_true
   pmt.is_false
   pmt.from_bool
   pmt.to_bool

Symbols
-------

.. autosummary::
   :nosignatures:

   pmt.is_symbol
   pmt.string_to_symbol
   pmt.symbol_to_string
   pmt.intern

Numbers
-------

.. autosummary::
   :nosignatures:

   pmt.is_number

Integers
--------

.. autosummary::
   :nosignatures:

   pmt.is_integer
   pmt.to_long
   pmt.from_long

uint64_t
---------

.. autosummary::
   :nosignatures:

   pmt.is_uint64
   pmt.to_uint64
   pmt.from_uint64

Reals
-----

.. autosummary::
   :nosignatures:

   pmt.is_real
   pmt.to_double
   pmt.from_double

Complex
-------

.. autosummary::
   :nosignatures:

   pmt.is_complex
   pmt.to_complex
   pmt.from_complex
   pmt.make_rectangular

Pairs
-----

.. autosummary::
   :nosignatures:

   pmt.is_pair
   pmt.cons
   pmt.car
   pmt.cdr
   pmt.set_car
   pmt.set_cdr
   pmt.caar
   pmt.cadddr
   pmt.caddr
   pmt.cadr
   pmt.cdar
   pmt.cddr

Tuples
------

.. autosummary::
   :nosignatures:

   pmt.is_tuple
   pmt.make_tuple
   pmt.tuple_ref
   pmt.to_tuple

Vectors
-------

.. autosummary::
   :nosignatures:

   pmt.is_vector
   pmt.make_vector
   pmt.vector_ref
   pmt.vector_set
   pmt.vector_fill

BLOBs
-----

.. autosummary::
   :nosignatures:

   pmt.is_blob
   pmt.make_blob
   pmt.blob_data
   pmt.blob_length

Uniform Numeric Vectors
-----------------------

.. autosummary::
   :nosignatures:

   pmt.is_uniform_vector
   pmt.is_u8vector
   pmt.is_u16vector
   pmt.is_u32vector
   pmt.is_u64vector
   pmt.is_s16vector
   pmt.is_s32vector
   pmt.is_s64vector
   pmt.is_s8vector
   pmt.is_c32vector
   pmt.is_c64vector
   pmt.is_f32vector
   pmt.is_f64vector

   pmt.make_u8vector
   pmt.make_u16vector
   pmt.make_u32vector
   pmt.make_u64vector
   pmt.make_s8vector
   pmt.make_s16vector
   pmt.make_s32vector
   pmt.make_s64vector
   pmt.make_c32vector
   pmt.make_c64vector
   pmt.make_f32vector
   pmt.make_f64vector

   pmt.init_u8vector
   pmt.init_u16vector
   pmt.init_u32vector
   pmt.init_u64vector

   pmt.init_s8vector
   pmt.init_s16vector
   pmt.init_s32vector
   pmt.init_s64vector
   pmt.init_c32vector
   pmt.init_c64vector
   pmt.init_f32vector
   pmt.init_f64vector

   pmt.c32vector_ref
   pmt.c64vector_ref
   pmt.f32vector_ref
   pmt.f64vector_ref
   pmt.s16vector_ref
   pmt.s32vector_ref
   pmt.s64vector_ref
   pmt.s8vector_ref
   pmt.u16vector_ref
   pmt.u32vector_ref
   pmt.u64vector_ref
   pmt.u8vector_ref

   pmt.c32vector_set
   pmt.c64vector_set
   pmt.f32vector_set
   pmt.f64vector_set
   pmt.s16vector_set
   pmt.s32vector_set
   pmt.s64vector_set
   pmt.s8vector_set
   pmt.u16vector_set
   pmt.u32vector_set
   pmt.u64vector_set
   pmt.u8vector_set

   pmt.uniform_vector_elements

   pmt.c32vector_elements
   pmt.c64vector_elements
   pmt.f32vector_elements
   pmt.f64vector_elements
   pmt.s16vector_elements
   pmt.s32vector_elements
   pmt.s8vector_elements
   pmt.u16vector_elements
   pmt.u32vector_elements
   pmt.u8vector_elements

Dictionary
----------

.. autosummary::
   :nosignatures:

   pmt.is_dict
   pmt.make_dict
   pmt.dict_add
   pmt.dict_delete
   pmt.dict_has_key
   pmt.dict_ref
   pmt.dict_items
   pmt.dict_keys
   pmt.dict_values

Any
---

.. autosummary::
   :nosignatures:

   pmt.is_any
   pmt.make_any
   pmt.any_ref
   pmt.any_set

Message Accepter
----------------

.. autosummary::
   :nosignatures:

   pmt.is_msg_accepter
   pmt.make_msg_accepter
   pmt.msg_accepter_ref

General Functions
-----------------

.. autosummary::
   :nosignatures:

   pmt.eq
   pmt.equal
   pmt.eqv
   pmt.length
   pmt.assq
   pmt.assv
   pmt_assoc

Lists
-----

.. autosummary::
   :nosignatures:

   pmt.map
   pmt.reverse
   pmt.reverse_x
   pmt.acons
   pmt.nth
   pmt.nthcdr
   pmt.memq
   pmt.memv
   pmt.member
   pmt.subsetp
   pmt.list1
   pmt.list2
   pmt.list3
   pmt.list4
   pmt.list5
   pmt.list6
   pmt.list_add
   pmt.list_rm
   pmt.list_has

Read/Write
----------

.. autosummary::
   :nosignatures:

   pmt.is_eof_object
   pmt.read
   pmt.write
   pmt.write_string

Serialize
---------

.. autosummary::
   :nosignatures:

   pmt.serialize
   pmt.deserialize
   pmt.dump_sizeof
   pmt.serialize_str
   pmt.deserialize_str

Misc
----

.. autosummary::
   :nosignatures: 

   pmt.is_null
   pmt.cvar

Generic Casts
-------------

.. autosummary::
   :nosignatures:

   pmt.to_pmt
   pmt.to_python
   pmt.pmt_to_python.pmt_to_python

Constants
--------

.. autosummary::
   :nosignatures:

   pmt.PMT_EOF
   pmt.PMT_EOF
   pmt.PMT_F
   pmt.PMT_NIL
   pmt.PMT_T

