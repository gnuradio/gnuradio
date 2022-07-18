#include <gnuradio/logger.h>

namespace gr {

enum class executor_state { WORKING, DONE, FLUSHED, EXIT };
enum class executor_iteration_status {
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
};

} // namespace gr