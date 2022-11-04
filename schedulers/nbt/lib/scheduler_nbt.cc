#include <gnuradio/prefs.h>
#include <gnuradio/schedulers/nbt/scheduler_nbt.h>
#include <yaml-cpp/yaml.h>
namespace gr {
namespace schedulers {

void scheduler_nbt::push_message(scheduler_message_sptr msg)
{
    // Use 0 for blkid all threads
    if (msg->blkid() == 0) {
        for (auto element : _block_thread_map) {
            auto thd = element.second;
            thd->push_message(msg);
        }
    }
    else {
        _block_thread_map[msg->blkid()]->push_message(msg);
    }
}

void scheduler_nbt::add_block_group(const std::vector<block_sptr>& blocks,
                                    const std::string& name,
                                    const std::vector<unsigned int>& affinity_mask)
{
    _block_groups.push_back(
        std::move(block_group_properties(blocks, name, affinity_mask)));
}

void scheduler_nbt::initialize(flat_graph_sptr fg, runtime_monitor_sptr fgmon)
{

    auto bufman = std::make_shared<buffer_manager>(_opts->default_buffer_size);
    bufman->initialize_buffers(fg, _default_buf_properties, base());

    //  Partition the flowgraph according to how blocks are specified in groups
    //  By default, one Thread Per Block

    auto blocks = fg->calc_used_blocks();
    for (auto& b : blocks) {
        b->populate_work_io();
    }

    // look at our block groups, create confs and remove from blocks
    for (auto& bg : _block_groups) {
        std::vector<block_sptr> blocks_for_this_thread;

        if (!bg.blocks().empty()) {
            auto t = thread_wrapper::make(
                id(),
                bg,
                std::make_shared<graph_executor>(bg.name(), blocks_for_this_thread),
                fgmon,
                _opts);
            _threads.push_back(t);

            for (auto& b : bg.blocks()) {
                auto it = std::find(blocks.begin(), blocks.end(), b);
                if (it != blocks.end()) {
                    blocks.erase(it);
                }

                b->set_parent_intf(t);
                for (auto& p : b->all_ports()) {
                    p->set_parent_intf(t); // give a shared pointer to the scheduler class
                }
                _block_thread_map[b->id()] = t;
            }
        }
    }

    // For the remaining blocks that weren't in block groups
    for (auto& b : blocks) {
        auto t = thread_wrapper::make(
            id(),
            block_group_properties({ b }),
            std::make_shared<graph_executor>(b->alias(), std::vector<block_sptr>{ b }),
            fgmon,
            _opts);
        _threads.push_back(t);

        b->set_parent_intf(t);
        for (auto& p : b->all_ports()) {
            p->set_parent_intf(t); // give a shared pointer to the scheduler class
        }

        _block_thread_map[b->id()] = t;
    }
}

void scheduler_nbt::start()
{
    for (const auto& thd : _threads) {
        thd->start();
    }
}
void scheduler_nbt::stop()
{
    for (const auto& thd : _threads) {
        thd->stop();
    }
}
void scheduler_nbt::wait()
{
    for (const auto& thd : _threads) {
        thd->wait();
    }
}
void scheduler_nbt::run()
{
    for (const auto& thd : _threads) {
        thd->start();
    }
    for (const auto& thd : _threads) {
        thd->wait();
    }
}

void scheduler_nbt::kill()
{
    for (const auto& thd : _threads) {
        thd->kill();
    }
}


} // namespace schedulers
} // namespace gr


// External plugin interface for instantiating out of tree schedulers
// TODO: name, version, other info methods
extern "C" {
std::shared_ptr<gr::scheduler> factory(const std::string& options)
{
    return gr::schedulers::scheduler_nbt::make(
        gr::schedulers::scheduler_nbt_options::opts_from_yaml(options));
}
}
