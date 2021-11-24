# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
"""
This dict class holds a (shared) cache of compiled mako templates.
These

"""

from mako.template import Template
from mako.exceptions import SyntaxException

from ..errors import TemplateError

# The utils dict contains convenience functions
# that can be called from any template


def no_quotes(string, fallback=None):
    if len(string) > 2:
        if str(string)[0] + str(string)[-1] in ("''", '""'):
            return str(string)[1:-1]
    return str(fallback if fallback else string)


utils = {'no_quotes': no_quotes}


class MakoTemplates(dict):

    _template_cache = {}

    def __init__(self, _bind_to=None, *args, **kwargs):
        self.instance = _bind_to
        dict.__init__(self, *args, **kwargs)

    def __get__(self, instance, owner):
        if instance is None or self.instance is not None:
            return self
        copy = self.__class__(_bind_to=instance, **self)
        if getattr(instance.__class__, 'templates', None) is self:
            setattr(instance, 'templates', copy)
        return copy

    @classmethod
    def compile(cls, text):
        text = str(text)
        try:
            template = Template(text, strict_undefined=True)
        except SyntaxException as error:
            raise TemplateError(text, *error.args)

        cls._template_cache[text] = template
        return template

    def _get_template(self, text):
        try:
            return self._template_cache[str(text)]
        except KeyError:
            return self.compile(text)

    def render(self, item):
        text = self.get(item)
        if not text:
            return ''
        namespace = self.instance.namespace_templates
        namespace = {**namespace, **utils}

        try:
            if isinstance(text, list):
                templates = (self._get_template(t) for t in text)
                return [template.render(**namespace) for template in templates]
            else:
                template = self._get_template(text)
                return template.render(**namespace)
        except Exception as error:
            raise TemplateError(error, text)
