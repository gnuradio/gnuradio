# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from enum import IntEnum

from ..specification import AccessSpecifier, ArgumentType, ArrayArgument

from .formatters import fmt_argument_as_py_type, fmt_scoped_py_name


class IconNumber(IntEnum):
    """ The numbers of the different icons.  The values are those used by the
    eric IDE.
    """

    CLASS = 1
    METHOD = 4
    VARIABLE = 7
    ENUM = 10


def output_api(spec, api_filename):
    """ Output a QScintilla API file. """

    with open(api_filename, 'w', encoding='UTF-8') as af:
        module = spec.module

        _enums(af, spec, module)
        _variables(af, spec, module)

        for overload in module.overloads:
            if overload.common.module is module and overload.common.py_slot is None:
                _overload(af, spec, module, overload)

        for klass in spec.classes:
            if klass.iface_file.module is module and not klass.external:
                _enums(af, spec, module, scope=klass)
                _variables(af, spec, module, scope=klass)

                for ctor in klass.ctors:
                    if ctor.access_specifier is not AccessSpecifier.PRIVATE:
                        _ctor(af, spec, module, ctor, klass)

                for overload in klass.overloads:
                    if overload.access_specifier is not AccessSpecifier.PRIVATE and overload.common.py_slot is None:
                        _overload(af, spec, module, overload, scope=klass)


def _ctor(af, spec, module, ctor, scope):
    """ Generate an API ctor. """

    py_class = module.py_name + '.' + fmt_scoped_py_name(scope.scope, scope.py_name.name)
    py_arguments = _get_py_arguments(spec, ctor.py_signature)

    # Do the callable type form.
    args_s = ', '.join(py_arguments)
    af.write(f'{py_class}?{IconNumber.CLASS}({args_s})\n')

    # Do the call __init__ form.
    py_arguments.insert(0, 'self')

    args_s = ', '.join(py_arguments)
    af.write(f'{py_class}.__init__?{IconNumber.CLASS}({args_s})\n')


def _enums(af, spec, module, scope=None):
    """ Generate the APIs for all the enums in a scope. """

    for enum in spec.enums:
        if enum.module is module and enum.scope is scope:
            if enum.py_name is not None:
                py_name = fmt_scoped_py_name(enum.scope, enum.py_name.name)
                af.write(f'{module.py_name}.{py_name}?{IconNumber.ENUM}\n')

            for member in enum.members:
                enum_name = enum.module.py_name

                if enum.py_name is not None:
                    enum_name += '.'
                    enum_name += fmt_scoped_py_name(enum.scope,
                            enum.py_name.name)

                af.write(f'{enum_name}.{member.py_name.name}?{IconNumber.ENUM}\n')


def _variables(af, spec, module, scope=None):
    """ Generate the APIs for all the variables in a scope. """

    for variable in spec.variables:
        if variable.module is module and variable.scope is scope:
            py_name = fmt_scoped_py_name(variable.scope, variable.py_name.name)
            af.write(f'{module.py_name}.{py_name}?{IconNumber.VARIABLE}\n')


def _overload(af, spec, module, overload, scope=None):
    """ Generate a single API overload. """

    py_arguments = _get_py_arguments(spec, overload.py_signature)
    py_results = _get_py_results(spec, overload.py_signature)

    scoped_py_name = fmt_scoped_py_name(scope, overload.common.py_name.name)
    args_s = ', '.join(py_arguments)

    s = f'{module.py_name}.{scoped_py_name}?{IconNumber.METHOD}({args_s})'

    if len(py_results) != 0:
        s += ' -> '

        results_s = ', '.join(py_results)

        if len(py_results) > 1:
            s += '(' + results_s + ')'
        else:
            s += results_s

    s += '\n'

    af.write(s)


def _get_py_arguments(spec, py_signature):
    """ Return the list of Python arguments. """

    args = []

    for arg in py_signature.args:
        if arg.array is not ArrayArgument.ARRAY_SIZE and arg.is_in:
            args.append(fmt_argument_as_py_type(spec, arg, default_value=True))

    return args


def _get_py_results(spec, py_signature):
    """ Return the list of Python results. """

    results = []

    if py_signature.result is not None:
        if py_signature.result.type is not ArgumentType.VOID or len(py_signature.result.derefs) != 0:
            results.append(fmt_argument_as_py_type(spec,py_signature.result))

    for arg in py_signature.args:
        if arg.is_out:
            results.append(fmt_argument_as_py_type(spec, arg))

    return results
