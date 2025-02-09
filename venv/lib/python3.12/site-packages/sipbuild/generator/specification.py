# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from dataclasses import dataclass, field
from enum import auto, Enum
from typing import Any, Optional, Union

from .scoped_name import ScopedName


class AccessSpecifier(Enum):
    """ The class access specifiers. """

    # Private access.
    PRIVATE = auto()

    # Protected access.
    PROTECTED = auto()

    # Public access.
    PUBLIC = auto()


class ArgumentType(Enum):
    """ The types of either C/C++ or Python arguments.  The numerical values of
    these can occur in generated code so so types must always be appended and
    old (unused) types must never be removed.
    """

    # The type hasn't been specified.
    NONE = 0

    # A user defined type.
    DEFINED = 1

    # A class.
    CLASS = 2

    # A struct.
    STRUCT = 3

    # A void.
    VOID = 4

    # An enum.
    ENUM = 5

    # A template.
    TEMPLATE = 6

    # No longer used.
    SIGNAL_UNUSED = 7

    # No longer used.
    SLOT_UNUSED = 8

    # No longer used.
    RXCON_UNUSED = 9

    # No longer used.
    RXDIS_UNUSED = 10

    # No longer used.
    SLOTCON_UNUSED = 11

    # No longer used.
    SLOTDIS_UNUSED = 12

    # An unsigned char.
    USTRING = 13

    # A char.
    STRING = 14

    # A short.
    SHORT = 15

    # An unsigned short.
    USHORT = 16

    # A constrained int.
    CINT = 17

    # An int.
    INT = 18

    # An unsigned int.
    UINT = 19

    # A long.
    LONG = 20

    # An unsigned long.
    ULONG = 21

    # A float.
    FLOAT = 22

    # A constrained float.
    CFLOAT = 23

    # A double.
    DOUBLE = 24

    # A constrained double.
    CDOUBLE = 25

    # A bool.
    BOOL = 26

    # A mapped type.
    MAPPED = 27

    # A Python object.
    PYOBJECT = 28

    # A Python tuple.
    PYTUPLE = 29

    # A Python list.
    PYLIST = 30

    # A Python dict.
    PYDICT = 31

    # A Python callable.
    PYCALLABLE = 32

    # A Python slice.
    PYSLICE = 33

    # No longer used.
    QOBJECT_UNUSED = 34

    # A function.
    FUNCTION = 35

    # A Python type.
    PYTYPE = 36

    # An ellipsis.
    ELLIPSIS = 37

    # A long long.
    LONGLONG = 38

    # An unsigned long long.
    ULONGLONG = 39

    # No longer used.
    ANYSLOT_UNUSED = 40

    # A constrained bool.
    CBOOL = 41

    # A signed char.
    SSTRING = 42

    # A wchar_t.
    WSTRING = 43

    # A temporary void *.
    FAKE_VOID = 44

    # A Py_ssize_t.
    SSIZE = 45

    # An ASCII encoded string.
    ASCII_STRING = 46

    # A Latin-1 encoded string.
    LATIN1_STRING = 47

    # A UTF-8 encoded string.
    UTF8_STRING = 48

    # A char used as an int.
    BYTE = 49

    # A signed char used as an int.
    SBYTE = 50

    # An unsigned char used as an unsigned int.
    UBYTE = 51

    # A Python capsule.
    CAPSULE = 52

    # A Python object that implements the buffer protocol.
    PYBUFFER = 53

    # A size_t.
    SIZE = 54

    # A Python enum.
    PYENUM = 55

    # A union.
    UNION = 56

    # A Py_hash_t.
    HASH = 57


class ArrayArgument(Enum):
    """ The array support provided by an argument. """

    # /Array/ was specified.
    ARRAY = auto()

    # /ArraySize/ was specified.
    ARRAY_SIZE = auto()

    # The argument provides no array support.
    NONE = auto()


class ClassKey(Enum):
    """ The key that identifies a particular type of class. """

    # A class.
    CLASS = auto()

    # A struct.
    STRUCT = auto()

    # A union.
    UNION = auto()


class DocstringFormat(Enum):
    """ The formatting applied to the text of the docstring. """

    # Any leading spaces common to all non-blank lines in the docstring are
    # removed.
    DEINDENTED = auto()

    # The docstring is used as it is specified in the .sip file.
    RAW = auto()


class DocstringSignature(Enum):
    """ The position of the automatically generated function or method
    signature relative to the docstring text.  In the context of a class's
    docstring then it applies to all the class's ctors.
    """

    # The signature is appended to the docstring.
    APPENDED = auto()

    # The signature is discard.
    DISCARDED = auto()

    # The signature is prepended to the docstring.
    PREPENDED = auto()


class EnumBaseType(Enum):
    """ The different base types fo an enum. """

    # enum.Enum
    ENUM = auto()

    # enum.Flag
    FLAG = auto()

    # enum.IntEnum
    INT_ENUM = auto()

    # enum.IntFlag
    INT_FLAG = auto()

    # enum.IntEnum with unsigned values.
    UINT_ENUM = auto()


class GILAction(Enum):
    """ The action to take with the GIL when calling C/C++ code. """

    # The default action.
    DEFAULT = auto()

    # Hold the GIL.
    HOLD = auto()

    # Release the GIL.
    RELEASE = auto()


class IfaceFileType(Enum):
    """ The type of an interface file. """

    # A class.
    CLASS = auto()

    # An %Exception.
    EXCEPTION = auto()

    # A %MappedType.
    MAPPED_TYPE = auto()

    # A namespace.
    NAMESPACE = auto()


class KwArgs(Enum):
    """ The level of support for passing argument as keyword arguments. """

    # All named arguments can be passed as keyword arguments.
    ALL = auto()

    # Keyword arguments are not supported.
    NONE = auto()

    # All named optional arguments (ie. those with a default value) can be
    # passed as keyword arguments.
    OPTIONAL = auto()


class PyQtMethodSpecifier(Enum):
    """ The PyQt-specific method specifier. """

    # A signal.
    SIGNAL = auto()

    # A slot.
    SLOT = auto()


class PySlot(Enum):
    """ The Python slots corresponding to entries in a type object. """

    # tp_str
    STR = auto()

    # tp_as_number.nb_int
    INT = auto()

    # tp_as_number.nb_float
    FLOAT = auto()

    # tp.as_mapping.mp_length and tp.as_sequence.sq_length
    LEN = auto()

    # tp.as_sequence.sq_contains
    CONTAINS = auto()

    # tp_as_number.nb_add
    ADD = auto()

    # tp.as_sequence.sq_concat
    CONCAT = auto()

    # tp_as_number.nb_subtract
    SUB = auto()

    # tp_as_number.nb_multiply
    MUL = auto()

    # tp.as_sequence.sq_repeat
    REPEAT = auto()

    # tp_as_number.nb_remainder
    MOD = auto()

    # tp_as_number.nb_floor_divide
    FLOORDIV = auto()

    # tp_as_number.nb_true_divide
    TRUEDIV = auto()

    # tp_as_number.nb_and
    AND = auto()

    # tp_as_number.nb_or
    OR = auto()

    # tp_as_number.nb_xor
    XOR = auto()

    # tp_as_number.nb_lshift
    LSHIFT = auto()

    # tp_as_number.nb_rshift
    RSHIFT = auto()

    # tp_as_number.nb_inplace_add
    IADD = auto()

    # tp.as_sequence.sq_inplace_concat
    ICONCAT = auto()

    # tp_as_number.nb_inplace_subtract
    ISUB = auto()

    # tp_as_number.nb_inplace_multiply
    IMUL = auto()

    # tp.as_sequence.sq_inplace_repeat
    IREPEAT = auto()

    # tp_as_number.nb_inplace_remainder
    IMOD = auto()

    # tp_as_number.nb_inplace_floor_divide
    IFLOORDIV = auto()

    # tp_as_number.nb_inplace_true_divide
    ITRUEDIV = auto()

    # tp_as_number.nb_inplace_and
    IAND = auto()

    # tp_as_number.nb_inplace_or
    IOR = auto()

    # tp_as_number.nb_inplace_xor
    IXOR = auto()

    # tp_as_number.nb_inplace_lshift
    ILSHIFT = auto()

    # tp_as_number.nb_inplace_rshift
    IRSHIFT = auto()

    # tp_as_number.nb_invert
    INVERT = auto()

    # tp_call
    CALL = auto()

    # tp.as_mapping.mp_subscript and tp.as_sequence.sq_item
    GETITEM = auto()

    # tp.as_mapping.mp_ass_subscript and tp.as_sequence.sq_ass_item
    SETITEM = auto()

    # tp.as_mapping.mp_ass_subscript and tp.as_sequence.sq_ass_item
    DELITEM = auto()

    # tp_richcompare (Py_LT)
    LT = auto()

    # tp_richcompare (Py_LE)
    LE = auto()

    # tp_richcompare (Py_EQ)
    EQ = auto()

    # tp_richcompare (Py_NE)
    NE = auto()

    # tp_richcompare (Py_GT)
    GT = auto()

    # tp_richcompare (Py_GE)
    GE = auto()

    # tp_as_number.nb_bool
    BOOL = auto()

    # tp_as_number.nb_negative
    NEG = auto()

    # tp_as_number.nb_positive
    POS = auto()

    # tp_as_number.nb_absolute
    ABS = auto()

    # tp_repr
    REPR = auto()

    # tp_hash
    HASH = auto()

    # tp_as_number.nb_index
    INDEX = auto()

    # tp_iter
    ITER = auto()

    # tp_iter_next
    NEXT = auto()

    # tp_setattr
    SETATTR = auto()

    # Internal to the parser (implemented as tp_setattr)
    DELATTR = auto()

    # tp_as_number.nb_matrix_multiply
    MATMUL = auto()

    # tp_as_number.nb_inplace_matrix_multiply
    IMATMUL = auto()

    # tp_as_async.am_await
    AWAIT = auto()

    # tp_as_async.am_aiter
    AITER = auto()

    # tp_as_async.am_anext
    ANEXT = auto()


class QualifierType(Enum):
    """ The type of a qualifier used in %If/%End directives. """

    # The qualifier is a feature.
    FEATURE = auto()

    # The qualifier is a platform.
    PLATFORM = auto()

    # The qualifier is part of a timeline.
    TIME = auto()


class Transfer(Enum):
    """ The different types of ownership transfer. """

    # No transfer of ownership.
    NONE = auto()

    # /Transfer/ was specified.
    TRANSFER = auto()

    # /TransferBack/ was specified.
    TRANSFER_BACK = auto()

    # /TransferThis/ was specified.
    TRANSFER_THIS = auto()


class ValueType(Enum):
    """ The different types of a value in an expression. """

    # A quoted character.
    QCHAR = auto()

    # A string.
    STRING = auto()

    # A number.
    NUMERIC = auto()

    # A floating point number.
    REAL = auto()

    # A scoped name.
    SCOPED = auto()

    # A function call.
    FCALL = auto()

    # A placeholder.
    EMPTY = auto()


@dataclass
class Argument:
    """ Encapsulate a callable argument (or return value or variable type). """

    # The type.
    type: ArgumentType

    # Set if /AllowNone/ was specified.
    allow_none: bool = False

    # The support the argument provides for arrays.
    array: ArrayArgument = ArrayArgument.NONE

    # The optional default value.
    default_value: Optional[list['Value']] = None

    # The optional definition.  What this is depends on the type.
    definition: Any = None

    # The sequence of dereferences.  An element is True if the corresponding
    # dereference is const.
    derefs: list[bool] = field(default_factory=list)

    # Set if /DisallowNone/ was specified.
    disallow_none: bool = False

    # Set if /GetWrapper/ was specified.
    get_wrapper: bool = False

    # Set if the argument is const.
    is_const: bool = False

    # Set if /Constrained/ was specified.
    is_constrained: bool = False

    # Set if the argument is passing a value in.
    is_in: bool = False

    # Set if the argument is passing a value out.
    is_out: bool = False

    # Set if the argument is a reference.
    is_reference: bool = False

    # The key for a reference.  If None then /KeepReference/ wasn't specified.
    key: Optional[int] = None

    # The optional name.
    name: Optional['CachedName'] = None

    # Set if /NoCopy/ was specified.
    no_copy: bool = False

    # The original type if it was a typedef.
    original_typedef: Optional['WrappedTypedef'] = None

    # Set if /ResultSize/ was specified.
    result_size: bool = False

    # The value of /ScopesStripped/.
    scopes_stripped: int = 0

    # The source location.
    source_location: Optional['SourceLocation'] = None

    # Any transfer of ownership.
    transfer: Transfer = Transfer.NONE

    # The non-default type hints.
    type_hints: Optional['TypeHints'] = None

    def __hash__(self):
        """ Reimplemented so an Argument object can be used as a dict key. """

        return id(self)


@dataclass
class CachedName:
    """ Encapsulate a name that may be needed as a string in the generated
    code.
    """

    # The name.
    name: str

    # Set if the name is a substring of another. (resolver)
    is_substring: bool = False

    # The offset of the name in the string pool. (resolver)
    offset: int = 0

    # Set if the name is used in the generated code.
    used: bool = False

    def __str__(self):
        """ Return the string representation. """

        return self.name


@dataclass
class CodeBlock:
    """ Encapsulate a code block, ie. the literal body of a directive. """

    # The name of the .sip file containing the code.
    sip_file: str

    # The line number in the .sip file that the code starts at.
    line_nr: int = 1

    # The text of the code block.
    text: str = ''


@dataclass
class Constructor:
    """ Encapsulate a constructor. """

    # The access specifier.
    access_specifier: AccessSpecifier

    # The Python signature.
    py_signature: 'Signature'

    # The C/C++ signature.  It will be none if /NoDerived/ was specified.
    cpp_signature: Optional['Signature'] = None

    # deprecated message if /Deprecated/ was specified.
    deprecated: Optional[str] = None

    # The docstring.
    docstring: Optional['Docstring'] = None

    # The action required on the GIL.
    gil_action: GILAction = GILAction.DEFAULT

    # Set if the ctor is the implementation of a cast. (resolver)
    is_cast: bool = False

    # The keyword argument support.
    kw_args: KwArgs = KwArgs.NONE

    # The code specified by any %MethodCode directive.
    method_code: Optional[CodeBlock] = None

    # Set if the type hint should be suppressed.
    no_type_hint: bool = False

    # The /PostHook/ name.
    posthook: Optional[str] = None

    # The /PreHook/ name.
    prehook: Optional[str] = None

    # The code specified by any %PreMethodCode directive.
    premethod_code: Optional[CodeBlock] = None

    # Set if a Python exception is raised.
    raises_py_exception: bool = False

    # The optional throw arguments.  Replace with 'noexcept' in SIP v7.
    throw_args: Optional['ThrowArguments'] = None

    # Any transfer of ownership.
    transfer: Transfer = Transfer.NONE


@dataclass
class Docstring:
    """ Encapsulate a docstring. """

    # The position of the automatically generated signature.
    signature: DocstringSignature

    # The text of the docstring.
    text: str


@dataclass
class Extract:
    """ Encapsulate a part of an extract. """

    # The ID of the extract.
    id: str

    # The order.  A negative value implies the part is appended to the extract.
    order: int

    # The text of the extract part.
    text: str


@dataclass
class FunctionCall:
    """ Encapsulate a call to a function in an expression. """

    # The type of the result.
    result: Argument

    # The list of arguments.
    args: list[list['Value']]


@dataclass
class IfaceFile:
    """ Encapsulate an interface file, ie. a generated source file. """

    # The type.
    type: IfaceFileType

    # The C/C++ name.  It will be None if the interface file relates to a
    # template.  Note that this is fully qualified and so overlaps with
    # 'fq_cpp_name'.
    cpp_name: Optional[CachedName] = None

    # The filename extension.
    file_extension: Optional[str] = None

    # The fully qualified C/C++ name.  It will be None if the interface file
    # relates to a template.
    fq_cpp_name: Optional[ScopedName] = None

    # The generated type number. (resolver)
    type_nr: int = -1

    # The defining module.  It will be None if the interface file relates to a
    # template.
    module: Optional['Module'] = None

    # Set if this interface file is needed by the module for which code is to
    # be generated.
    needed: bool = False

    # The %TypeHeaderCode.
    type_header_code: list[CodeBlock] = field(default_factory=list)

    # The interface files used by this one (either directly or indirectly).
    used: list['IfaceFile'] = field(default_factory=list)


@dataclass
class License:
    """ Encapsulate a license. """

    # The type of the license.
    type: str

    # The licensee.
    licensee: Optional[str] = None

    # The timestamp.
    timestamp: Optional[str] = None

    # The signature.
    signature: Optional[str] = None


@dataclass
class MappedType:
    """ Encapsulate a mapped type. """

    # The interface file.
    iface_file: IfaceFile

    # The type.
    type: Argument

    # The %ConvertFromTypeCode.
    convert_from_type_code: Optional[CodeBlock] = None

    # The %ConvertToTypeCode.
    convert_to_type_code: Optional[CodeBlock] = None

    # The C/C++ name.  It will be None for mapped type templates.
    cpp_name: Optional[CachedName] = None

    # Set if /AllowNone/ was specified.
    handles_none: bool = False

    # The %InstanceCode.
    instance_code: Optional[CodeBlock] = None

    # The member functions.
    members: list['Member'] = field(default_factory=list)

    # Set if the handwritten code requires user state information.
    needs_user_state: bool = False

    # Set if /NoAssignmentOperator/ was specified.
    no_assignment_operator: bool = False

    # Set if /NoCopyCtor/ was specified.
    no_copy_ctor: bool = False

    # Set if /NoDefaultCtor/ was specified.
    no_default_ctor: bool = False

    # Set if /NoRelease/ was specified.
    no_release: bool = False

    # The overloaded member functions.
    overloads: list['Overload'] = field(default_factory=list)

    # The Python name.  It will be None for mapped type templates.
    py_name: Optional[CachedName] = None

    # The /PyQtFlags/.
    pyqt_flags: int = 0

    # The %ReleaseCode.
    release_code: Optional[CodeBlock] = None

    # The %TypeCode.
    type_code: list[CodeBlock] = field(default_factory=list)

    # The %TypeHintCode.
    type_hint_code: Optional[CodeBlock] = None

    # The type hints.
    type_hints: Optional['TypeHints'] = None


@dataclass
class MappedTypeTemplate:
    """ Encapsulate a mapped type template. """

    # The prototype mapped type.
    mapped_type: MappedType

    # The template arguments.
    signature: 'Signature'


@dataclass
class Member:
    """ Encapsulate a member function. """

    # The defining module.
    module: 'Module'

    # The Python name.
    py_name: CachedName

    # Set if keyword arguments are allowed.
    allow_keyword_args: bool = False

    # Set if at least one of the overloads is protected.
    has_protected: bool = False

    # Set if /Numeric/ was specified.
    is_numeric: bool = False

    # Set if /Sequence/ was specified.
    is_sequence: bool = False

    # The number of the member. (outputter)
    member_nr: int = -1

    # The original interface file if the function was defined in a namespace.
    namespace_iface_file: Optional[IfaceFile] = None

    # Set if /NoArgParser/ was specified.
    no_arg_parser: bool = False

    # The Python slot if it is not an ordinary member function.
    py_slot: Optional[PySlot] = None


@dataclass
class Module:
    """ Encapsulate a module. """

    # The list of all (explicit and implied) imports. (resolver)
    all_imports: list['Module'] = field(default_factory=list)

    # Set if wrapped ctors should support cooperative multi-inheritance.
    call_super_init: bool = False

    # The text specified by any %Copying directives.
    copying: list[CodeBlock] = field(default_factory=list)

    # The default docstring format.
    default_docstring_format: DocstringFormat = DocstringFormat.RAW

    # The default docstring signature position.
    default_docstring_signature: DocstringSignature = DocstringSignature.DISCARDED

    # The default exception.
    default_exception: Optional['WrappedException'] = None

    # The default meta-type.
    default_metatype: Optional[CachedName] = None

    # The default super-type.
    default_supertype: Optional[CachedName] = None

    # The handler called when a Python re-implementation of a virtual C++
    # function raises an exception.
    default_virtual_error_handler: Optional[str] = None

    # The module's docstring.
    docstring: Optional[Docstring] = None

    # The fully qualified name of the module.  It is only None until %Module is
    # specified.
    fq_py_name: Optional[CachedName] = None

    # The global functions.
    global_functions: list[Member] = field(default_factory=list)

    # Set if any class defined in the module has a delayed dtor.
    has_delayed_dtors: bool = False

    # The list of direct imports.
    imports: list['Module'] = field(default_factory=list)

    # The code specified by any %InitialisationCode directives.
    initialisation_code: list[CodeBlock] = field(default_factory=list)

    # The software license.
    license: Optional[License] = None

    # The %ModuleCode.
    module_code: list[CodeBlock] = field(default_factory=list)

    # The %ModuleHeaderCode.
    module_header_code: list[CodeBlock] = field(default_factory=list)

    # The next key to auto-allocate.
    next_key: int = -1

    # The number of exceptions defined in this module. (resolver)
    nr_exceptions: int = 0

    # The generated types needed by this module. (resolver)
    needed_types: list[Argument] = field(default_factory=list)

    # The number of typedefs defined in this module.
    nr_typedefs: int = 0

    # The number of virtual error handlers defined in this module. (resolver)
    nr_virtual_error_handlers: int = 0

    # The overloaded global functions.
    overloads: list['Overload'] = field(default_factory=list)

    # The code specified by any %PostInitialisationCode directives.
    postinitialisation_code: list[CodeBlock] = field(default_factory=list)

    # The proxy classes.
    proxies: list['WrappedClass'] = field(default_factory=list)

    # The code specified by any %PreInitialisationCode directives.
    preinitialisation_code: list[CodeBlock] = field(default_factory=list)

    # The name of the module. (resolver)
    py_name: Optional[str] = None

    # Set if the generated bindings are Py_ssize_t clean.
    py_ssize_t_clean: bool = False

    # The list of qualifiers.
    qualifiers: list['Qualifier'] = field(default_factory=list)

    # The %TypeHintCode.
    type_hint_code: list[CodeBlock] = field(default_factory=list)

    # The %UnitCode.
    unit_code: list[CodeBlock] = field(default_factory=list)

    # The %UnitPostIncludeCode.
    unit_postinclude_code: list[CodeBlock] = field(default_factory=list)

    # Set if the actual argument names to wrapped callables should be used in
    # the generated bindings rather than automatically generated ones.
    use_arg_names: bool = False

    # Set if the generated bindings should only use the limited Python API.
    use_limited_api: bool = False

    # The interface files used by the module.
    used: list[IfaceFile] = field(default_factory=list)


@dataclass
class Overload:
    """ Encapsulate an overloaded member function. """

    # The access specifier.
    access_specifier: Optional[AccessSpecifier]

    # The member common to all overloads.
    common: Member

    # The C/C++ name if not a operator/slot.
    cpp_name: str

    # The C/C++ signature.
    cpp_signature: 'Signature'

    # The Python signature.
    py_signature: 'Signature'

    # Set if /AbortOnException/ is specified.
    abort_on_exception: bool = False

    # Set if the overload is really protected.
    access_is_really_protected: bool = False

    # deprecated message if /Deprecated/ was specified.
    deprecated: Optional[str] = None
    
    # The docstring.
    docstring: Optional[Docstring] = None

    # Set if /Factory/ was specified.
    factory: bool = False

    # The action required on the GIL.
    gil_action: GILAction = GILAction.DEFAULT

    # Set if the overload is abstract.
    is_abstract: bool = False

    # Set if /AutoGen/ was specified and the associated feature was enabled.
    is_auto_generated: bool = False

    # Set if the overload is a complementary slot. (resolver)
    is_complementary: bool = False

    # Set if the overload is const.
    is_const: bool = False

    # Set if the overload implements __delattr__ (as opposed to __setattr__).
    is_delattr: bool = False

    # Set if the overload is final.
    is_final: bool = False

    # Set if the C++ overload is global. (resolver)
    is_global: bool = False

    # Set if self should not be dereferenced. (resolver)
    dont_deref_self: bool = False

    # Set if the overload is a reflected slot. (resolver)
    is_reflected: bool = False

    # Set if the overload is static.
    is_static: bool = False

    # Set if the overload is virtual.
    is_virtual: bool = False

    # Set if the overload is a virtual reimplementation. (resolver)
    is_virtual_reimplementation: bool = False

    # The keyword argument support.
    kw_args: KwArgs = KwArgs.NONE

    # The code specified by any %MethodCode directive.
    method_code: Optional[CodeBlock] = None

    # Set if /NewThread/ was specified.
    new_thread: bool = False

    # Set if the type hint should be suppressed.
    no_type_hint: bool = False

    # Set if any virtual error handler should be ignored.
    no_virtual_error_handler: bool = False

    # The /PostHook/ name.
    posthook: Optional[str] = None

    # The /PreHook/ name.
    prehook: Optional[str] = None

    # The code specified by any %PreMethodCode directive.
    premethod_code: Optional[CodeBlock] = None

    # The PyQt method specifier.
    pyqt_method_specifier: Optional[PyQtMethodSpecifier] = None

    # Set if a Python exception is raised.
    raises_py_exception: bool = False

    # The source location.
    source_location: Optional['SourceLocation'] = None

    # The optional throw arguments.  Replace with 'noexcept' in SIP v7.
    throw_args: Optional['ThrowArguments'] = None

    # Any transfer of ownership.
    transfer: Transfer = Transfer.NONE

    # The code specified by any %VirtualCallCode directive.
    virtual_call_code: Optional[CodeBlock] = None

    # The code specified by any %VirtualCatcherCode directive.
    virtual_catcher_code: Optional[CodeBlock] = None

    # The name of the virtual error handler to use.
    virtual_error_handler: Optional[str] = None


@dataclass
class Property:
    """ Encapsulate a property. """

    # The name of the getter.
    getter: str

    # The name.
    name: CachedName

    # The docstring.
    docstring: Optional[Docstring] = None

    # The name of the optional setter.
    setter: Optional[str] = None


@dataclass
class Qualifier:
    """ Encapsulate a qualifier used in %If/%End directives. """

    # The defining module.
    module: 'Module'

    # The name of the qualifier.
    name: str

    # The type of the qualifier
    type: QualifierType

    # Set if the qualifier is enabled by default.
    enabled_by_default: bool = False

    # The order if it is a TIME qualifier (ie. the position within the
    # timeline).
    order: int = 0

    # The timeline number within the defining module if it is a TIME qualifier.
    # A negative value implies the SIP timeline.
    timeline: int = -1


@dataclass
class Signature:
    """ Encapsulate a signature (including the optional result). """

    # The list of arguments.
    args: list[Argument] = field(default_factory=list)

    # The type of the result.
    result: Optional[Argument] = None


@dataclass
class SourceLocation:
    """ Encapsulate a location in a .sip source file. """

    # The .sip file name.
    sip_file: str

    # The column number.
    column: int = 0

    # The line number.
    line: int = 0


@dataclass
class Specification:
    """ Encapsulate a parsed .sip file. """

    # The version of the ABI being targeted.
    abi_version: tuple

    # Set if the specification is strict.
    is_strict: bool

    # The fully qualified name of the sip module.  If it is None then there is
    # no shared sip module.
    sip_module: Optional[str]

    # Set if the bindings are for C rather than C++.
    c_bindings: bool = False

    # The list of classes.
    classes: list['WrappedClass'] = field(default_factory=list)

    # The list of enums.
    enums: list['WrappedEnum'] = field(default_factory=list)

    # The list of exceptions.
    exceptions: list['WrappedException'] = field(default_factory=list)

    # The %ExportedHeaderCode.
    exported_header_code: list[CodeBlock] = field(default_factory=list)

    # The %ExportedTypeHintCode.
    exported_type_hint_code: list[CodeBlock] = field(default_factory=list)

    # The extracts.
    extracts: list[Extract] = field(default_factory=list)

    # The interface files.
    iface_files: list[IfaceFile] = field(default_factory=list)

    # Set if the specification is for a composite module.
    is_composite: bool = False

    # The mapped type templates.
    mapped_type_templates: list[MappedTypeTemplate] = field(default_factory=list)

    # The mapped types.
    mapped_types: list[MappedType] = field(default_factory=list)

    # The module for which code is to be generated.
    module: Module = field(default_factory=Module)

    # The cache of names that may be required as strings in the generated code.
    name_cache: dict[int, list[CachedName]] = field(default_factory=dict)

    # The number of virtual handlers. (resolver)
    nr_virtual_handlers: int = 0

    # The list of plugins.  Note that these are PyQt-specific and will be
    # removed in SIP v7.
    plugins: list[str] = field(default_factory=list)

    # The QObject class.
    pyqt_qobject: Optional['WrappedClass'] = None

    # The list of typedefs.
    typedefs: list['WrappedTypedef'] = field(default_factory=list)

    # The list of variables.
    variables: list['WrappedVariable'] = field(default_factory=list)

    # The list of virtual error handlers.
    virtual_error_handlers: list['VirtualErrorHandler'] = field(default_factory=list)

    # The list of virtual handlers. (resolver)
    virtual_handlers: list['VirtualHandler'] = field(default_factory=list)

    def __hash__(self):
        """ Reimplemented so a Specification object can be used as a dict key.
        """

        return id(self)


@dataclass
class Template:
    """ Encapsulate a template. """

    # The C++ name.
    cpp_name: ScopedName

    # The types.
    types: Signature


@dataclass
class ThrowArguments:
    """ Encapsulate the arguments to a C++ throw(). """

    # The list of the argument names. If it is None then 'noexcept' was
    # specified, otherwise there will be at least one argument.
    arguments: Optional[list['WrappedException']] = None


@dataclass
class TypeHints:
    """ Encapsulate a set of PEP 484 type hints for a type. """

    # The type hint when used to pass a value into a callable.
    hint_in: Optional[str]

    # The type hint used to return a value from a callable.
    hint_out: Optional[str]

    # The representation of a default value in a type hint.
    default_value: Optional[str]


@dataclass
class Value:
    """ Encapsulate a literal value. """

    # The type of the value.
    value_type: ValueType

    # Any literal value.
    value: Optional[Union[str, int, float, FunctionCall, ScopedName]]

    # Any binary operator.
    binary_operator: Optional[str] = None

    # Any cast.
    cast: Optional[ScopedName] = None

    # Any unary operator.
    unary_operator: Optional[str] = None


@dataclass
class VirtualErrorHandler:
    """ Encapsulate a virtual error handler. """

    # The code implementing the handler.
    code: CodeBlock

    # The defining module.
    module: Module

    # The name of the handler.
    name: str

    # The number of the handler. (resolver)
    handler_nr: int = -1


@dataclass
class VirtualHandler:
    """ Encapsulate a virtual overload handler. (resolver) """

    # The C/C++ signature.
    cpp_signature: Signature

    # The Python signature.
    py_signature: Signature

    # The code specified by any %VirtualCatcherCode directive.
    virtual_catcher_code: Optional[CodeBlock]

    # The virtual error handler.
    virtual_error_handler: VirtualErrorHandler

    # Set if execution should abort if there is an exception.
    abort_on_exception: bool = False

    # The number of the handler.
    handler_nr: int = -1

    # Set if ownership of the result should be transferred.
    transfer_result: bool = False


@dataclass
class VirtualOverload:
    """ Encapsulate a virtual overloaded member function. (resolver) """

    # The overload
    overload: Overload

    # The handler for the overload.  It is only set for the module for which
    # code is being generated.
    handler: Optional[VirtualHandler]


@dataclass
class VisibleMember:
    """ Encapsulate a visible member function. (resolver) """

    # The member function.
    member: Member

    # The defining class.
    scope: 'WrappedClass'


@dataclass
class WrappedClass:
    """ Encapsulate a wrapped C/C++ namespace/class/struct/union. """

    # The interface file.
    iface_file: IfaceFile

    # The Python name.
    py_name: CachedName

    # The enclosing scope.
    scope: Optional['WrappedClass']

    # The %BIGetBufferCode.
    bi_get_buffer_code: Optional[CodeBlock] = None

    # The %BIReleaseBufferCode.
    bi_release_buffer_code: Optional[CodeBlock] = None

    # Set if the class has usable constructors.
    can_create: bool = False

    # Set if an instance of the class cannot be assigned.
    cannot_assign: bool = False

    # Set if an instance of the class cannot be copied. (resolver)
    cannot_copy: bool = False

    # The list of operator casts.
    casts: list[Argument] = field(default_factory=list)

    # The specific type of class.  It will be None for namespaces.
    class_key: Optional[ClassKey] = None

    # The %ConvertFromTypeCode.
    convert_from_type_code: Optional[CodeBlock] = None

    # The %ConvertToSubClassCode.
    convert_to_subclass_code: Optional[CodeBlock] = None

    # The %ConvertToTypeCode.
    convert_to_type_code: Optional[CodeBlock] = None

    # The constructors.
    ctors: list[Constructor] = field(default_factory=list)

    # The dtor's %PreMethodCode and %MethodCode.
    dealloc_code: list[CodeBlock] = field(default_factory=list)

    # The constructor that has /Default/ specified.
    default_ctor: Optional[Constructor] = None

    # Set if /DelayDtor/ was specified.
    delay_dtor: bool = False

    # deprecated message if /Deprecated/ was specified.
    deprecated: Optional[str] = None

    # The docstring.
    docstring: Optional[Docstring] = None

    # The access specifier of any dtor.
    dtor: Optional[AccessSpecifier] = None

    # The action required on the GIL.
    dtor_gil_action: GILAction = GILAction.DEFAULT

    # The optional dtor throw arguments.  Replace with 'noexcept' in SIP v7.
    dtor_throw_args: Optional[ThrowArguments] = None

    # The code specified by any dtor %VirtualCatcherCode directive.
    dtor_virtual_catcher_code: Optional[CodeBlock] = None

    # Set if /ExportDerived/ was specified.
    export_derived: bool = False

    # Set if /External/ was specified.
    external: bool = False

    # The %FinalisationCode.
    finalisation_code: Optional[CodeBlock] = None

    # The %GCClearCode.
    gc_clear_code: Optional[CodeBlock] = None

    # The %GCTraverseCode.
    gc_traverse_code: Optional[CodeBlock] = None

    # Set if /AllowNone/ was specified.
    handles_none: bool = False

    # Set if the class has a non-lazy method.
    has_nonlazy_method: bool = False

    # Set if the class actually has a shadow (ie. derived) class. (resolver)
    has_shadow: bool = False

    # Set if the class has variables that need handlers. (resolver)
    has_variable_handlers: bool = False

    # The %InstanceCode.
    instance_code: Optional[CodeBlock] = None

    # Set if the class is abstract.
    is_abstract: bool = False

    # Set if the class is a hidden namespace.
    is_hidden_namespace: bool = False

    # Set if the class is incomplete.
    is_incomplete: bool = False

    # Set if the class is opaque.
    is_opaque: bool = False

    # Set if the class is defined in a protected section.
    is_protected: bool = False

    # Set if the class is QObject or a sub-class. (resolver)
    is_qobject: bool = False

    # The C++ name of any overload annotated with __len__.
    len_cpp_name: Optional[str] = None

    # The methods.
    members: list[Member] = field(default_factory=list)

    # The value of /Metatype/ if specified.
    metatype: Optional[CachedName] = None

    # Set if /Mixin/ was specified.
    mixin: bool = False

    # The list of all classes in the class hierarchy starting with itself.
    # (resolver)
    mro: list['WrappedClass'] = field(default_factory=list)

    # Set if the class needs an array helper. (resolver)
    needs_array_helper: bool = False

    # Set if the class needs a copy helper. (resolver)
    needs_copy_helper: bool = False

    # Set if the class needs a shadow (ie. derived) class.
    needs_shadow: bool = False

    # Set if /NoDefaultCtors/ was specified.
    no_default_ctors: bool = False

    # Set if /NoTypeHint/ was specified.
    no_type_hint: bool = False

    # Set if the class name should not be used in the generated code (and the
    # instantiated template name should be used instead).
    no_type_name: bool = False

    # The overloaded methods.
    overloads: list[Overload] = field(default_factory=list)

    # The %PickleCode.
    pickle_code: Optional[CodeBlock] = None

    # The properties.
    properties: list[Property] = field(default_factory=list)

    # The /PyQtFlags/.
    pyqt_flags: int = 0

    # The /PyQtFlagsEnums/.
    pyqt_flags_enums: Optional[list[str]] = None

    # The /PyQtInterface/.
    pyqt_interface: Optional[str] = None

    # Set if /PyQtNoQMetaObject/ was specified.
    pyqt_no_qmetaobject: bool = False

    # The real class if this is a proxy or a namespace extender.
    real_class: Optional['WrappedClass'] = None

    # The sub-class base class. (resolver)
    subclass_base: Optional['WrappedClass'] = None

    # The super-classes.
    superclasses: list['WrappedClass'] = field(default_factory=list)

    # The value of /Supertype/ if specified.
    supertype: Optional[CachedName] = None

    # The template that was instantiated to create this class.
    template: Optional[Template] = None

    # The %TypeCode.
    type_code: list[CodeBlock] = field(default_factory=list)

    # The %TypeHintCode.
    type_hint_code: Optional[CodeBlock] = None

    # The type hints.
    type_hints: Optional[TypeHints] = None

    # The name of the virtual error handler to use.
    virtual_error_handler: Optional[str] = None

    # The virtual overloaded methods. (resolver)
    virtual_overloads: list[VirtualOverload] = field(default_factory=list)

    # The visible member functions. (resolver)
    visible_members: list[VisibleMember] = field(default_factory=list)

    def __hash__(self):
        """ Reimplemented so an Argument object can be used as a dict key. """

        return id(self)


@dataclass
class WrappedEnum:
    """ Encapsulate a wrapped enum. """

    # The base type.
    base_type: EnumBaseType

    # The fully qualified C++ name.
    fq_cpp_name: Optional[ScopedName]

    # The defining module.
    module: Module

    # The cached fully qualified C++ name.
    cached_fq_cpp_name: Optional[CachedName] = None

    # Set if the enum is defined in a protected section.
    is_protected: bool = False

    # Set if the enum is a scoped enum.
    is_scoped: bool = False

    # The members.
    members: list['WrappedEnumMember'] = field(default_factory=list)

    # Set if this enum is needed by the module for which code is to be
    # generated. (resolver)
    needed: bool = False

    # Set if /NoScope/ was specified.
    no_scope: bool = False

    # Set if the type hint should be suppressed.
    no_type_hint: bool = False

    # The overloaded slot member functions. (resolver)
    overloads: list['Overload'] = field(default_factory=list)

    # The Python name.
    py_name: Optional[CachedName] = None

    # The enclosing scope.
    scope: Optional[Union[MappedType, WrappedClass]] = None

    # The slot member functions.  These can only be created by global operators
    # being moved. (resolver)
    slots: list[Member] = field(default_factory=list)

    # The generated type number. (resolver)
    type_nr: int = -1

    def __hash__(self):
        """ Reimplemented so an Argument object can be used as a dict key. """

        return id(self)


@dataclass
class WrappedEnumMember:
    """ Encapsulate a member of a wrapped enum. """

    # The C++ name.
    cpp_name: str

    # The Python name.
    py_name: CachedName

    # The enclosing enum.
    scope: 'WrappedEnum'

    # Set if the type hint should be suppressed.
    no_type_hint: bool = False


@dataclass
class WrappedException:
    """ Encapsulate a wrapped exception. """

    # The interface file.
    iface_file: IfaceFile

    # The code specified by the %RaiseCode directive.
    raise_code: CodeBlock

    # The base exception if it is a builtin.
    builtin_base_exception: Optional[str] = None

    # The class that implements the exception (if the exception is not a Python
    # exception).
    class_exception: Optional[WrappedClass] = None

    # The base exception if it is defined in the specification.
    defined_base_exception: Optional['WrappedException'] = None

    # The number of the exception (only if a Python exception object is
    # required. (resolver)
    exception_nr: int = -1

    # Set if this exception is needed by the module for which code is to be
    # generated. (resolver)
    needed: bool = False

    # The Python name.
    py_name: Optional[str] = None


@dataclass
class WrappedTypedef:
    """ Encapsulate a wrapped typedef. """

    # The fully qualified C++ name.
    fq_cpp_name: ScopedName

    # The defining module.
    module: Module

    # The enclosing scope.
    scope: Optional[WrappedClass]

    # The type.
    type: Argument

    # Set if the typedef name should not be used in the generated code.
    no_type_name: bool = False


@dataclass
class WrappedVariable:
    """ Encapsulate a wrapped variable. """

    # The fully qualified C++ name.
    fq_cpp_name: ScopedName

    # The defining module.
    module: Module

    # The Python name.
    py_name: CachedName

    # The enclosing scope.
    scope: Optional[WrappedClass]

    # The type.
    type: Argument

    # The code specified by any %AccessCode directive.
    access_code: Optional[CodeBlock] = None

    # The code specified by any %GetCode directive.
    get_code: Optional[CodeBlock] = None

    # Set if the variable is static.
    is_static: bool = False

    # Set if the variable needs a handler. (resolver)
    needs_handler: bool = False

    # Set if the type hint should be suppressed.
    no_type_hint: bool = False

    # Set if the variable has no setter and will be read-only.
    no_setter: bool = False

    # The code specified by any %SetCode directive.
    set_code: Optional[CodeBlock] = None
