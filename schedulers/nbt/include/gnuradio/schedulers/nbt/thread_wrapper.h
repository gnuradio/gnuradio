#pragma once

#include <gnuradio/block.h>
#include <gnuradio/block_group_properties.h>
#include <gnuradio/concurrent_queue.h>
#include <gnuradio/neighbor_interface.h>
#include <gnuradio/runtime_monitor.h>
#include <gnuradio/scheduler_message.h>
#include <condition_variable>
#include <mutex>
#include <thread>

#include "graph_executor.h"

namespace gr {
namespace schedulers {

/**
 * @brief Wrapper for scheduler thread
 *
 * Creates the worker thread that will process work for all blocks in the graph assigned
 * to this scheduler.  This is the core of the single threaded scheduler.
 *
 */
class thread_wrapper : public neighbor_interface
{
private:
    /**
     * @brief Single message queue for all types of messages to this thread
     *
     */
    concurrent_queue<scheduler_message_sptr> msgq;
    std::thread d_thread;
    bool d_thread_stopped = false;
    std::unique_ptr<graph_executor> _exec;

    int _id;
    block_group_properties d_block_group;
    std::vector<block_sptr> d_blocks;
    std::map<nodeid_t, block_sptr> d_block_id_to_block_map;

    logger_ptr d_logger;
    logger_ptr d_debug_logger;

    runtime_monitor_sptr d_rtmon;

    bool d_flushing = false;
    int d_flush_cnt = 0;
    std::atomic<bool> kick_pending = false;

public:
    using sptr = std::shared_ptr<thread_wrapper>;

    static sptr make(int id,
                     block_group_properties bgp,
                     buffer_manager::sptr bufman,
                     runtime_monitor_sptr rtmon)
    {
        return std::make_shared<thread_wrapper>(id, bgp, bufman, rtmon);
    }

    thread_wrapper(int id,
                   block_group_properties bgp,
                   buffer_manager::sptr bufman,
                   runtime_monitor_sptr rtmon);
    int id() { return _id; }
    const std::string& name() { return d_block_group.name(); }

    void push_message(scheduler_message_sptr msg) override { msgq.push(msg); }
    bool pop_message(scheduler_message_sptr& msg) { return msgq.pop(msg); }
    bool pop_message_nonblocking(scheduler_message_sptr& msg)
    {
        return msgq.try_pop(msg);
    }

    void start();
    void stop();
    void stop_blocks()
    {
        for (auto& b : d_blocks) {
            b->stop();
        }
    }
    void wait();
    void run();
    void kill();

    bool handle_work_notification();
    void handle_parameter_query(std::shared_ptr<param_query_action> item);
    void handle_parameter_change(std::shared_ptr<param_change_action> item);
    static void thread_body(thread_wrapper* top);

    void start_flushing()
    {
        d_flushing = true;
        d_flush_cnt = 0;
        push_message(
            std::make_shared<scheduler_action>(scheduler_action_t::NOTIFY_ALL, 0));
    }

    std::mutex _start_mutex;
    std::condition_variable _start_cv;
    bool _ready_to_start = false;
};
} // namespace schedulers
} // namespace gr
