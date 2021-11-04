# Copyright 2008-2016 Free Software Foundation, Inc.
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-2.0-or-later
#


from itertools import chain

from .. import blocks


class LoopError(Exception):
    pass


def upstream_ports(port):
    if port.is_sink:
        return _sources_from_virtual_sink_port(port)
    else:
        return _sources_from_virtual_source_port(port)


def _sources_from_virtual_sink_port(sink_port, _traversed=None):
    """
    Resolve the source port that is connected to the given virtual sink port.
    Use the get source from virtual source to recursively resolve subsequent ports.
    """
    source_ports_per_virtual_connection = (
        # there can be multiple ports per virtual connection
        _sources_from_virtual_source_port(
            c.source_port, _traversed)  # type: list
        for c in sink_port.connections(enabled=True)
    )
    # concatenate generated lists of ports
    return list(chain(*source_ports_per_virtual_connection))


def _sources_from_virtual_source_port(source_port, _traversed=None):
    """
    Recursively resolve source ports over the virtual connections.
    Keep track of traversed sources to avoid recursive loops.
    """
    _traversed = set(_traversed or [])  # a new set!
    if source_port in _traversed:
        raise LoopError('Loop found when resolving port type')
    _traversed.add(source_port)

    block = source_port.parent_block
    flow_graph = source_port.parent_flowgraph

    if not isinstance(block, blocks.VirtualSource):
        return [source_port]  # nothing to resolve, we're done

    stream_id = block.params['stream_id'].value

    # currently the validation does not allow multiple virtual sinks and one virtual source
    # but in the future it may...
    connected_virtual_sink_blocks = (
        b for b in flow_graph.iter_enabled_blocks()
        if isinstance(b, blocks.VirtualSink) and b.params['stream_id'].value == stream_id
    )
    source_ports_per_virtual_connection = (
        _sources_from_virtual_sink_port(b.sinks[0], _traversed)  # type: list
        for b in connected_virtual_sink_blocks
    )
    # concatenate generated lists of ports
    return list(chain(*source_ports_per_virtual_connection))


def downstream_ports(port):
    if port.is_source:
        return _sinks_from_virtual_source_port(port)
    else:
        return _sinks_from_virtual_sink_port(port)


def _sinks_from_virtual_source_port(source_port, _traversed=None):
    """
    Resolve the sink port that is connected to the given virtual source port.
    Use the get sink from virtual sink to recursively resolve subsequent ports.
    """
    sink_ports_per_virtual_connection = (
        # there can be multiple ports per virtual connection
        _sinks_from_virtual_sink_port(c.sink_port, _traversed)  # type: list
        for c in source_port.connections(enabled=True)
    )
    # concatenate generated lists of ports
    return list(chain(*sink_ports_per_virtual_connection))


def _sinks_from_virtual_sink_port(sink_port, _traversed=None):
    """
    Recursively resolve sink ports over the virtual connections.
    Keep track of traversed sinks to avoid recursive loops.
    """
    _traversed = set(_traversed or [])  # a new set!
    if sink_port in _traversed:
        raise LoopError('Loop found when resolving port type')
    _traversed.add(sink_port)

    block = sink_port.parent_block
    flow_graph = sink_port.parent_flowgraph

    if not isinstance(block, blocks.VirtualSink):
        return [sink_port]

    stream_id = block.params['stream_id'].value

    connected_virtual_source_blocks = (
        b for b in flow_graph.iter_enabled_blocks()
        if isinstance(b, blocks.VirtualSource) and b.params['stream_id'].value == stream_id
    )
    sink_ports_per_virtual_connection = (
        _sinks_from_virtual_source_port(b.sources[0], _traversed)  # type: list
        for b in connected_virtual_source_blocks
    )
    # concatenate generated lists of ports
    return list(chain(*sink_ports_per_virtual_connection))
