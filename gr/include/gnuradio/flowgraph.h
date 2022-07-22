#pragma once

#include <gnuradio/flat_graph.h>
#include <gnuradio/graph.h>

namespace gr {

/**
 * @brief Top level graph representing the flowgraph
 *
 */
class runtime;
class flowgraph : public graph
{
public:
    using sptr = std::shared_ptr<flowgraph>;
    static sptr make(const std::string& name = "flowgraph")
    {
        return std::make_shared<flowgraph>(name);
    }
    flowgraph(const std::string& name = "flowgraph");
    ~flowgraph() override{};
    static void check_connections(const graph_sptr& g);
    flat_graph_sptr make_flat();

    void start();
    void stop();
    void wait();
    void run();
    void kill();

private:
    std::shared_ptr<gr::runtime> d_runtime_sptr = nullptr;
};

using flowgraph_sptr = flowgraph::sptr;
} // namespace gr
