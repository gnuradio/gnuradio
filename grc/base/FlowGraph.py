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
from Element import Element
from .. gui import Messages

class FlowGraph(Element):

    def __init__(self, platform):
        """
        Make a flow graph from the arguments.
        
        Args:
            platform: a platforms with blocks and contrcutors
        
        Returns:
            the flow graph object
        """
        #initialize
        Element.__init__(self, platform)
        #inital blank import
        self.import_data()

    def _get_unique_id(self, base_id=''):
        """
        Get a unique id starting with the base id.
        
        Args:
            base_id: the id starts with this and appends a count
        
        Returns:
            a unique id
        """
        index = 0
        while True:
            id = '%s_%d'%(base_id, index)
            index = index + 1
            #make sure that the id is not used by another block
            if not filter(lambda b: b.get_id() == id, self.get_blocks()): return id

    def __str__(self): return 'FlowGraph - %s(%s)'%(self.get_option('title'), self.get_option('id'))
    def rewrite(self):
        def refactor_bus_structure():
            
            for block in self.get_blocks():
                for direc in ['source', 'sink']:
                    if direc == 'source':
                        get_p = block.get_sources;
                        get_p_gui = block.get_sources_gui;
                        bus_structure = block.form_bus_structure('source');
                    else:
                        get_p = block.get_sinks;
                        get_p_gui = block.get_sinks_gui
                        bus_structure = block.form_bus_structure('sink');
                
                    if 'bus' in map(lambda a: a.get_type(), get_p_gui()):
                    
                            
                        
                        if len(get_p_gui()) > len(bus_structure):
                            times = range(len(bus_structure), len(get_p_gui()));
                            for i in times:
                                for connect in get_p_gui()[-1].get_connections():
                                    block.get_parent().remove_element(connect);
                                get_p().remove(get_p_gui()[-1]);
                        elif len(get_p_gui()) < len(bus_structure):
                            n = {'name':'bus','type':'bus'};
                            if True in map(lambda a: isinstance(a.get_nports(), int), get_p()):
                                n['nports'] = str(1);
                            
                            times = range(len(get_p_gui()), len(bus_structure));    
                            
                            for i in times:
                                n['key'] = str(len(get_p()));
                                n = odict(n);
                                port = block.get_parent().get_parent().Port(block=block, n=n, dir=direc);
                                get_p().append(port);
                                
                                
                                
        for child in self.get_children(): child.rewrite()
        refactor_bus_structure();

    def get_option(self, key):
        """
        Get the option for a given key.
        The option comes from the special options block.
        
        Args:
            key: the param key for the options block
        
        Returns:
            the value held by that param
        """
        return self._options_block.get_param(key).get_evaluated()

    def is_flow_graph(self): return True

    ##############################################
    ## Access Elements
    ##############################################
    def get_block(self, id): return filter(lambda b: b.get_id() == id, self.get_blocks())[0]
    def get_blocks_unordered(self): return filter(lambda e: e.is_block(), self.get_elements())
    def get_blocks(self):
        blocks = self.get_blocks_unordered();
        for i in range(len(blocks)):
            if blocks[i].get_key() == 'variable':
                blk = blocks[i];
                blocks.remove(blk);
                blocks.insert(1, blk);
        return blocks;
    def get_connections(self): return filter(lambda e: e.is_connection(), self.get_elements())
    def get_children(self): return self.get_elements()
    def get_elements(self):
        """
        Get a list of all the elements.
        Always ensure that the options block is in the list (only once).
        
        Returns:
            the element list
        """
        options_block_count = self._elements.count(self._options_block)
        if not options_block_count:
            self._elements.append(self._options_block)
        for i in range(options_block_count-1):
            self._elements.remove(self._options_block)
        return self._elements

    def get_enabled_blocks(self):
        """
        Get a list of all blocks that are enabled.
        
        Returns:
            a list of blocks
        """
        return filter(lambda b: b.get_enabled(), self.get_blocks())

    def get_enabled_connections(self):
        """
        Get a list of all connections that are enabled.
        
        Returns:
            a list of connections
        """
        return filter(lambda c: c.get_enabled(), self.get_connections())

    def get_new_block(self, key):
        """
        Get a new block of the specified key.
        Add the block to the list of elements.
        
        Args:
            key: the block key
        
        Returns:
            the new block or None if not found
        """
        if key not in self.get_parent().get_block_keys(): return None
        block = self.get_parent().get_new_block(self, key)
        self.get_elements().append(block);
        if block._bussify_sink:
            block.bussify({'name':'bus','type':'bus'}, 'sink')
        if block._bussify_source:
            block.bussify({'name':'bus','type':'bus'}, 'source')
        return block;

    def connect(self, porta, portb):
        """
        Create a connection between porta and portb.
        
        Args:
            porta: a port
            portb: another port
        @throw Exception bad connection
        
        Returns:
            the new connection
        """
        connection = self.get_parent().Connection(flow_graph=self, porta=porta, portb=portb)
        self.get_elements().append(connection)
        return connection

    def remove_element(self, element):
        """
        Remove the element from the list of elements.
        If the element is a port, remove the whole block.
        If the element is a block, remove its connections.
        If the element is a connection, just remove the connection.
        """
        if element not in self.get_elements(): return
        #found a port, set to parent signal block
        if element.is_port():
            element = element.get_parent()
        #remove block, remove all involved connections
        if element.is_block():
            for port in element.get_ports():
                map(self.remove_element, port.get_connections())
        if element.is_connection():
            if element.is_bus():
                cons_list = []
                for i in map(lambda a: a.get_connections(), element.get_source().get_associated_ports()):
                    cons_list.extend(i);
                map(self.remove_element, cons_list);
        self.get_elements().remove(element)

    def evaluate(self, expr):
        """
        Evaluate the expression.
        
        Args:
            expr: the string expression
        @throw NotImplementedError
        """
        raise NotImplementedError

    ##############################################
    ## Import/Export Methods
    ##############################################
    def export_data(self):
        """
        Export this flow graph to nested data.
        Export all block and connection data.
        
        Returns:
            a nested data odict
        """
        import time
        n = odict()
        n['timestamp'] = time.ctime()
        n['block'] = [block.export_data() for block in self.get_blocks()]
        n['connection'] = [connection.export_data() for connection in self.get_connections()]
        return odict({'flow_graph': n})

    def import_data(self, n=None):
        """
        Import blocks and connections into this flow graph.
        Clear this flowgraph of all previous blocks and connections.
        Any blocks or connections in error will be ignored.
        
        Args:
            n: the nested data odict
        """
        #remove previous elements
        self._elements = list()
        #use blank data if none provided
        fg_n = n and n.find('flow_graph') or odict()
        blocks_n = fg_n.findall('block')
        connections_n = fg_n.findall('connection')
        #create option block
        self._options_block = self.get_parent().get_new_block(self, 'options')
        #build the blocks
        for block_n in blocks_n:
            key = block_n.find('key')
            if key == 'options': block = self._options_block
            else: block = self.get_new_block(key)
            #only load the block when the block key was valid
            if block: block.import_data(block_n)
            else: Messages.send_error_load('Block key "%s" not found in %s'%(key, self.get_parent()))
        #build the connections
        for connection_n in connections_n:
            #try to make the connection
            try:
                #get the block ids
                source_block_id = connection_n.find('source_block_id')
                sink_block_id = connection_n.find('sink_block_id')
                #get the port keys
                source_key = connection_n.find('source_key')
                sink_key = connection_n.find('sink_key')
                #verify the blocks
                block_ids = map(lambda b: b.get_id(), self.get_blocks())
                if source_block_id not in block_ids:
                    raise LookupError('source block id "%s" not in block ids'%source_block_id)
                if sink_block_id not in block_ids:
                    raise LookupError('sink block id "%s" not in block ids'%sink_block_id)
                #get the blocks
                source_block = self.get_block(source_block_id)
                sink_block = self.get_block(sink_block_id)
                #verify the ports
                if source_key not in source_block.get_source_keys():
                    raise LookupError('source key "%s" not in source block keys'%source_key)
                if sink_key not in sink_block.get_sink_keys():
                    raise LookupError('sink key "%s" not in sink block keys'%sink_key)
                #get the ports
                source = source_block.get_source(source_key)
                sink = sink_block.get_sink(sink_key)
                #build the connection
                self.connect(source, sink)
            except LookupError, e: Messages.send_error_load(
                'Connection between %s(%s) and %s(%s) could not be made.\n\t%s'%(
                    source_block_id, source_key, sink_block_id, sink_key, e
                )
            )
        self.rewrite() #global rewrite
