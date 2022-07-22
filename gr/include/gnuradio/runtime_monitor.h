#pragma once


#include <gnuradio/concurrent_queue.h>
#include <gnuradio/logger.h>
#include <gnuradio/runtime_proxy.h>
#include <map>
#include <thread>
#include <vector>

namespace gr {

enum class rt_monitor_message_t { UNKNOWN, DONE, FLUSHED, KILL, START };

class scheduler;

/**
 * @brief Messages received by runtime_monitor
 *
 */
class rt_monitor_message
{
public:
    using sptr = std::shared_ptr<rt_monitor_message>;
    static sptr make(rt_monitor_message_t type = rt_monitor_message_t::UNKNOWN,
                     int64_t schedid = -1,
                     int64_t blkid = -1)
    {
        return std::make_shared<rt_monitor_message>(type, schedid, blkid);
    }

    static std::map<rt_monitor_message_t, std::string> string_map;
    static std::map<std::string, rt_monitor_message_t> rev_string_map;
    rt_monitor_message(rt_monitor_message_t type = rt_monitor_message_t::UNKNOWN,
                       int64_t schedid = -1,
                       int64_t blkid = -1)
        : _type(type), _blkid(blkid), _schedid(schedid)
    {
    }
    rt_monitor_message_t type() { return _type; }
    int64_t schedid() { return _schedid; }
    void set_schedid(int64_t id) { _schedid = id; }
    int64_t blkid() { return _blkid; }

    std::string to_string();
    static sptr from_string(const std::string& str);

private:
    rt_monitor_message_t _type;
    int64_t _blkid;
    int64_t _schedid;
};

using rt_monitor_message_sptr = rt_monitor_message::sptr;

/**
 * @brief The runtime_monitor is responsible for tracking the start/stop status of
 * execution threads in the flowgraph
 *
 */
class runtime_monitor
{

public:
    runtime_monitor(std::vector<std::shared_ptr<scheduler>>& sched_ptrs,
                    std::vector<std::shared_ptr<runtime_proxy>>& proxy_ptrs,
                    const std::string& fgname = "");

    virtual ~runtime_monitor() {}

    virtual void push_message(rt_monitor_message_sptr msg) { msgq.push(msg); }
    void start()
    {
        push_message(rt_monitor_message::make(rt_monitor_message_t::START, 0, 0));
    }
    void stop()
    {
        push_message(rt_monitor_message::make(rt_monitor_message_t::KILL, 0, 0));
    }

private:
    bool _monitor_thread_stopped = false;
    std::vector<std::shared_ptr<scheduler>> d_schedulers;
    std::vector<std::shared_ptr<runtime_proxy>> d_runtime_proxies;

    logger_ptr d_logger, d_debug_logger;

protected:
    concurrent_queue<rt_monitor_message_sptr> msgq;
    virtual bool pop_message(rt_monitor_message_sptr& msg) { return msgq.pop(msg); }
    virtual void empty_queue() { msgq.clear(); }
};

using runtime_monitor_sptr = std::shared_ptr<runtime_monitor>;

} // namespace gr
