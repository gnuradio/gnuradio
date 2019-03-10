# Copyright 2015-16 Free Software Foundation, Inc.
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

from __future__ import absolute_import

from ast import literal_eval
from textwrap import dedent

from . import Block, register_build_in
from ._templates import MakoTemplates

from .. import utils
from ..base import Element

from ._build import build_params


DEFAULT_CODE = '''\
"""
Embedded Python Blocks:

Each time this file is saved, GRC will instantiate the first class it finds
to get ports and parameters of your block. The arguments to __init__  will
be the parameters. All of them are required to have default values!
"""

import numpy as np
from gnuradio import gr


class blk(gr.sync_block):  # other base classes are basic_block, decim_block, interp_block
    """Embedded Python Block example - a simple multiply const"""

    def __init__(self, example_param=1.0):  # only default arguments here
        """arguments to this function show up as parameters in GRC"""
        gr.sync_block.__init__(
            self,
            name='Embedded Python Block',   # will show up in GRC
            in_sig=[np.complex64],
            out_sig=[np.complex64]
        )
        # if an attribute with the same name as a parameter is found,
        # a callback is registered (properties work, too).
        self.example_param = example_param

    def work(self, input_items, output_items):
        """example: multiply with constant"""
        output_items[0][:] = input_items[0] * self.example_param
        return len(output_items[0])
'''

DOC = """
This block represents an arbitrary GNU Radio Python Block.

Its source code can be accessed through the parameter 'Code' which opens your editor. \
Each time you save changes in the editor, GRC will update the block. \
This includes the number, names and defaults of the parameters, \
the ports (stream and message) and the block name and documentation.

Block Documentation:
(will be replaced the docstring of your block class)
"""


@register_build_in
class EPyBlock(Block):

    key = 'epy_block'
    label = 'Python Block'
    documentation = {'': DOC}

    parameters_data = build_params(
        params_raw=[
            dict(label='Code', id='_source_code', dtype='_multiline_python_external',
                 default=DEFAULT_CODE, hide='part')
        ], have_inputs=True, have_outputs=True, flags=Block.flags, block_id=key
    )
    inputs_data = []
    outputs_data = []

    def __init__(self, flow_graph, **kwargs):
        super(EPyBlock, self).__init__(flow_graph, **kwargs)
        self.states['_io_cache'] = ''

        self._epy_source_hash = -1
        self._epy_reload_error = None

    def rewrite(self):
        Element.rewrite(self)

        param_src = self.params['_source_code']

        src = param_src.get_value()
        src_hash = hash((self.name, src))
        if src_hash == self._epy_source_hash:
            return

        try:
            blk_io = utils.epy_block_io.extract(src)

        except Exception as e:
            self._epy_reload_error = ValueError(str(e))
            try:  # Load last working block io
                blk_io_args = literal_eval(self.states['_io_cache'])
                if len(blk_io_args) == 6:
                    blk_io_args += ([],)  # add empty callbacks
                blk_io = utils.epy_block_io.BlockIO(*blk_io_args)
            except Exception:
                return
        else:
            self._epy_reload_error = None  # Clear previous errors
            self.states['_io_cache'] = repr(tuple(blk_io))

        # print "Rewriting embedded python block {!r}".format(self.name)
        self._epy_source_hash = src_hash

        self.label = blk_io.name or blk_io.cls
        self.documentation = {'': blk_io.doc}

        self.templates['imports'] = 'import ' + self.name
        self.templates['make'] = '{mod}.{cls}({args})'.format(
            mod=self.name,
            cls=blk_io.cls,
            args=', '.join('{0}=${{ {0} }}'.format(key) for key, _ in blk_io.params))
        self.templates['callbacks'] = [
            '{0} = ${{ {0} }}'.format(attr) for attr in blk_io.callbacks
            ]

        self._update_params(blk_io.params)
        self._update_ports('in', self.sinks, blk_io.sinks, 'sink')
        self._update_ports('out', self.sources, blk_io.sources, 'source')

        super(EPyBlock, self).rewrite()

    def _update_params(self, params_in_src):
        param_factory = self.parent_platform.make_param
        params = {}
        for param in list(self.params):
            if hasattr(param, '__epy_param__'):
                params[param.key] = param
                del self.params[param.key]

        for id_, value in params_in_src:
            try:
                param = params[id_]
                if param.default == param.value:
                    param.set_value(value)
                param.default = str(value)
            except KeyError:  # need to make a new param
                param = param_factory(
                    parent=self,  id=id_, dtype='raw', value=value,
                    name=id_.replace('_', ' ').title(),
                )
                setattr(param, '__epy_param__', True)
            self.params[id_] = param

    def _update_ports(self, label, ports, port_specs, direction):
        port_factory = self.parent_platform.make_port
        ports_to_remove = list(ports)
        iter_ports = iter(ports)
        ports_new = []
        port_current = next(iter_ports, None)
        for key, port_type, vlen in port_specs:
            reuse_port = (
                port_current is not None and
                port_current.dtype == port_type and
                port_current.vlen == vlen and
                (key.isdigit() or port_current.key == key)
            )
            if reuse_port:
                ports_to_remove.remove(port_current)
                port, port_current = port_current, next(iter_ports, None)
            else:
                n = dict(name=label + str(key), dtype=port_type, id=key)
                if port_type == 'message':
                    n['name'] = key
                    n['optional'] = '1'
                if vlen > 1:
                    n['vlen'] = str(vlen)
                port = port_factory(self, direction=direction, **n)
            ports_new.append(port)
        # replace old port list with new one
        del ports[:]
        ports.extend(ports_new)
        # remove excess port connections
        self.parent_flowgraph.disconnect(*ports_to_remove)

    def validate(self):
        super(EPyBlock, self).validate()
        if self._epy_reload_error:
            self.params['_source_code'].add_error_message(str(self._epy_reload_error))


@register_build_in
class EPyModule(Block):
    key = 'epy_module'
    label = 'Python Module'
    documentation = {'': dedent("""
        This block lets you embed a python module in your flowgraph.

        Code you put in this module is accessible in other blocks using the ID of this
        block. Example:

        If you put

            a = 2

            def double(arg):
                return 2 * arg

        in a Python Module Block with the ID 'stuff' you can use code like

            stuff.a  # evals to 2
            stuff.double(3)  # evals to 6

        to set parameters of other blocks in your flowgraph.
    """)}

    parameters_data = build_params(
        params_raw=[
            dict(label='Code', id='source_code', dtype='_multiline_python_external',
                 default='# this module will be imported in the into your flowgraph',
                 hide='part')
        ], have_inputs=False, have_outputs=False, flags=Block.flags, block_id=key
    )

    templates = MakoTemplates(
        imports='import ${ id }  # embedded python module',
    )
