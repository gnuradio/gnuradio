
def calculate(flowgraph):
    """ Determines the complexity of a flowgraph """

    try:
        dbal = 0.0
        for block in flowgraph.blocks:
            if block.key == "options":
                continue

            # Determine the base value for this block
            sinks = sum(1.0 for port in block.sinks if not port.optional)
            sources = sum(1.0 for port in block.sources if not port.optional)
            base = max(min(sinks, sources), 1)

            # Determine the port multiplier
            block_connections = 0.0
            for port in block.sources:
                block_connections += sum(1.0 for c in port.connections())
            source_multi = max(block_connections / max(sources, 1.0), 1.0)

            # Port ratio multiplier
            multi = 1.0
            if min(sinks, sources) > 0:
                multi = float(sinks / sources)
                multi = float(1 / multi) if multi > 1 else multi

            dbal += base * multi * source_multi

        blocks = float(len(flowgraph.blocks) - 1)
        connections = float(len(flowgraph.connections))
        variables = float(len(flowgraph.get_variables()))

        enabled = float(len(flowgraph.get_enabled_blocks()))
        enabled_connections = float(len(flowgraph.get_enabled_connections()))
        disabled_connections = connections - enabled_connections

        # Disabled multiplier
        if enabled > 0:
            disabled_multi = 1 / \
                (max(1 - ((blocks - enabled) / max(blocks, 1)), 0.05))
        else:
            disabled_multi = 1

        # Connection multiplier (How many connections )
        if (connections - disabled_connections) > 0:
            conn_multi = 1 / \
                (max(1 - (disabled_connections / max(connections, 1)), 0.05))
        else:
            conn_multi = 1

        final = round(max((dbal - 1) * disabled_multi *
                      conn_multi * connections, 0.0) / 1000000, 6)
        return final

    except Exception:
        return "<Error>"
