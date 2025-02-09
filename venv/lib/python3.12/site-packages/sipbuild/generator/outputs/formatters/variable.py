# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


def fmt_variable_as_rest_ref(variable):
    """ Return the fully qualified Python name as a reST reference. """

    module_name = variable.module.fq_py_name.name
    variable_name = fmt_scoped_py_name(self.scope, variable.py_name.name)

    return f':sip:ref:`~{module_name}.{variable_name}`'
