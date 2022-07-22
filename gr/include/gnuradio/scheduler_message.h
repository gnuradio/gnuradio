#pragma once

#include <pmtf/wrap.hpp>

namespace gr {

enum class scheduler_action_t {
    SIGNAL_DONE,
    DONE,
    NOTIFY_OUTPUT,
    NOTIFY_INPUT,
    NOTIFY_ALL,
    EXIT
};

enum class scheduler_message_t {
    SCHEDULER_ACTION,
    MSGPORT_MESSAGE,
    PARAMETER_CHANGE,
    PARAMETER_QUERY,
};

class scheduler_message
{
public:
    scheduler_message() {}
    scheduler_message(scheduler_message_t type) : _type(type) {}
    virtual ~scheduler_message() {}
    scheduler_message_t type() { return _type; }
    void set_blkid(int64_t blkid_) { _blkid = blkid_; }
    int64_t blkid() { return _blkid; }
    virtual std::string to_json() { return "{ }"; }
    virtual std::shared_ptr<scheduler_message> from_json(const std::string& str)
    {
        throw std::runtime_error(
            "from_json not implemented in scheduler_message base class");
    }

private:
    scheduler_message_t _type;
    int64_t _blkid = -1;
};

using scheduler_message_sptr = std::shared_ptr<scheduler_message>;

class scheduler_action : public scheduler_message
{
public:
    scheduler_action(scheduler_action_t action, uint32_t blkid = 0)
        : scheduler_message(scheduler_message_t::SCHEDULER_ACTION), _action(action)
    {
        set_blkid(int64_t{ blkid });
    }
    scheduler_action_t action() { return _action; }

private:
    scheduler_action_t _action;
};

using scheduler_action_sptr = std::shared_ptr<scheduler_action>;


using message_port_callback_fcn = std::function<void(pmtf::pmt)>;
class msgport_message : public scheduler_message
{
public:
    msgport_message() {}
    msgport_message(pmtf::pmt msg, message_port_callback_fcn cb)
        : scheduler_message(scheduler_message_t::MSGPORT_MESSAGE), _msg(msg), _cb(cb)
    {
    }
    void set_callback(message_port_callback_fcn cb) { _cb = cb; }
    message_port_callback_fcn callback() { return _cb; }
    pmtf::pmt message() { return _msg; }
    std::string to_json() override;
    scheduler_message_sptr from_json(const std::string& str) override;

private:
    pmtf::pmt _msg;
    message_port_callback_fcn _cb;
};
using msgport_message_sptr = std::shared_ptr<msgport_message>;

} // namespace gr
