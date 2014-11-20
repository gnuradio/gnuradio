"""
Copyright 2014 Free Software Foundation, Inc.
This file is part of GNU Radio

GNU Radio Companion is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

GNU Radio Companion is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
"""

from os import path
from collections import OrderedDict
from lxml import etree
from mako.template import Template

from . block_category_loader import xml_to_nested_data

BLOCK_DTD = etree.DTD(path.join(path.dirname(__file__), 'block.dtd'))


def load_block_xml(xml_file):
    """Load block description from xml file"""
    try:
        xml = etree.parse(xml_file).getroot()
        BLOCK_DTD.validate(xml)
    except etree.LxmlError:
        return

    n = xml_to_nested_data(xml)[1]
    n['block_wrapper_path'] = xml_file  # inject block wrapper path

    return construct_block_class_from_nested_data(n)


BLOCK_TEMPLATE = Template('''\
<%!
def to_func_args(kwargs):
    return ", ".join(
        "{}={}".format(key, repr(value))
        for key, value in kwargs.iteritems()
    )

def indent(s, level=1):
    if isinstance(s, str):
        s = s.split('\\n')
    return ("\\n" + " " * 4 * level).join(s)

%>
<%def name="on_rewrite(rewrites)">\\
% if rewrites:
.on_rewrite(
            ${ to_func_args(rewrites) }
        )\\
% endif
</%def>
class XMLBlock(Block):
    key = "${ key }"
    name = "${ name }"

    % if import_template:
    import_template = """
        ${ indent(import_template, 2) }
    """
    % endif
    % if make_template:

    make_template = """
        ${ indent(make_template, 2) }
    """
    % endif

    def setup(self, **kwargs):
        super(XMLBlock, self).setup(**kwargs)

        # params
        % for kwargs, rewrites in params:
        % if 'options' in kwargs:
        <%
            options = kwargs.pop('options')
        %>p = self.add_param(${ to_func_args(kwargs) })${ on_rewrite(rewrites) }
        % for option in options:
        p.add_option(${ to_func_args(option) })
        % endfor
        % else:
        self.add_param(${ to_func_args(kwargs) })${ on_rewrite(rewrites) }
        % endif
        % endfor

        # sinks
        % for method, kwargs, rewrites in sinks:
        self.${ method }(${ to_func_args(kwargs) })${ on_rewrite(rewrites) }
        % endfor

        # sources
        % for method, kwargs, rewrites in sources:
        self.${ method }(${ to_func_args(kwargs) })${ on_rewrite(rewrites) }
        % endfor
''')


class Resolver(object):

    def __init__(self, block_n):
        self.params = dict()
        self.collected_rewrites = {}
        self.get_param_defaults(block_n)

    def get_param_defaults(self, block_n):
        for param_n in block_n.get('param', []):
            key = param_n['key'][0]
            value = param_n.get('value', [None])[0]
            if value is None:
                options = param_n.get('option')
                if options:
                    value = options[0].get('key', None)
            self.params[key] = value


    def pop_rewrites(self):
        rewrites = self.collected_rewrites
        self.collected_rewrites = {}
        return rewrites

    def eval(self, key, expr):
        if '$' in expr:  # template
            try:
                param_key = expr[1:]
                default = self.params[param_key] # simple subst
                self.collected_rewrites[key] = param_key
                return default
            except KeyError:
                pass
            # todo parse/eval advanced template
        return expr

    def get(self, namespace, key, key2=None):
        expr = self.get_raw(namespace, key)
        return self.eval(key2 or key, expr) if expr else expr

    @staticmethod
    def get_raw(namespace, key):
        items = namespace.get(key, [None])
        if not isinstance(items, str):
            items = items[0]
        return items


def get_param_options(param_n, resolver):
    options = []
    for option_n in param_n.get('option', []):
        kwargs = OrderedDict()
        kwargs['name'] = resolver.get_raw(option_n, 'name')
        kwargs['value'] = resolver.get_raw(option_n, 'key')
        extras = dict(
            opt_n.split(':', 2)
            for opt_n in option_n.get('opt', []) if ':' in opt_n
        )
        if extras:
            kwargs['extra'] = extras
        options.append(kwargs)
    return options


def get_params(block_n, resolver):
    params = []
    for n in block_n.get('param', []):
        kwargs = OrderedDict()
        kwargs['name'] = resolver.get_raw(n, 'name')
        kwargs['key'] = resolver.get_raw(n, 'key')

        vtype = resolver.get(n, 'type')
        kwargs['vtype'] = vtype if vtype != 'enum' else None

        #todo: parse hide tag
        value = resolver.get_raw(n, 'value')
        if value:
            kwargs['default'] = value

        category = resolver.get_raw(n, 'tab')
        if category:
            kwargs['category'] = category

        if vtype == 'enum':
            kwargs['cls'] = 'OptionsParam'
            kwargs['options'] = get_param_options(n, resolver)

        params.append((kwargs, resolver.pop_rewrites()))
    return params


def get_ports(block_n, resolver, direction):
    ports = []
    for n in block_n.get(direction, []):
        kwargs = OrderedDict()
        kwargs['name'] = resolver.get_raw(n, 'name')

        dtype = resolver.get(n, 'type', 'dtype')
        if dtype == 'message':
            method_name = "add_message_" + direction
            kwargs['key'] = kwargs['name']
        else:
            method_name = "add_stream_" + direction
            kwargs['dtype'] = dtype
            vlen = resolver.get(n, 'vlen')
            if vlen:
                kwargs['vlen'] = int(vlen)

        ports.append((method_name, kwargs, resolver.pop_rewrites()))
    return ports


def construct_block_class_from_nested_data(nested_data):
    n = nested_data
    r = Resolver(n)
    return BLOCK_TEMPLATE.render(
        key=r.get_raw(n,'key'),
        name=r.get_raw(n, 'name'),

        make_template=n['make'][0],
        import_template="\n".join(n.get('import', [])),

        params=get_params(n, r),
        sinks=get_ports(n, r, 'sink'),
        sources=get_ports(n, r, 'source'),
    )


def _parse_string_literal(value):
    try:
        evaluated = eval(value, [])
        if isinstance(evaluated, str):
            value = evaluated
    except:
        pass
    return '"{}"'.format(value.replace('"', '\\"'))
