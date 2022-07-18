#pragma once

#include <gnuradio/api.h>
#include <gnuradio/types.h>
#include <map>
#include <string>
#include <typeindex>
#include <typeinfo>

namespace gr {

/**
 * @brief Definition of supported types used for port parameters and port data types
 *
 */
enum class GR_RUNTIME_API param_type_t {
    UNTYPED,
    FLOAT,
    DOUBLE,
    CFLOAT,
    CDOUBLE,
    INT8,
    INT16,
    INT32,
    INT64,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    BOOL,
    ENUM,
    STRING,
    VOID
};

/**
 * @brief Utility Functions for converting param type enums to other datatypes
 *
 */
class GR_RUNTIME_API parameter_functions
{
private:
    static std::map<param_type_t, std::type_index> param_type_index_map;
    static std::map<param_type_t, size_t> param_type_size_map;
    static std::map<param_type_t, std::string> param_type_format_map;
    static std::map<std::type_index, param_type_t> param_index_type_map;

public:
    static size_t param_size_info(param_type_t p);
    static param_type_t get_param_type_from_typeinfo(std::type_index t);
    static std::string get_format_descriptor(param_type_t p);
};


} // namespace gr
