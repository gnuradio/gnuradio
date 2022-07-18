#pragma once

#include <gnuradio/block.h>
#include <algorithm>
#include <limits>


namespace gr {

inline static unsigned int round_down(unsigned int n, unsigned int multiple)
{
    return (n / multiple) * multiple;
}


/**
 * @brief synchronous 1:1 input to output
 *
 */
class GR_RUNTIME_API sync_block : public block
{
public:
    sync_block(const std::string& name, const std::string& module = "")
        : block(name, module)
    {
    }

    /**
     * @brief Performs checks on inputs and outputs before and after the call
     * to the derived block's work function
     *
     * The sync_block guarantees that the input and output buffers to the
     * work function of the derived block fit the constraints of the 1:1
     * sample input/output relationship
     *
     * 1. Check all inputs and outputs have the same number of items
     * 2. Fix all inputs and outputs to the absolute min across ports
     * 3. Call the work() function on the derived block
     * 4. Throw runtime_error if n_produced is not the same on every port
     * 5. Set n_consumed = n_produced for every input port
     *
     * @param work_input
     * @param work_output
     * @return work_return_code_t
     */
    work_return_code_t do_work(work_io& wio) override
    {
        // Check all inputs and outputs have the same number of items
        auto min_num_items = std::numeric_limits<size_t>::max();
        for (auto& w : wio.inputs()) {
            min_num_items = std::min(min_num_items, w.n_items);
        }
        for (auto& w : wio.outputs()) {
            min_num_items = std::min(min_num_items, w.n_items);
        }

        if (output_multiple_set()) {
            min_num_items = round_down(min_num_items, output_multiple());
        }

        // all inputs and outputs need to be fixed to the absolute min
        for (auto& w : wio.inputs()) {
            w.n_items = min_num_items;
        }
        for (auto& w : wio.outputs()) {
            w.n_items = min_num_items;
        }

        for (auto& w : wio.outputs()) {
            if (w.n_items < output_multiple()) {
                return work_return_code_t::WORK_INSUFFICIENT_OUTPUT_ITEMS;
            }
        }

        work_return_code_t ret = work(wio);

        // For a sync block the n_produced must be the same on every
        // output port

        bool firsttime = true;
        size_t n_produced = 0;
        bool allsame = true;
        bool output_ports = false;
        for (auto& w : wio.outputs()) {
            if (firsttime) {
                output_ports = true;
                n_produced = w.n_produced;
                firsttime = false;
            }
            if (n_produced != w.n_produced) {
                allsame = false;
                break;
            }
        }
        if (!allsame) {
            throw new std::runtime_error(
                "outputs for sync_block must produce same number of items");
        }

        // by definition of a sync block the n_consumed must be equal to n_produced
        // also, a sync block must consume all of its items
        for (auto& w : wio.inputs()) {
            w.n_consumed = output_ports ? n_produced : w.n_items;
        }

        return ret;
    };
};
} // namespace gr
