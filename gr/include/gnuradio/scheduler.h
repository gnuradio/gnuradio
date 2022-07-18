#pragma once

#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>

#include <gnuradio/buffer.h>
#include <gnuradio/flat_graph.h>
#include <gnuradio/logger.h>
#include <gnuradio/runtime_monitor.h>
#include <gnuradio/scheduler_message.h>
namespace gr {

/**
 * @brief Base class for GNU Radio Scheduler
 *
 */
class scheduler : public std::enable_shared_from_this<scheduler>,
                  public neighbor_interface
{
public:
    scheduler(const std::string& name)
    {
        _name = name;
        gr::configure_default_loggers(d_logger, d_debug_logger, name);
    };
    ~scheduler() override {}
    std::shared_ptr<scheduler> base() { return shared_from_this(); }
    virtual void initialize(flat_graph_sptr fg, runtime_monitor_sptr fgmon) = 0;
    void push_message(scheduler_message_sptr msg) override = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void wait() = 0;
    virtual void kill() = 0;

    std::string name() { return _name; }
    int id() { return _id; }
    void set_id(int id) { _id = id; }

    virtual void
    set_default_buffer_factory(std::shared_ptr<buffer_properties> bp = nullptr)
    {
        _default_buf_properties = bp;
    }

protected:
    logger_ptr d_logger;
    logger_ptr d_debug_logger;

    std::shared_ptr<buffer_properties> _default_buf_properties = nullptr;

private:
    std::string _name;
    int _id;
};

using scheduler_sptr = std::shared_ptr<scheduler>;

} // namespace gr
