"""
Copyright 2026 Ettus Research, a National Instruments Brand
This file is part of GNU Radio

SPDX-License-Identifier: GPL-2.0-or-later

Toolkit-independent algorithm for automatically connecting the ports of a set
of blocks. The GUI supplies the geometry of the ports via a callback, so this
module can be shared between the GTK and the Qt implementation of GRC.
"""

import math

from ..Constants import ALIASES_OF

# How much a sideways offset between two ports is penalized compared to the
# distance along the port direction (makes in-line ports preferable).
PERPENDICULAR_WEIGHT = 1.0
# Penalty factor for ports that point slightly away from each other
BACKWARDS_WEIGHT = 4.0
# A port may point away from its peer by this fraction of their distance (plus
# a fixed margin) and still be considered. This allows e.g. "loop back" layouts
# where a row of blocks continues below in the opposite direction.
BACKWARDS_TOLERANCE_REL = 0.25
BACKWARDS_TOLERANCE_ABS = 20.0


def _types_match(source, sink):
    """Check if source and sink port can be connected based on their types."""
    platform = source.parent_platform
    if (source.domain, sink.domain) not in platform.connection_templates:
        return False
    # Ports with empty type are wildcards (e.g. virtual blocks, pads)
    if source.inherit_type or sink.inherit_type:
        return True
    if source.dtype != sink.dtype and source.dtype not in ALIASES_OF.get(sink.dtype, set()):
        return False
    return source.vlen == sink.vlen


def _cost(source_geometry, sink_geometry):
    """Return the cost of connecting two ports based on their geometry.

    Return None if the ports are not facing each other.

    Each geometry is a tuple ((x, y), (dx, dy)) of the absolute connection
    point and the unit vector the port is pointing to.
    """
    (src_x, src_y), (src_dx, src_dy) = source_geometry
    (snk_x, snk_y), (snk_dx, snk_dy) = sink_geometry
    vx, vy = snk_x - src_x, snk_y - src_y
    distance = math.hypot(vx, vy)
    # How far the sink is in front of the source, and vice versa
    along_source = vx * src_dx + vy * src_dy
    along_sink = -(vx * snk_dx + vy * snk_dy)
    tolerance = BACKWARDS_TOLERANCE_ABS + BACKWARDS_TOLERANCE_REL * distance
    if along_source < -tolerance or along_sink < -tolerance:
        return None
    perpendicular = abs(vx * src_dy - vy * src_dx)
    backwards = max(0.0, -along_source) + max(0.0, -along_sink)
    return distance + PERPENDICULAR_WEIGHT * perpendicular + BACKWARDS_WEIGHT * backwards


def _uncross(matches):
    """'Uncross' connection lines.

    Reorder matches between the same pair of blocks such that port indices are
    connected in ascending order (out0->in0, out1->in1, ...), which avoids
    crossing connections. Only applied if the ports remain type compatible.
    """
    by_block_pair = {}
    for source, sink in matches:
        by_block_pair.setdefault((source.parent_block, sink.parent_block), []).append((source, sink))
    result = []
    for (src_block, snk_block), pairs in by_block_pair.items():
        sources = sorted((s for s, _ in pairs), key=src_block.sources.index)
        sinks = sorted((k for _, k in pairs), key=snk_block.sinks.index)
        reordered = list(zip(sources, sinks))
        if all(_types_match(s, k) for s, k in reordered):
            pairs = reordered
        result.extend(pairs)
    return result


def find_connections(blocks, port_geometry):
    """
    Determine which connections to make to auto-connect the given blocks.

    Args:
        blocks: iterable of core blocks to connect amongst each other
        port_geometry: callable taking a port and returning a tuple
            ((x, y), (dx, dy)): the absolute coordinate at which connections
            attach to the port, and the unit vector of the direction the port
            points to (in canvas coordinates).

    Returns:
        a list of (source_port, sink_port) tuples, which are not yet connected
    """
    blocks = list(blocks)

    def visible(ports):
        return [p for p in ports if not p.hidden]

    sources = [p for b in blocks for p in visible(b.sources)]
    sinks = [p for b in blocks for p in visible(b.sinks)]
    geometry = {port: port_geometry(port) for port in sources + sinks}

    candidates = []
    for source in sources:
        for sink in sinks:
            if source.parent_block is sink.parent_block:
                continue
            if not _types_match(source, sink):
                continue
            cost = _cost(geometry[source], geometry[sink])
            if cost is not None:
                candidates.append((cost, source, sink))
    candidates.sort(key=lambda c: c[0])

    used_sinks = {sink for sink in sinks if any(sink.connections())}
    used_sources = {source for source in sources if any(source.connections())}

    # Pass 1: one-to-one matching of unconnected ports, closest (i.e., least "costly"
    # ports first
    matches = []
    for _, source, sink in candidates:
        if source in used_sources or sink in used_sinks:
            continue
        matches.append((source, sink))
        used_sources.add(source)
        used_sinks.add(sink)
    matches = _uncross(matches)

    # Pass 2: fan out to remaining required sinks, if the domain allows it.
    # A source never feeds more than one input of the same block this way.
    fed_blocks = {(source, sink.parent_block) for source, sink in matches}
    fed_blocks.update((source, con.sink_block) for source in sources for con in source.connections())
    for _, source, sink in candidates:
        if sink in used_sinks or sink.optional:
            continue
        if (source, sink.parent_block) in fed_blocks:
            continue
        domain = source.parent_platform.domains.get(source.domain)
        if domain is None or not domain.multi_out:
            continue
        matches.append((source, sink))
        used_sinks.add(sink)
        fed_blocks.add((source, sink.parent_block))

    return matches
