#include <gnuradio/block.h>
#include <gnuradio/pyblock_detail.h>
#include <gnuradio/scheduler.h>
#include <gnuradio/scheduler_message.h>
#include <nlohmann/json.hpp>
#include <pmtf/wrap.hpp>
#include <atomic>
#include <chrono>
#include <thread>

namespace gr {

block::block(const std::string& name, const std::string& module)
    : node(name),
      s_module(module),
      d_tag_propagation_policy(tag_propagation_policy_t::TPP_ALL_TO_ALL)
{
    // {# add message handler port for parameter updates#}
    auto msg_param_update = message_port::make("param_update", port_direction_t::INPUT);
    msg_param_update->register_callback(
        [this](pmtf::pmt msg) { this->handle_msg_param_update(msg); });
    add_port(std::move(msg_param_update));

    auto msg_system = message_port::make("system", port_direction_t::INPUT);
    msg_system->register_callback(
        [this](pmtf::pmt msg) { this->handle_msg_system(msg); });
    add_port(std::move(msg_system));
}

void block::set_pyblock_detail(std::shared_ptr<pyblock_detail> p)
{
    d_pyblock_detail = p;
}
std::shared_ptr<pyblock_detail> block::pb_detail() { return d_pyblock_detail; }
bool block::start()
{
    d_running = true;
    return true;
}

bool block::stop()
{
    d_running = false;
    return true;
}
bool block::done()
{
    d_running = false;
    return true;
}

void block::populate_work_io()
{
    d_work_io.clear();
    for (auto& p : input_stream_ports()) {
        d_work_io.add_input(p);
    }
    for (auto& p : output_stream_ports()) {
        d_work_io.add_output(p);
    }
}

work_io& block::get_work_io() { return d_work_io; }

tag_propagation_policy_t block::tag_propagation_policy()
{
    return d_tag_propagation_policy;
};

void block::set_tag_propagation_policy(tag_propagation_policy_t policy)
{
    d_tag_propagation_policy = policy;
};


void block::on_parameter_change(param_action_sptr action)
{
    d_debug_logger->debug(
        "block {}: on_parameter_change param_id: {}", id(), action->id());
    auto param = d_parameters.get(action->id());
    *param = action->pmt_value();
}

void block::on_parameter_query(param_action_sptr action)
{
    d_debug_logger->debug(
        "block {}: on_parameter_query param_id: {}", id(), action->id());
    auto param = d_parameters.get(action->id());
    action->set_pmt_value(*param);
}

void block::set_output_multiple(size_t multiple)
{
    if (multiple < 1)
        throw std::invalid_argument("block::set_output_multiple");

    d_output_multiple_set = true;
    d_output_multiple = multiple;
}

void block::handle_msg_param_update(pmtf::pmt msg)
{
    // Update messages are a pmtf::map with the name of
    // the param as the "id" field, and the pmt::wrap
    // that holds the update as the "value" field

    auto id = pmtf::string(pmtf::map(msg)["id"]).data();
    auto value = pmtf::map(msg)["value"];

    request_parameter_change(get_param_id(id), value, false);
}

void block::handle_msg_system(pmtf::pmt msg)
{
    auto str_msg = pmtf::get_as<std::string>(msg);
    if (str_msg == "done") {
        d_finished = true;
        p_scheduler->push_message(
            std::make_shared<scheduler_action>(scheduler_action_t::SIGNAL_DONE, id()));
    }
}

void block::request_parameter_change(int param_id, pmtf::pmt new_value, bool block)
{
    if (rpc_client() && !rpc_name().empty()) {
        rpc_client()->block_parameter_change(
            rpc_name(), get_param_str(param_id), new_value.to_base64());
    }
    else if (p_scheduler && d_running) {
        std::condition_variable cv;
        std::mutex m;
        bool ready{ false };
        auto lam = [&](param_action_sptr a) {
            {
                std::unique_lock<std::mutex> lk(m);
                ready = true;
            }
            cv.notify_all();
        };

        if (block) {
            p_scheduler->push_message(std::make_shared<param_change_action>(
                id(), param_action::make(param_id, new_value, 0), lam));

            // block until confirmation that parameter has been set
            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, [&ready]() { return ready == true; });
        }
        else {
            p_scheduler->push_message(std::make_shared<param_change_action>(
                id(), param_action::make(param_id, new_value, 0), nullptr));
        }
    }
    // else go ahead and update parameter value
    else {
        on_parameter_change(param_action::make(param_id, new_value, 0));
    }
}

pmtf::pmt block::request_parameter_query(int param_id)
{

    if (rpc_client() && !rpc_name().empty()) {
        auto encoded_str =
            rpc_client()->block_parameter_query(rpc_name(), get_param_str(param_id));
        return pmtf::pmt::from_base64(encoded_str);
    }
    // call back to the scheduler if ptr is not null
    else if (p_scheduler && d_running) {
        std::condition_variable cv;
        std::mutex m;
        pmtf::pmt newval;
        bool ready{ false };
        auto lam = [&](param_action_sptr a) {
            {
                std::unique_lock<std::mutex> lk(m);
                newval = a->pmt_value();
                ready = true;
            }
            cv.notify_all();
        };

        auto msg =
            std::make_shared<param_query_action>(id(), param_action::make(param_id), lam);
        p_scheduler->push_message(msg);

        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&ready]() { return ready == true; });
        return newval;
    }
    // else go ahead and return parameter value
    else {
        auto action = param_action::make(param_id);
        on_parameter_query(action);
        return action->pmt_value();
    }
}

void block::notify_scheduler()
{
    if (p_scheduler) {
        this->p_scheduler->push_message(
            std::make_shared<scheduler_action>(scheduler_action_t::NOTIFY_ALL));
    }
}

void block::notify_scheduler_input()
{
    if (p_scheduler) {
        this->p_scheduler->push_message(
            std::make_shared<scheduler_action>(scheduler_action_t::NOTIFY_INPUT));
    }
}

void block::notify_scheduler_output()
{
    if (p_scheduler) {
        this->p_scheduler->push_message(
            std::make_shared<scheduler_action>(scheduler_action_t::NOTIFY_OUTPUT));
    }
}

std::string block::to_json()
{
    // Example string describing this block
    // {"module": "blocks", "id": "copy", "properties": {"itemsize": 8}}

    nlohmann::json json_obj;

    json_obj["module"] = s_module;
    json_obj["id"] = name() + suffix();
    json_obj["format"] = "b64";
    for (auto [key, val] : d_parameters.param_map) {
        auto encoded_str = val->to_base64();
        json_obj["parameters"][key] = encoded_str;
    }

    return json_obj.dump();
}

void block::from_json(const std::string& json_str)
{
    using json = nlohmann::json;
    auto json_obj = json::parse(json_str);
    for (auto& [key, value] : json_obj["parameters"].items()) {
        // deserialize from the b64 string
        auto p = pmtf::pmt::from_base64(value.get<std::string>());
        auto block_pmt = d_parameters.get(key);
        *block_pmt = p;
    }
}

// This should go into pmt
pmtf::pmt block::deserialize_param_to_pmt(const std::string& encoded_str)
{
    return pmtf::pmt::from_base64(encoded_str);
}


void block::come_back_later(size_t count_ms)
{
    if (!p_scheduler) {
        return;
    }
    // Launch a thread to come back and try again some time later

    std::atomic<bool> d_sleeping = true;
    std::thread t([this, count_ms]() {
        d_debug_logger->debug("Setting timer to notify scheduler in {} ms", count_ms);
        std::this_thread::sleep_for(std::chrono::milliseconds(count_ms));
        std::atomic<bool> d_sleeping = false;
        p_scheduler->push_message(
            std::make_shared<scheduler_action>(scheduler_action_t::NOTIFY_INPUT));
    });
    t.detach();
}

} // namespace gr
