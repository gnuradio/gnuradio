#include <gnuradio/logger.h>
#include <gnuradio/node.h>
#include <vector>

namespace gr {

enum class executor_state_t { WORKING, DONE, FLUSHED, EXIT };
enum class executor_iteration_status_t {
    READY,           // We made progress; everything's cool.
    READY_NO_OUTPUT, // We consumed some input, but produced no output.
    BLKD_IN,         // no progress; we're blocked waiting for input data.
    BLKD_OUT,        // no progress; we're blocked waiting for output buffer space.
    DONE,            // we're done; don't call me again.
    MSG_ONLY,        // Block with no stream ports
};

class executor
{
protected:
    std::string _name;
    logger_ptr d_logger;
    logger_ptr d_debug_logger;

public:
    executor(const std::string& name) : _name(name)
    {
        gr::configure_default_loggers(d_logger, d_debug_logger, _name);
    }
    virtual ~executor() {}
    virtual std::map<nodeid_t, executor_iteration_status_t>
    run_one_iteration(std::vector<block_sptr> blocks = std::vector<block_sptr>()) = 0;
};

using executor_sptr = std::shared_ptr<executor>;

} // namespace gr