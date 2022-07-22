#include <gnuradio/parameter_types.h>

namespace gr {

std::map<param_type_t, std::type_index> parameter_functions::param_type_index_map = {
    { param_type_t::FLOAT, typeid(float) },
    { param_type_t::DOUBLE, typeid(double) },
    { param_type_t::CFLOAT, typeid(gr_complex) },
    { param_type_t::CDOUBLE, typeid(gr_complexd) },
    { param_type_t::INT8, typeid(int8_t) },
    { param_type_t::INT16, typeid(int16_t) },
    { param_type_t::INT32, typeid(int32_t) },
    { param_type_t::INT64, typeid(int64_t) },
    { param_type_t::UINT8, typeid(uint8_t) },
    { param_type_t::UINT16, typeid(uint16_t) },
    { param_type_t::UINT32, typeid(uint32_t) },
    { param_type_t::UINT64, typeid(uint64_t) },
    { param_type_t::BOOL, typeid(bool) },
    { param_type_t::ENUM, typeid(int) }, //??
    { param_type_t::STRING, typeid(std::string) },
    { param_type_t::VOID, typeid(void) }
};

std::map<param_type_t, std::string> parameter_functions::param_type_format_map = {
    { param_type_t::FLOAT, "f" },   { param_type_t::DOUBLE, "d" },
    { param_type_t::CFLOAT, "c8" }, { param_type_t::CDOUBLE, "c16" },
    { param_type_t::INT8, "b" },    { param_type_t::INT16, "h" },
    { param_type_t::INT32, "i" },   { param_type_t::INT64, "q" },
    { param_type_t::UINT8, "B" },   { param_type_t::UINT16, "H" },
    { param_type_t::UINT32, "I" },  { param_type_t::UINT64, "Q" },
    { param_type_t::BOOL, "?" },    { param_type_t::ENUM, "Q" }, //??
    { param_type_t::STRING, "s" },  { param_type_t::VOID, "p" }
};


std::map<std::type_index, param_type_t> parameter_functions::param_index_type_map = {
    { std::type_index(typeid(float)), param_type_t::FLOAT },
    { std::type_index(typeid(double)), param_type_t::DOUBLE },
    { std::type_index(typeid(gr_complex)), param_type_t::CFLOAT },
    { std::type_index(typeid(gr_complexd)), param_type_t::CDOUBLE },
    { std::type_index(typeid(int8_t)), param_type_t::INT8 },
    { std::type_index(typeid(int16_t)), param_type_t::INT16 },
    { std::type_index(typeid(int32_t)), param_type_t::INT32 },
    { std::type_index(typeid(int64_t)), param_type_t::INT64 },
    { std::type_index(typeid(uint8_t)), param_type_t::UINT8 },
    { std::type_index(typeid(uint16_t)), param_type_t::UINT16 },
    { std::type_index(typeid(uint32_t)), param_type_t::UINT32 },
    { std::type_index(typeid(uint64_t)), param_type_t::UINT64 },
    { std::type_index(typeid(bool)), param_type_t::BOOL },
    { std::type_index(typeid(int)), param_type_t::ENUM }, //??
    { std::type_index(typeid(std::string)), param_type_t::STRING },
    { std::type_index(typeid(void)), param_type_t::VOID }
};

std::map<param_type_t, size_t> parameter_functions::param_type_size_map = {
    { param_type_t::FLOAT, sizeof(float) },
    { param_type_t::DOUBLE, sizeof(double) },
    { param_type_t::CFLOAT, sizeof(gr_complex) },
    { param_type_t::CDOUBLE, sizeof(gr_complexd) },
    { param_type_t::INT8, sizeof(int8_t) },
    { param_type_t::INT16, sizeof(int16_t) },
    { param_type_t::INT32, sizeof(int32_t) },
    { param_type_t::INT64, sizeof(int64_t) },
    { param_type_t::UINT8, sizeof(uint8_t) },
    { param_type_t::UINT16, sizeof(uint16_t) },
    { param_type_t::UINT32, sizeof(uint32_t) },
    { param_type_t::UINT64, sizeof(uint64_t) },
    { param_type_t::BOOL, sizeof(bool) },
    { param_type_t::ENUM, sizeof(int) }, //??
    { param_type_t::STRING, sizeof(std::string) },
    { param_type_t::VOID, sizeof(void*) }
};

size_t parameter_functions::param_size_info(param_type_t p)
{
    return param_type_size_map[p];
}

param_type_t parameter_functions::get_param_type_from_typeinfo(std::type_index t)
{
    return param_index_type_map[t];
}

std::string parameter_functions::get_format_descriptor(param_type_t p)
{
    return param_type_format_map[p];
}
} // namespace gr
