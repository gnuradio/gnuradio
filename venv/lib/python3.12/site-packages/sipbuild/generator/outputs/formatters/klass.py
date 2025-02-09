# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from ...scoped_name import STRIP_NONE

from .misc import fmt_scoped_py_name, iface_is_defined
from .template import fmt_template_as_cpp_type


def fmt_class_as_rest_ref(klass):
    """ Return a fully qualified Python name as a reST reference. """

    module_name = klass.iface_file.module.fq_py_name.name
    klass_name = fmt_scoped_py_name(klass.scope, klass.py_name.name)

    return f':sip:ref:`~{module_name}.{klass_name}`'


def fmt_class_as_scoped_name(spec, klass, scope=None, strip=STRIP_NONE,
        make_public=False, as_xml=False):
    """ Return a class's scoped name. """

    if klass.no_type_name:
        return fmt_template_as_cpp_type(spec, klass.template, strip=strip,
                as_xml=as_xml)

    # Protected classes have to be explicitly scoped.
    if klass.is_protected and not make_public:
        # This should never happen.
        if scope is None:
            scope = klass.iface_file

        return f'sip{scope.fq_cpp_name.as_word}::sip{klass.iface_file.fq_cpp_name.base_name}'

    return klass.iface_file.fq_cpp_name.cpp_stripped(strip)


def fmt_class_as_type_hint(spec, klass, defined):
    """ Return the type hint. """

    module = spec.module

    # We assume that an external class will be handled properly by some
    # handwritten type hint code.
    quote = '' if klass.external or iface_is_defined(klass.iface_file, klass.scope, module, defined) else "'"

    # Include the module name if it is not the current one.
    module_name = klass.iface_file.module.py_name + '.' if klass.iface_file.module is not module and defined is not None else ''
    py_name = fmt_scoped_py_name(klass.scope, klass.py_name.name)

    return f'{quote}{module_name}{py_name}{quote}'
