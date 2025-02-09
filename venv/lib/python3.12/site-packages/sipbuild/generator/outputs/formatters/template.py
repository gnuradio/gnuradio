# SPDX-License-Identifier: BSD-2-Clause

# Copyright (c) 2024 Phil Thompson <phil@riverbankcomputing.com>


from ...scoped_name import STRIP_NONE

from .signature import fmt_signature_as_cpp_declaration


def fmt_template_as_cpp_type(spec, template, strip=STRIP_NONE, as_xml=False):
    """ Return the C++ representation of a template type. """

    sig_s = fmt_signature_as_cpp_declaration(spec, template.types, strip=strip,
            as_xml=as_xml)

    return template.cpp_name.cpp_stripped(strip) + '<' + sig_s + '>'
