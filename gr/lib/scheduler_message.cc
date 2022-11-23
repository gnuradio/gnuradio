#include <gnuradio/scheduler_message.h>

#include <nlohmann/json.hpp>
#include <pmtv/pmt.hpp>

namespace gr {

std::string msgport_message::to_json()
{
    nlohmann::json ret;
    ret["type"] = "msgport_message";
    ret["msg"] = pmtv::to_base64(_msg);
    return ret.dump();
}
scheduler_message_sptr msgport_message::from_json(const std::string& str)
{
    auto json_obj = nlohmann::json::parse(str);
    if (json_obj["type"] != "msgport_message") {
        throw std::runtime_error("Invalid message type for msgport_message");
    }
    auto msg = pmtv::from_base64(str);
    return std::make_shared<msgport_message>(msg, nullptr);
}

} // namespace gr
