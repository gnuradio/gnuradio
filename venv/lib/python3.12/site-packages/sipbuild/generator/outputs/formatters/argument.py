# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from ...scoped_name import STRIP_NONE
from ...specification import ArgumentType, ArrayArgument, ClassKey, ValueType

from ..type_hints import format_voidptr, TypeHintManager

from .misc import fmt_scoped_py_name


def fmt_argument_as_name(spec, arg, arg_nr):
    """ Return an argument as a name. """

    if spec.module.use_arg_names and arg.name is not None and arg.type is not ArgumentType.ELLIPSIS:
        return arg.name.name

    return f'a{arg_nr}'


def fmt_argument_as_cpp_type(spec, arg, name=None, scope=None,
        strip=STRIP_NONE, make_public=False, use_typename=True, plain=False,
        no_derefs=False, as_xml=False):
    """ Return an argument as a C++ type. """

    original_typedef = arg.original_typedef
    nr_derefs = 0 if no_derefs else len(arg.derefs)
    is_const = arg.is_const and not plain
    is_reference = arg.is_reference and not plain

    s = ''

    if use_typename and original_typedef is not None and not original_typedef.no_type_name and arg.array is not ArrayArgument.ARRAY_SIZE:
        # Reverse the previous expansion of the typedef.
        if is_const and not original_typedef.type.is_const:
            s += 'const '

        nr_derefs -= len(original_typedef.type.derefs)

        if original_typedef.type.is_reference:
            is_reference = False

        s += original_typedef.fq_cpp_name.cpp_stripped(strip)

    else:
        # A function type is handled differently because of the position of the
        # name.
        if arg.type is ArgumentType.FUNCTION:
            from .signature import fmt_signature_as_cpp_declaration

            s += fmt_argument_as_cpp_type(spec, arg.definition.result,
                    scope=scope, strip=strip, as_xml=as_xml)

            s += ' (' + '*' * nr_derefs + name + ')('

            s += fmt_signature_as_cpp_declaration(spec, arg.definition,
                    scope=scope, as_xml=as_xml)

            s += ')'

            return s

        if is_const:
            s += 'const '

        if arg.type in (ArgumentType.SBYTE, ArgumentType.SSTRING):
            s += 'signed char'

        elif arg.type in (ArgumentType.UBYTE, ArgumentType.USTRING):
            s += 'unsigned char'

        elif arg.type is ArgumentType.WSTRING:
            s += 'wchar_t'

        elif arg.type in (ArgumentType.BYTE, ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING, ArgumentType.STRING):
            s += 'char'

        elif arg.type is ArgumentType.USHORT:
            s += 'unsigned short'

        elif arg.type is ArgumentType.SHORT:
            s += 'short'

        elif arg.type is ArgumentType.UINT:
            # Qt4 moc uses "uint" in signal signatures.  We do all the time and
            # hope it is always defined.
            s += 'uint'

        elif arg.type in (ArgumentType.INT, ArgumentType.CINT):
            s += 'int'

        elif arg.type is ArgumentType.HASH:
            s += 'Py_hash_t'

        elif arg.type is ArgumentType.SSIZE:
            s += 'Py_ssize_t'

        elif arg.type is ArgumentType.SIZE:
            s += 'size_t'

        elif arg.type is ArgumentType.ULONG:
            s += 'unsigned long'

        elif arg.type is ArgumentType.LONG:
            s += 'long'

        elif arg.type is ArgumentType.ULONGLONG:
            s += 'unsigned long long'

        elif arg.type is ArgumentType.LONGLONG:
            s += 'long long'

        elif arg.type is ArgumentType.STRUCT:
            s += 'struct ' + arg.definition.as_cpp

        elif arg.type is ArgumentType.UNION:
            s += 'union ' + arg.definition.as_cpp

        elif arg.type is ArgumentType.CAPSULE:
            nr_derefs = 1
            s += 'void'

        elif arg.type in (ArgumentType.FAKE_VOID, ArgumentType.VOID):
            s += 'void'

        elif arg.type in (ArgumentType.BOOL, ArgumentType.CBOOL):
            s += 'bool'

        elif arg.type in (ArgumentType.FLOAT, ArgumentType.CFLOAT):
            s += 'float'

        elif arg.type in (ArgumentType.DOUBLE, ArgumentType.CDOUBLE):
            s += 'double'

        elif arg.type is ArgumentType.DEFINED:
            # The only defined types still remaining are arguments to templates
            # and default values.
            if as_xml:
                s += arg.definition.as_py
            else:
                if spec.c_bindings:
                    s += 'struct '

                s += arg.definition.cpp_stripped(strip)

        elif arg.type is ArgumentType.MAPPED:
            s += fmt_argument_as_cpp_type(spec, arg.definition.type,
                    scope=scope, strip=strip, as_xml=as_xml)

        elif arg.type is ArgumentType.CLASS:
            from .klass import fmt_class_as_scoped_name

            if spec.c_bindings:
                s += 'union ' if arg.definition.class_key is ClassKey.UNION else 'struct '

            s += fmt_class_as_scoped_name(spec, arg.definition, scope=scope,
                    strip=strip, make_public=make_public, as_xml=as_xml)

        elif arg.type is ArgumentType.TEMPLATE:
            from .template import fmt_template_as_cpp_type

            s += fmt_template_as_cpp_type(spec, arg.definition, strip=strip,
                    as_xml=as_xml)

        elif arg.type is ArgumentType.ENUM:
            from .enum import fmt_enum_as_cpp_type

            s += fmt_enum_as_cpp_type(arg.definition, make_public=make_public,
                    strip=strip)

        elif arg.type in (ArgumentType.PYOBJECT, ArgumentType.PYTUPLE, ArgumentType.PYLIST, ArgumentType.PYDICT, ArgumentType.PYCALLABLE, ArgumentType.PYSLICE, ArgumentType.PYTYPE, ArgumentType.PYBUFFER, ArgumentType.PYENUM, ArgumentType.ELLIPSIS):
            s += 'PyObject *'

    space_before_name = True

    for i in range(nr_derefs):
        # Note that we don't put a space before the '*' so that Qt normalised
        # signal signatures are correct.
        s += '*'
        space_before_name = False

        if arg.derefs[i]:
            s += ' const'
            space_before_name = True

    if is_reference:
        s += '&'

    if name:
        if space_before_name:
            s += ' '

        s += name

    return s


# The types that are implicitly pointers.
_IMPLICIT_POINTERS = (ArgumentType.PYOBJECT, ArgumentType.PYTUPLE,
        ArgumentType.PYLIST, ArgumentType.PYDICT, ArgumentType.PYCALLABLE,
        ArgumentType.PYSLICE, ArgumentType.PYTYPE, ArgumentType.CAPSULE,
        ArgumentType.PYBUFFER, ArgumentType.PYENUM
)

def fmt_argument_as_py_default_value(spec, arg, type_name, embedded=False,
        as_xml=False):
    """ Return the Python representation of an argument's default value. """

    from .value_list import fmt_value_list_as_py_expression

    # Use any explicitly provided documentation.
    if arg.type_hints is not None and arg.type_hints.default_value is not None:
        return arg.type_hints.default_value

    # Translate some special cases.
    if len(arg.default_value) == 1 and arg.default_value[0].value_type is ValueType.NUMERIC:
        value = arg.default_value[0].value

        if value == 0 and ('voidptr' in type_name or len(arg.derefs) != 0 or arg.type in _IMPLICIT_POINTERS):
            return 'None'

        if arg.type in (ArgumentType.BOOL, ArgumentType.CBOOL):
            return 'True' if value else 'False'

    return fmt_value_list_as_py_expression(spec, arg.default_value,
            embedded=embedded, as_xml=as_xml)


def fmt_argument_as_py_type(spec, arg, pep484=False, default_value=False,
        as_xml=False):
    """ Return an argument as a Python type. """

    scope, name = _py_arg(spec, arg, pep484, as_xml)

    s = fmt_scoped_py_name(scope, name)

    if default_value and arg.default_value is not None:
        if arg.name is not None:
            s += ' ' + arg.name.name

        s += '=' + fmt_argument_as_py_default_value(spec, arg, name)

    return s


def fmt_argument_as_rest_ref(spec, arg, out, as_xml=False):
    """ Return an argument as a reST reference. """

    s = ''

    hint = _get_hint(arg, out)

    if hint is None:
        if arg.type is ArgumentType.CLASS:
            from .klass import fmt_class_as_rest_ref

            s += fmt_class_as_rest_ref(arg.definition)
        elif arg.type is ArgumentType.ENUM:
            if arg.definition.py_name is not None:
                from .enum import fmt_enum_as_rest_ref

                s += fmt_enum_as_rest_ref(arg.definition)
            else:
                s += 'int'
        elif arg.type is ArgumentType.MAPPED:
            # There would normally be a type hint.
            s += "unknown-type"
        else:
            s += fmt_argument_as_py_type(spec, arg, as_xml=as_xml)
    else:
        context = arg.definition if arg.type is ArgumentType.CLASS else None

        s += TypeHintManager(spec).as_rest_ref(hint, out, context,
                as_xml=as_xml)

    return s


def fmt_argument_as_type_hint(spec, arg, defined, arg_nr=-1):
    """ Return an argument as a type hint. """

    if arg.array is ArrayArgument.ARRAY_SIZE:
        return ''

    pep484 = defined is not None

    s = ''

    if arg_nr is None:
        # It's a variable.
        out = False
    elif arg_nr >= 0:
        # It's a callable input argument.
        out = False

        if arg.name is None:
            s += f'a{arg_nr}: '
        else:
            name = _fix_py_keyword(arg.name.name)
            s += f'{name}: '
    else:
        # It's a callable result or output argument.
        out = True

    hint = _get_hint(arg, out)

    # Assume pointers can be None unless specified otherwise.
    allow_none = arg.allow_none or (arg.type in (ArgumentType.CLASS, ArgumentType.MAPPED) and arg.definition.handles_none)

    if hint is None and allow_none:
        use_optional = True
    else:
        use_optional = (not arg.disallow_none and len(arg.derefs) != 0)

    if use_optional:
        s += 'typing.Optional[' if pep484 else 'Optional['

    if arg.array is ArrayArgument.ARRAY:
        s += _sip_module_name(spec) + 'array['

    if hint is None:
        if arg.type is ArgumentType.CLASS:
            from .klass import fmt_class_as_type_hint

            type_name = fmt_class_as_type_hint(spec, arg.definition, defined)
        elif arg.type is ArgumentType.ENUM:
            if arg.definition.py_name is not None:
                from .enum import fmt_enum_as_type_hint

                type_name = fmt_enum_as_type_hint(spec, arg.definition,
                        defined)
            else:
                type_name = 'int'
        elif arg.type is ArgumentType.MAPPED:
            # There would normally be a type hint.
            type_name = 'typing.Any' if pep484 else 'Any'
        else:
            type_name = fmt_argument_as_py_type(spec, arg, pep484=pep484)
    else:
        type_hint_manager = TypeHintManager(spec)

        context = arg.definition if arg.type is ArgumentType.CLASS else None

        if pep484:
            type_name = type_hint_manager.as_type_hint(hint, out, context,
                    defined)
        else:
            type_name = type_hint_manager.as_docstring(hint, out, context)

    s += type_name

    if arg.array is ArrayArgument.ARRAY:
        s += ']'

    if use_optional:
        s += ']'

    # See if the argument is optional.
    if arg_nr is not None and arg_nr >= 0 and arg.default_value is not None:
        # Historically .pyi files don't include specific default values.
        s += ' = '
        s += '...' if pep484 else fmt_argument_as_py_default_value(spec, arg, type_name, embedded=True)

    return s


_PYTHON_KEYWORDS = (
    'False', 'None', 'True', 'and', 'as', 'assert', 'break', 'class',
    'continue', 'def', 'del', 'elif', 'else', 'except', 'finally', 'for',
    'from', 'global', 'if', 'import', 'in', 'is', 'lambda', 'nonlocal', 'not',
    'or', 'pass', 'raise', 'return', 'try', 'while', 'with', 'yield', 'exec',
    'print',
)

def _fix_py_keyword(word):
    """ Return a fixed word if it is a Python keyword (or has been at any
    time).
    """

    if word in _PYTHON_KEYWORDS:
        word += '_'

    return word


def _get_hint(arg, out):
    """ Return a raw type hint. """

    # Use any explicit type hint unless the argument is constrained.
    if arg.type_hints is None:
        hint = None
    elif out:
        hint = arg.type_hints.hint_out
    elif arg.is_constrained:
        hint = None
    else:
        hint = arg.type_hints.hint_in

    return hint


def _py_arg(spec, arg, pep484, as_xml):
    """ Return an argument as a 2-tuple of scope and name. """

    type = arg.type
    definition = arg.definition

    sip_module_name = _sip_module_name(spec)

    scope = None
    name = "unknown-type"

    if type is ArgumentType.CLASS:
        scope = definition.scope
        name = definition.py_name.name

    elif type is ArgumentType.MAPPED:
        if definition.py_name is not None:
            name = definition.py_name.name

    elif type is ArgumentType.ENUM:
        if definition.py_name is None:
            name = 'int'
        else:
            scope = definition.scope
            name = definition.py_name.name

    elif type is ArgumentType.DEFINED:
        name = definition.as_py

    elif type is ArgumentType.CAPSULE:
        name = definition.base_name

    elif type in (ArgumentType.STRUCT, ArgumentType.UNION, ArgumentType.VOID):
        name = format_voidptr(spec, as_xml)

    elif type in (ArgumentType.STRING, ArgumentType.SSTRING, ArgumentType.USTRING):
        name = 'bytes'

    elif type in (ArgumentType.WSTRING, ArgumentType.ASCII_STRING, ArgumentType.LATIN1_STRING, ArgumentType.UTF8_STRING):
        name = 'bytes' if arg.array is ArrayArgument.ARRAY else 'str'

    elif type in (ArgumentType.BYTE, ArgumentType.SBYTE, ArgumentType.UBYTE, ArgumentType.USHORT, ArgumentType.UINT, ArgumentType.LONG, ArgumentType.LONGLONG, ArgumentType.ULONG, ArgumentType.ULONGLONG, ArgumentType.SHORT, ArgumentType.INT, ArgumentType.CINT, ArgumentType.SSIZE, ArgumentType.SIZE, ArgumentType.HASH):
        name = 'int'

    elif type in (ArgumentType.FLOAT, ArgumentType.CFLOAT, ArgumentType.DOUBLE, ArgumentType.CDOUBLE):
        name = 'float'

    elif type in (ArgumentType.BOOL, ArgumentType.CBOOL):
        name = 'bool'

    elif type in (ArgumentType.PYOBJECT, ArgumentType.ELLIPSIS):
        name = 'typing.Any' if pep484 else 'Any'

    elif type is ArgumentType.PYTUPLE:
        name = 'tuple'

    elif type is ArgumentType.PYLIST:
        name = 'list'

    elif type is ArgumentType.PYDICT:
        name = 'dict'

    elif type is ArgumentType.PYCALLABLE:
        name = 'typing.Callable[..., None]' if pep484 else 'Callable[..., None]'

    elif type is ArgumentType.PYSLICE:
        name = 'slice'

    elif type is ArgumentType.PYTYPE:
        name = 'type'

    elif type is ArgumentType.PYBUFFER:
        if pep484:
            name = sip_module_name + 'Buffer'
        else:
            # This replicates sip.pyi.
            name = f'Union[bytes, bytearray, memoryview, {sip_module_name}array, {sip_module_name}voidptr]'

    elif type is ArgumentType.PYENUM:
        name = 'enum.Enum'

    return scope, name


def _sip_module_name(spec):
    """ Return the name of the sip module to be used as a prefix to an object
    in the module.
    """

    sip_module = spec.sip_module

    return sip_module + '.' if sip_module else ''
