#pragma once

#include <gnuradio/scheduler_options.h>
#include <string>

namespace gr {
namespace schedulers {

struct scheduler_nbt_options : public scheduler_options {
    using sptr = std::shared_ptr<scheduler_nbt_options>;
    static sptr make() { return std::make_shared<scheduler_nbt_options>(); }
    scheduler_nbt_options() : scheduler_options("nbt") {}

    static sptr opts_from_yaml(const std::string& options = "{}")
    {
        return std::static_pointer_cast<scheduler_nbt_options>(
            scheduler_options::opts_from_yaml(options));
        // Additional options
    }
};

using scheduler_nbt_options_sptr = scheduler_nbt_options::sptr;

} // namespace schedulers
} // namespace gr
