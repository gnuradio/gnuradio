# Copyright 2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# GNU Radio Companion is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# GNU Radio Companion is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""
This dict class holds a (shared) cache of compiled mako templates.
These

"""
from __future__ import absolute_import, print_function

from mako.template import Template
from mako.exceptions import SyntaxException

from ..errors import TemplateError


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
            template = Template(text)
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

        try:
            if isinstance(text, list):
                templates = (self._get_template(t) for t in text)
                return [template.render(**namespace) for template in templates]
            else:
                template = self._get_template(text)
                return template.render(**namespace)
        except Exception as error:
            raise TemplateError(error, text)
