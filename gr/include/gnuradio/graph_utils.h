#pragma once

#include <gnuradio/graph.h>
#include <gnuradio/scheduler.h>

namespace gr {

/**
 * @brief Struct providing info for particular partition of flowgraph and how it relates
 * to the other partitions
 *
 * scheduler - the sptr to the scheduler for this partition
 * subgraph - the portion of the flowgraph controlled by this scheduler
 */
struct graph_partition_info {
    scheduler_sptr scheduler = nullptr;
    graph_sptr subgraph;
    bool operator==(const graph_partition_info& other)
    {
        return (scheduler == other.scheduler && subgraph == other.subgraph);
    }
};

using graph_partition_info_vec = std::vector<graph_partition_info>;

struct graph_utils {
    static std::pair<std::vector<graph_sptr>,
                     std::vector<std::tuple<edge_sptr, graph_sptr, graph_sptr>>>
    partition(graph_sptr input_graph, std::vector<std::vector<node_sptr>> nodes);

    static void connect_crossings(
        std::pair<std::vector<graph_sptr>,
                  std::vector<std::tuple<edge_sptr, graph_sptr, graph_sptr>>>&);
};
} // namespace gr
