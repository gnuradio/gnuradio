# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


def fmt_copying(copying, comment):
    """ Return a formatted %Copying text. """

    s = ''.join([b.text for b in copying]).rstrip().replace('\n', '\n' + comment + ' ')

    if s:
        s = comment + '\n' + comment + ' ' + s + '\n'

    return s


def fmt_scoped_py_name(scope, py_name):
    """ Return a formatted scoped Python name. """

    if scope is None or scope.is_hidden_namespace:
        scope_s = ''
    else:
        scope_s = fmt_scoped_py_name(scope.scope, None) + scope.py_name.name + '.'

    if py_name is None:
        py_name_s = ''
    else:
        py_name_s = py_name

    return scope_s + py_name_s


def iface_is_defined(iface_file, scope, module, defined):
    """ Return True if a type corresponding to an interface file has been
    defined in the context of a module.
    """

    # It is implicitly defined if we are not keeping a record.
    if defined is None:
        return True

    # A type in another module would have been imported.
    if iface_file.module is not module:
        return True

    if iface_file not in defined:
        return False

    # Check all enclosing scopes have been defined as well.
    while scope is not None:
        if scope.iface_file not in defined:
            return False

        scope = scope.scope

    return True
