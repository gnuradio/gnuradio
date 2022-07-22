#pragma once

#include <gnuradio/flowgraph.h>
#include <gnuradio/runtime_monitor.h>
#include <gnuradio/runtime_proxy.h>
#include <gnuradio/scheduler.h>

namespace gr {
class runtime
{
public:
    using sptr = std::shared_ptr<runtime>;
    static sptr make() { return std::make_shared<runtime>(); }
    runtime();
    void initialize(graph_sptr fg);
    void start();
    void stop();
    void wait();
    void run();
    void kill();
    /**
     * @brief Add a scheduler via a pair of scheduler and vector of blocks
     *
     * @param conf
     */
    void add_scheduler(std::pair<scheduler_sptr, std::vector<node_sptr>> conf);
    /**
     * @brief Add a scheduler with no associated blocks
     *
     * This Scheduler will be assigned all the blocks in the flowgraph
     *
     * @param sched
     */
    void add_scheduler(scheduler_sptr sched);

    void add_proxy(runtime_proxy_sptr proxy);


private:
    bool d_initialized = false;
    std::vector<scheduler_sptr> d_schedulers;
    std::vector<std::vector<node_sptr>> d_blocks_per_scheduler;
    const std::string s_default_scheduler_name = "nbt";
    scheduler_sptr d_default_scheduler = nullptr;
    bool d_default_scheduler_inuse = true;
    runtime_monitor_sptr d_rtmon;
    std::vector<runtime_proxy_sptr> d_runtime_proxies;
};
} // namespace gr
