#pragma once

#include <gnuradio/block.h>
#include <gnuradio/buffer.h>
#include <gnuradio/buffer_management.h>
#include <gnuradio/executor.h>

#include <map>

namespace gr {

/**
 * @brief Responsible for the execution of a graph
 *
 * Maintains the list of blocks for a particular thread and executes the workflow when
 * run_one_iteration is called
 *
 */
class graph_executor : public executor
{
private:
    std::vector<block_sptr> _blocks;
    static const int s_min_items_to_process = 1;
    static const int s_min_buf_items = 1;

public:
    graph_executor(const std::string& name, const std::vector<block_sptr>& blocks)
        : executor(name), _blocks(blocks)
    {
    }

    std::map<nodeid_t, executor_iteration_status_t> run_one_iteration(
        std::vector<block_sptr> blocks = std::vector<block_sptr>()) override;
};

} // namespace gr