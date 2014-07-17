"""
Copyright 2008-2011 Free Software Foundation, Inc.
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

from . import odict
from . Constants import ADVANCED_PARAM_TAB, DEFAULT_PARAM_TAB
from Element import Element

from Cheetah.Template import Template
from UserDict import UserDict
from itertools import imap

class TemplateArg(UserDict):
    """
    A cheetah template argument created from a param.
    The str of this class evaluates to the param's to code method.
    The use of this class as a dictionary (enum only) will reveal the enum opts.
    The __call__ or () method can return the param evaluated to a raw python data type.
    """

    def __init__(self, param):
        UserDict.__init__(self)
        self._param = param
        if param.is_enum():
            for key in param.get_opt_keys():
                self[key] = str(param.get_opt(key))

    def __str__(self):
        return str(self._param.to_code())

    def __call__(self):
        return self._param.get_evaluated()

def _get_keys(lst): return [elem.get_key() for elem in lst]
def _get_elem(lst, key):
    try: return lst[_get_keys(lst).index(key)]
    except ValueError: raise ValueError, 'Key "%s" not found in %s.'%(key, _get_keys(lst))

class Block(Element):

    def __init__(self, flow_graph, n):
        """
        Make a new block from nested data.

        Args:
            flow: graph the parent element
            n: the nested odict

        Returns:
            block a new block
        """
        #build the block
        Element.__init__(self, flow_graph)
        #grab the data
        params = n.findall('param')
        sources = n.findall('source')
        sinks = n.findall('sink')
        self._name = n.find('name')
        self._key = n.find('key')
        self._category = n.find('category') or ''
        self._grc_source = n.find('grc_source') or ''
        self._block_wrapper_path = n.find('block_wrapper_path')
        self._bussify_sink = n.find('bus_sink')
        self._bussify_source = n.find('bus_source')
        self._var_value = n.find('var_value') or '$value'

        # get list of param tabs
        n_tabs = n.find('param_tab_order') or None
        self._param_tab_labels = n_tabs.findall('tab') if n_tabs is not None else [DEFAULT_PARAM_TAB]

        #create the param objects
        self._params = list()
        #add the id param
        self.get_params().append(self.get_parent().get_parent().Param(
            block=self,
            n=odict({
                'name': 'ID',
                'key': 'id',
                'type': 'id',
            })
        ))
        self.get_params().append(self.get_parent().get_parent().Param(
            block=self,
            n=odict({
                'name': 'Enabled',
                'key': '_enabled',
                'type': 'raw',
                'value': 'True',
                'hide': 'all',
            })
        ))
        for param in imap(lambda n: self.get_parent().get_parent().Param(block=self, n=n), params):
            key = param.get_key()
            #test against repeated keys
            if key in self.get_param_keys():
                raise Exception, 'Key "%s" already exists in params'%key
            #store the param
            self.get_params().append(param)
        #create the source objects
        self._sources = list()
        for source in map(lambda n: self.get_parent().get_parent().Port(block=self, n=n, dir='source'), sources):
            key = source.get_key()
            #test against repeated keys
            if key in self.get_source_keys():
                raise Exception, 'Key "%s" already exists in sources'%key
            #store the port
            self.get_sources().append(source)
        self.back_ofthe_bus(self.get_sources())
        #create the sink objects
        self._sinks = list()
        for sink in map(lambda n: self.get_parent().get_parent().Port(block=self, n=n, dir='sink'), sinks):
            key = sink.get_key()
            #test against repeated keys
            if key in self.get_sink_keys():
                raise Exception, 'Key "%s" already exists in sinks'%key
            #store the port
            self.get_sinks().append(sink)
        self.back_ofthe_bus(self.get_sinks())
        self.current_bus_structure = {'source':'','sink':''};

        # Virtual source/sink and pad source/sink blocks are
        # indistinguishable from normal GR blocks. Make explicit
        # checks for them here since they have no work function or
        # buffers to manage.
        is_not_virtual_or_pad = ((self._key != "virtual_source") \
                                     and (self._key != "virtual_sink") \
                                     and (self._key != "pad_source") \
                                     and (self._key != "pad_sink"))

        if is_not_virtual_or_pad:
            self.get_params().append(self.get_parent().get_parent().Param(
                block=self,
                n=odict({'name': 'Block Alias',
                         'key': 'alias',
                         'type': 'string',
                         'hide': 'part',
                         'tab': ADVANCED_PARAM_TAB
                     })
            ))

        if (len(sources) or len(sinks)) and is_not_virtual_or_pad:
            self.get_params().append(self.get_parent().get_parent().Param(
                    block=self,
                    n=odict({'name': 'Core Affinity',
                             'key': 'affinity',
                             'type': 'int_vector',
                             'hide': 'part',
                             'tab': ADVANCED_PARAM_TAB
                             })
                    ))
        if len(sources) and is_not_virtual_or_pad:
            self.get_params().append(self.get_parent().get_parent().Param(
                    block=self,
                    n=odict({'name': 'Min Output Buffer',
                             'key': 'minoutbuf',
                             'type': 'int',
                             'hide': 'part',
                             'value': '0',
                             'tab': ADVANCED_PARAM_TAB
                             })
                    ))
            self.get_params().append(self.get_parent().get_parent().Param(
                    block=self,
                    n=odict({'name': 'Max Output Buffer',
                             'key': 'maxoutbuf',
                             'type': 'int',
                             'hide': 'part',
                             'value': '0',
                             'tab': ADVANCED_PARAM_TAB
                             })
                    ))


    def back_ofthe_bus(self, portlist):
        portlist.sort(key=lambda a: a.get_type() == 'bus');

    def filter_bus_port(self, ports):
        buslist = [i for i in ports if i.get_type() == 'bus'];
        if len(buslist) == 0:
            return ports;
        else:
            return buslist;

    def get_enabled(self):
        """
        Get the enabled state of the block.

        Returns:
            true for enabled
        """
        try: return eval(self.get_param('_enabled').get_value())
        except: return True

    def set_enabled(self, enabled):
        """
        Set the enabled state of the block.

        Args:
            enabled: true for enabled
        """
        self.get_param('_enabled').set_value(str(enabled))

    def __str__(self): return 'Block - %s - %s(%s)'%(self.get_id(), self.get_name(), self.get_key())

    def get_id(self): return self.get_param('id').get_value()
    def is_block(self): return True
    def get_name(self): return self._name
    def get_key(self): return self._key
    def get_category(self): return self._category
    def set_category(self, cat): self._category = cat
    def get_doc(self): return ''
    def get_ports(self): return self.get_sources() + self.get_sinks()
    def get_ports_gui(self): return self.filter_bus_port(self.get_sources()) + self.filter_bus_port(self.get_sinks());
    def get_children(self): return self.get_ports() + self.get_params()
    def get_children_gui(self): return self.get_ports_gui() + self.get_params()
    def get_block_wrapper_path(self): return self._block_wrapper_path

    ##############################################
    # Access Params
    ##############################################
    def get_param_tab_labels(self): return self._param_tab_labels
    def get_param_keys(self): return _get_keys(self._params)
    def get_param(self, key): return _get_elem(self._params, key)
    def get_params(self): return self._params
    def has_param(self, key):
        try:
            _get_elem(self._params, key);
            return True;
        except:
            return False;

    ##############################################
    # Access Sinks
    ##############################################
    def get_sink_keys(self): return _get_keys(self._sinks)
    def get_sink(self, key): return _get_elem(self._sinks, key)
    def get_sinks(self): return self._sinks
    def get_sinks_gui(self): return self.filter_bus_port(self.get_sinks())

    ##############################################
    # Access Sources
    ##############################################
    def get_source_keys(self): return _get_keys(self._sources)
    def get_source(self, key): return _get_elem(self._sources, key)
    def get_sources(self): return self._sources
    def get_sources_gui(self): return self.filter_bus_port(self.get_sources());

    def get_connections(self):
        return sum([port.get_connections() for port in self.get_ports()], [])

    def resolve_dependencies(self, tmpl):
        """
        Resolve a paramater dependency with cheetah templates.

        Args:
            tmpl: the string with dependencies

        Returns:
            the resolved value
        """
        tmpl = str(tmpl)
        if '$' not in tmpl: return tmpl
        n = dict((p.get_key(), TemplateArg(p)) for p in self.get_params())
        try:
            return str(Template(tmpl, n))
        except Exception as err:
            return "Template error: %s\n    %s" % (tmpl, err)

    ##############################################
    # Controller Modify
    ##############################################
    def type_controller_modify(self, direction):
        """
        Change the type controller.

        Args:
            direction: +1 or -1

        Returns:
            true for change
        """
        changed = False
        type_param = None
        for param in filter(lambda p: p.is_enum(), self.get_params()):
            children = self.get_ports() + self.get_params()
            #priority to the type controller
            if param.get_key() in ' '.join(map(lambda p: p._type, children)): type_param = param
            #use param if type param is unset
            if not type_param: type_param = param
        if type_param:
            #try to increment the enum by direction
            try:
                keys = type_param.get_option_keys()
                old_index = keys.index(type_param.get_value())
                new_index = (old_index + direction + len(keys))%len(keys)
                type_param.set_value(keys[new_index])
                changed = True
            except: pass
        return changed

    def port_controller_modify(self, direction):
        """
        Change the port controller.

        Args:
            direction: +1 or -1

        Returns:
            true for change
        """
        return False

    def form_bus_structure(self, direc):
        if direc == 'source':
            get_p = self.get_sources;
            get_p_gui = self.get_sources_gui;
            bus_structure = self.get_bus_structure('source');
        else:
            get_p = self.get_sinks;
            get_p_gui = self.get_sinks_gui
            bus_structure = self.get_bus_structure('sink');

        struct = [range(len(get_p()))];
        if True in map(lambda a: isinstance(a.get_nports(), int), get_p()):


            structlet = [];
            last = 0;
            for j in [i.get_nports() for i in get_p() if isinstance(i.get_nports(), int)]:
                structlet.extend(map(lambda a: a+last, range(j)));
                last = structlet[-1] + 1;
                struct = [structlet];
        if bus_structure:

            struct = bus_structure

        self.current_bus_structure[direc] = struct;
        return struct

    def bussify(self, n, direc):
        if direc == 'source':
            get_p = self.get_sources;
            get_p_gui = self.get_sources_gui;
            bus_structure = self.get_bus_structure('source');
        else:
            get_p = self.get_sinks;
            get_p_gui = self.get_sinks_gui
            bus_structure = self.get_bus_structure('sink');


        for elt in get_p():
            for connect in elt.get_connections():
                self.get_parent().remove_element(connect);






        if (not 'bus' in map(lambda a: a.get_type(), get_p())) and len(get_p()) > 0:

            struct = self.form_bus_structure(direc);
            self.current_bus_structure[direc] = struct;
            if get_p()[0].get_nports():
                n['nports'] = str(1);

            for i in range(len(struct)):
                n['key'] = str(len(get_p()));
                n = odict(n);
                port = self.get_parent().get_parent().Port(block=self, n=n, dir=direc);
                get_p().append(port);




        elif 'bus' in map(lambda a: a.get_type(), get_p()):
            for elt in get_p_gui():
                get_p().remove(elt);
            self.current_bus_structure[direc] = ''
    ##############################################
    ## Import/Export Methods
    ##############################################
    def export_data(self):
        """
        Export this block's params to nested data.

        Returns:
            a nested data odict
        """
        n = odict()
        n['key'] = self.get_key()
        n['param'] = map(lambda p: p.export_data(), self.get_params())
        if 'bus' in map(lambda a: a.get_type(), self.get_sinks()):
            n['bus_sink'] = str(1);
        if 'bus' in map(lambda a: a.get_type(), self.get_sources()):
            n['bus_source'] = str(1);
        return n

    def import_data(self, n):
        """
        Import this block's params from nested data.
        Any param keys that do not exist will be ignored.
        Since params can be dynamically created based another param,
        call rewrite, and repeat the load until the params stick.
        This call to rewrite will also create any dynamic ports
        that are needed for the connections creation phase.

        Args:
            n: the nested data odict
        """
        get_hash = lambda: hash(tuple(map(hash, self.get_params())))
        my_hash = 0
        while get_hash() != my_hash:
            params_n = n.findall('param')
            for param_n in params_n:
                key = param_n.find('key')
                value = param_n.find('value')
                #the key must exist in this block's params
                if key in self.get_param_keys():
                    self.get_param(key).set_value(value)
            #store hash and call rewrite
            my_hash = get_hash()
            self.rewrite()
        bussinks = n.findall('bus_sink');
        if len(bussinks) > 0 and not self._bussify_sink:
            self.bussify({'name':'bus','type':'bus'}, 'sink')
        elif len(bussinks) > 0:
            self.bussify({'name':'bus','type':'bus'}, 'sink')
            self.bussify({'name':'bus','type':'bus'}, 'sink')
        bussrcs = n.findall('bus_source');
        if len(bussrcs) > 0 and not self._bussify_source:
            self.bussify({'name':'bus','type':'bus'}, 'source')
        elif len(bussrcs) > 0:
            self.bussify({'name':'bus','type':'bus'}, 'source')
            self.bussify({'name':'bus','type':'bus'}, 'source')
