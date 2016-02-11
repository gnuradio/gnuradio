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


class FlowGraphProxy(object):

    def __init__(self, fg):
        self._fg = fg

    def __getattr__(self, item):
        return getattr(self._fg, item)

    def get_hier_block_stream_io(self, direction):
        """
        Get a list of stream io signatures for this flow graph.

        Args:
            direction: a string of 'in' or 'out'

        Returns:
            a list of dicts with: type, label, vlen, size, optional
        """
        return filter(lambda p: p['type'] != "message", self.get_hier_block_io(direction))

    def get_hier_block_message_io(self, direction):
        """
        Get a list of message io signatures for this flow graph.

        Args:
            direction: a string of 'in' or 'out'

        Returns:
            a list of dicts with: type, label, vlen, size, optional
        """
        return filter(lambda p: p['type'] == "message", self.get_hier_block_io(direction))

    def get_hier_block_io(self, direction):
        """
        Get a list of io ports for this flow graph.

        Args:
            direction: a string of 'in' or 'out'

        Returns:
            a list of dicts with: type, label, vlen, size, optional
        """
        pads = self.get_pad_sources() if direction in ('sink', 'in') else \
            self.get_pad_sinks() if direction in ('source', 'out') else []
        ports = []
        for pad in pads:
            master = {
                'label': str(pad.get_param('label').get_evaluated()),
                'type': str(pad.get_param('type').get_evaluated()),
                'vlen': str(pad.get_param('vlen').get_value()),
                'size': pad.get_param('type').get_opt('size'),
                'optional': bool(pad.get_param('optional').get_evaluated()),
            }
            num_ports = pad.get_param('num_streams').get_evaluated()
            if num_ports > 1:
                for i in xrange(num_ports):
                    clone = master.copy()
                    clone['label'] += str(i)
                    ports.append(clone)
            else:
                ports.append(master)
        return ports

    def get_pad_sources(self):
        """
        Get a list of pad source blocks sorted by id order.

        Returns:
            a list of pad source blocks in this flow graph
        """
        pads = filter(lambda b: b.get_key() == 'pad_source', self.get_enabled_blocks())
        return sorted(pads, lambda x, y: cmp(x.get_id(), y.get_id()))

    def get_pad_sinks(self):
        """
        Get a list of pad sink blocks sorted by id order.

        Returns:
            a list of pad sink blocks in this flow graph
        """
        pads = filter(lambda b: b.get_key() == 'pad_sink', self.get_enabled_blocks())
        return sorted(pads, lambda x, y: cmp(x.get_id(), y.get_id()))

    def get_pad_port_global_key(self, port):
        """
        Get the key for a port of a pad source/sink to use in connect()
        This takes into account that pad blocks may have multiple ports

        Returns:
            the key (str)
        """
        key_offset = 0
        pads = self.get_pad_sources() if port.is_source else self.get_pad_sinks()
        for pad in pads:
            # using the block param 'type' instead of the port domain here
            # to emphasize that hier block generation is domain agnostic
            is_message_pad = pad.get_param('type').get_evaluated() == "message"
            if port.get_parent() == pad:
                if is_message_pad:
                    key = pad.get_param('label').get_value()
                else:
                    key = str(key_offset + int(port.get_key()))
                return key
            else:
                # assuming we have either only sources or sinks
                if not is_message_pad:
                    key_offset += len(pad.get_ports())
        return -1