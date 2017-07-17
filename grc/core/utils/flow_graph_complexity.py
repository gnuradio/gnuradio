
def calculate(flowgraph):
    """ Determines the complexity of a flowgraph """
    return "  *** DISABLED ***  "  # TODO: Temporarily disabled.
    dbal = 0
    for block in flowgraph.blocks:
        # Skip options block
        if block.key == 'options':
            continue

        # Don't worry about optional sinks?
        sink_list = [c for c in block.sinks if not c.optional]
        source_list = [c for c in block.sources if not c.optional]
        sinks = float(len(sink_list))
        sources = float(len(source_list))
        base = max(min(sinks, sources), 1)

        # Port ratio multiplier
        if min(sinks, sources) > 0:
            multi = sinks / sources
            multi = (1 / multi) if multi > 1 else multi
        else:
            multi = 1

        # Connection ratio multiplier
        sink_multi = max(float(sum(len(c.connections()) for c in sink_list) / max(sinks, 1.0)), 1.0)
        source_multi = max(float(sum(len(c.connections()) for c in source_list) / max(sources, 1.0)), 1.0)
        dbal += base * multi * sink_multi * source_multi

    blocks = float(len(flowgraph.blocks))
    connections = float(len(flowgraph.connections))
    elements = blocks + connections
    disabled_connections = sum(not c.enabled for c in flowgraph.connections)

    variables = elements - blocks - connections
    enabled = float(len(flowgraph.get_enabled_blocks()))

    # Disabled multiplier
    if enabled > 0:
        disabled_multi = 1 / (max(1 - ((blocks - enabled) / max(blocks, 1)), 0.05))
    else:
        disabled_multi = 1

    # Connection multiplier (How many connections )
    if (connections - disabled_connections) > 0:
        conn_multi = 1 / (max(1 - (disabled_connections / max(connections, 1)), 0.05))
    else:
        conn_multi = 1

    final = round(max((dbal - 1) * disabled_multi * conn_multi * connections, 0.0) / 1000000, 6)
    return final
