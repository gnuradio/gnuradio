# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from ...scoped_name import STRIP_NONE
from ...specification import ArgumentType

from .argument import (fmt_argument_as_cpp_type, fmt_argument_as_name,
        fmt_argument_as_type_hint)


def fmt_signature_as_cpp_declaration(spec, cpp_signature, scope=None,
        strip=STRIP_NONE, make_public=False, as_xml=False):
    """ Return the C++ representation of a signature's arguments as a
    declaration.
    """

    args = [fmt_argument_as_cpp_type(spec, arg, scope=scope, strip=strip,
                    make_public=make_public, as_xml=as_xml)
                for arg in cpp_signature.args]

    return ', '.join(args)


def fmt_signature_as_cpp_definition(spec, cpp_signature, scope=None,
        make_public=False):
    """ Return the C++ representation of a signature's arguments as a
    definition.
    """

    args = []

    for arg_nr, arg in enumerate(cpp_signature.args):
        arg_name = fmt_argument_as_name(spec, arg, arg_nr)

        args.append(
                fmt_argument_as_cpp_type(spec, arg, name=arg_name, scope=scope,
                        make_public=make_public))

    return ', '.join(args)


def fmt_signature_as_type_hint(spec, py_signature, need_self=True,
        exclude_result=False, defined=None):
    """ Return a signature as a type hint. """

    # Handle the input values.
    in_args = []

    if need_self:
        in_args.append('self')

    nr_out = 0

    for arg_nr, arg in enumerate(py_signature.args):
        if arg.is_out:
            nr_out += 1

        if arg.is_in:
            type_hint = fmt_argument_as_type_hint(spec, arg, defined,
                    arg_nr=arg_nr)
            if type_hint:
                in_args.append(type_hint)

    args_s = '(' + ', '.join(in_args) +')'

    if exclude_result:
        return args_s

    # Handle the output values.
    result = py_signature.result

    if result is None or (result.type is ArgumentType.VOID and len(result.derefs) == 0):
        is_result = False
    else:
        type_hints = result.type_hints

        # An empty type hint specifies a void return.
        if type_hints is not None and type_hints.hint_out is not None and type_hints.hint_out == '':
            is_result = False
        else:
            is_result = True

    if is_result or nr_out > 0:
        results_s = ' -> '

        needs_tuple = ((is_result and nr_out > 0) or nr_out > 1)

        if needs_tuple:
            results_s += '(' if defined is None else 'typing.Tuple['

        out_args = []

        if is_result:
            type_hint = fmt_argument_as_type_hint(spec, result, defined)
            if type_hint:
                out_args.append(type_hint)

        for arg in py_signature.args:
            if arg.is_out:
                type_hint = fmt_argument_as_type_hint(spec, arg, defined)
                if type_hint:
                    out_args.append(type_hint)

        results_s += ', '.join(out_args)

        if needs_tuple:
            results_s += ')' if defined is None else ']'
    elif defined is not None:
        results_s = ' -> None'
    else:
        # We don't show 'None' returns in docstrings.
        results_s = ''

    return f'{args_s}{results_s}'
