#include "graph_executor.h"

namespace gr {
namespace schedulers {

inline static unsigned int round_down(unsigned int n, unsigned int multiple)
{
    return (n / multiple) * multiple;
}

static void post_work_cleanup(work_io& wio)
{
    // Decrement active counts for all inputs and outputs
    for (auto& outp : wio.outputs()) {
        outp.buf().decrement_active();
    }

    for (auto& inp : wio.inputs()) {
        inp.buf().decrement_active();
    }
}

void log_work_status(logger_ptr& logger, const block_sptr& b, work_io& wio)
{
    if (!wio.outputs().empty()) {
        logger->debug(
            "do_work (output) for {}, {}", b->alias(), wio.outputs()[0].n_items);
    }
    else if (!wio.inputs().empty()) {
        logger->debug("do_work (input) for {}, {}", b->alias(), wio.inputs()[0].n_items);
    }
    else {
        logger->debug("do_work for {}", b->alias());
    }
}


std::map<nodeid_t, executor_iteration_status_t>
graph_executor::run_one_iteration(std::vector<block_sptr> blocks)
{
    std::map<nodeid_t, executor_iteration_status_t> per_block_status;

    // If no blocks are specified for the iteration, then run over all the blocks
    // in the default ordering
    if (blocks.empty()) {
        blocks = d_blocks;
    }

    for (auto const& b : blocks) { // TODO - order the blocks
        if (b->is_hier()) {
            continue;
        }

        work_io& wio = b->get_work_io();
        wio.reset();

        // If a block is a message port only block, it will raise the finished() flag
        // to indicate that the rest of the flowgraph should clean up
        if (b->finished()) {
            per_block_status[b->id()] = executor_iteration_status_t::DONE;
            d_debug_logger->debug("pbs[{}]: {}", b->id(), (int)per_block_status[b->id()]);
            continue;
        }

        if (wio.inputs().empty() && wio.outputs().empty()) {
            // There is no streaming work to do for this block
            per_block_status[b->id()] = executor_iteration_status_t::MSG_ONLY;
            continue;
        }

        // for each input port of the block
        bool ready = true;
        for (auto& w : wio.inputs()) {
            auto p_buf = w.bufp();
            if (p_buf) {
                auto max_read = p_buf->max_buffer_read();
                auto min_read = p_buf->min_buffer_read();

                buffer_info_t read_info;
                p_buf->increment_active();
                ready = p_buf->read_info(read_info);
                d_debug_logger->debug("read_info {} - {} - {}, total: {}",
                                      b->alias(),
                                      read_info.n_items,
                                      read_info.item_size,
                                      read_info.total_items);

                if (read_info.n_items < s_min_items_to_process ||
                    (min_read > 0 && read_info.n_items < (int)min_read)) {

                    if (p_buf->input_blkd_cb_ready(read_info.n_items + 1)) {
                        gr::custom_lock lock(std::ref(*p_buf->mutex()), p_buf->bufp());

                        if (p_buf->input_blocked_callback(s_min_items_to_process)) {
                            w.port->notify_scheduler_action(
                                scheduler_action_t::NOTIFY_OUTPUT);
                        }
                    }

                    ready = false;
                    break;
                }

                if (max_read > 0 && read_info.n_items > (int)max_read) {
                    read_info.n_items = max_read;
                }


                auto tags = p_buf->get_tags(read_info.n_items);
                w.n_items = read_info.n_items;
            }
        }

        if (!ready) {
            per_block_status[b->id()] = executor_iteration_status_t::BLKD_IN;
            post_work_cleanup(wio);
            continue;
        }

        // for each output port of the block
        for (auto& w : wio.outputs()) {

            size_t max_output_buffer = std::numeric_limits<int>::max();

            auto p_buf = w.bufp();
            if (p_buf) {
                auto max_fill = p_buf->max_buffer_fill();
                auto min_fill = p_buf->min_buffer_fill();

                buffer_info_t write_info;
                p_buf->increment_active();
                ready = p_buf->write_info(write_info);
                d_debug_logger->debug("write_info {} - {} @ {} {}, total: {}",
                                      b->alias(),
                                      write_info.n_items,
                                      write_info.ptr,
                                      write_info.item_size,
                                      write_info.total_items);

                size_t tmp_buf_size = write_info.n_items;
                if (tmp_buf_size < s_min_buf_items ||
                    (min_fill > 0 && tmp_buf_size < min_fill)) {
                    ready = false;
                    if (p_buf->output_blkd_cb_ready(tmp_buf_size + 1)) {
                        gr::custom_lock lock(std::ref(*p_buf->mutex()), p_buf);
                        if (p_buf->output_blocked_callback(false)) {
                            w.port->notify_scheduler_action(
                                scheduler_action_t::NOTIFY_INPUT);
                        }
                    }
                    break;
                }

                if (tmp_buf_size < max_output_buffer)
                    max_output_buffer = tmp_buf_size;

                if (max_fill > 0 && max_output_buffer > max_fill) {
                    max_output_buffer = max_fill;
                }

                // if (b->output_multiple_set()) {
                //     d_debug_logger->debug("output_multiple {}", b->output_multiple());
                //     max_output_buffer =
                //         round_down(max_output_buffer, b->output_multiple());
                // }

                // if (max_output_buffer <= 0) {
                //     if (p_buf->output_blkd_cb_ready(b->output_multiple())) {
                //         gr::custom_lock lock(std::ref(*p_buf->mutex()), p_buf);
                //         if (p_buf->output_blocked_callback()) {
                //             w.port->notify_scheduler_action(
                //                 scheduler_action_t::NOTIFY_INPUT);
                //         }
                //     }
                //     ready = false;
                // }

                if (!ready)
                    break;

                w.n_items = max_output_buffer;
            }
        }

        if (!ready) {
            per_block_status[b->id()] = executor_iteration_status_t::BLKD_OUT;
            post_work_cleanup(wio);
            continue;
        }

        if (ready) {
            work_return_t ret;


            log_work_status(d_debug_logger, b, wio);
            ret = b->do_work(wio);
            d_debug_logger->debug("do_work returned {}", (int)ret);
            // ret = work_return_t::OK;

            if (ret == work_return_t::DONE) {
                per_block_status[b->id()] = executor_iteration_status_t::DONE;
                d_debug_logger->debug(
                    "pbs[{}]: {}", b->id(), (int)per_block_status[b->id()]);
            }
            else if (ret == work_return_t::OK) {
                per_block_status[b->id()] = executor_iteration_status_t::READY;
                d_debug_logger->debug(
                    "pbs[{}]: {}", b->id(), (int)per_block_status[b->id()]);

                // If a source block, and no outputs were produced, mark as BLKD_IN
                if (wio.inputs().empty() && !wio.outputs().empty()) {
                    size_t max_output = 0;
                    for (auto& w : wio.outputs()) {
                        max_output = std::max(w.n_produced, max_output);
                    }
                    if (max_output <= 0) {
                        per_block_status[b->id()] = executor_iteration_status_t::BLKD_IN;
                        d_debug_logger->debug(
                            "pbs[{}]: {}", b->id(), (int)per_block_status[b->id()]);
                    }
                }
            }
            else if (ret == work_return_t::INSUFFICIENT_INPUT_ITEMS) {

                per_block_status[b->id()] = executor_iteration_status_t::BLKD_IN;

                // // call the input blocked callback
                // bool notify = false;
                // for (auto& w : wio.inputs()) {
                //     if (w.buf().input_blkd_cb_ready(b->output_multiple())) {
                //         gr::custom_lock lock(std::ref(*w.bufp()->mutex()),
                //                              w.buf().bufp());
                //         notify |= w.buf().input_blocked_callback(b->output_multiple());
                //     }
                // }
                // if (notify) {
                //     // FIXME: Should just return RDY
                //     wio.inputs()[0].port->push_message(std::make_shared<scheduler_action>(
                //         scheduler_action_t::NOTIFY_INPUT));
                // }
                // per_block_status[b->id()] =
                // executor_iteration_status_t::READY_NO_OUTPUT;
                d_debug_logger->debug(
                    "pbs[{}]: {}", b->id(), (int)per_block_status[b->id()]);
            }
            else if (ret == work_return_t::INSUFFICIENT_OUTPUT_ITEMS) {
                per_block_status[b->id()] = executor_iteration_status_t::BLKD_OUT;
                d_debug_logger->debug(
                    "pbs[{}]: {}", b->id(), (int)per_block_status[b->id()]);
                // call the output blocked callback
            }

            // TODO - handle READY_NO_OUTPUT

            if (ret == work_return_t::OK || ret == work_return_t::DONE) {


                int input_port_index = 0;
                for (auto& w : wio.inputs()) {
                    auto p_buf = w.bufp();
                    if (p_buf) {
                        if (!p_buf->tags().empty()) {
                            // Pass the tags according to TPP
                            if (b->tag_propagation_policy() ==
                                tag_propagation_policy_t::TPP_ALL_TO_ALL) {
                                for (auto wo : wio.outputs()) {
                                    auto p_out_buf = wo.bufp();
                                    p_out_buf->propagate_tags(p_buf, w.n_consumed);
                                }
                            }
                            else if (b->tag_propagation_policy() ==
                                     tag_propagation_policy_t::TPP_ONE_TO_ONE) {
                                int output_port_index = 0;
                                for (auto wo : wio.outputs()) {
                                    if (output_port_index == input_port_index) {
                                        auto p_out_buf = wo.bufp();
                                        p_out_buf->propagate_tags(p_buf, w.n_consumed);
                                    }
                                    output_port_index++;
                                }
                            }
                        }

                        d_debug_logger->debug("post_read {} - {}, total: {}",
                                              b->alias(),
                                              w.n_consumed,
                                              w.nitems_read());

                        p_buf->post_read(w.n_consumed);
                        w.port->notify_scheduler_action(
                            scheduler_action_t::NOTIFY_OUTPUT);

                        input_port_index++;
                    }
                }

                for (auto& wo : wio.outputs()) {
                    auto p_buf = wo.bufp();
                    if (p_buf) {
                        d_debug_logger->debug("post_write {} - {}, total: {}",
                                              b->alias(),
                                              wo.n_produced,
                                              wo.nitems_written());
                        p_buf->post_write(wo.n_produced);
                        wo.port->notify_scheduler_action(
                            scheduler_action_t::NOTIFY_INPUT);
                        p_buf->prune_tags();
                    }
                }
            }
        }
        post_work_cleanup(wio);
    }


    return per_block_status;
}

} // namespace schedulers
} // namespace gr
