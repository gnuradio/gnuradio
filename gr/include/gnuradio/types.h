#pragma once

#include <gnuradio/gr_complex.h>
#include <pmtf/base.hpp>
#include <cstddef> // size_t
#include <cstdint>
#include <typeindex>
#include <vector>

using gr_vector_int = std::vector<int>;
using gr_vector_uint = std::vector<unsigned int>;
using gr_vector_float = std::vector<float>;
using gr_vector_double = std::vector<double>;
using gr_vector_void_star = std::vector<void*>;
using gr_vector_const_void_star = std::vector<const void*>;

using pmt_sptr = std::shared_ptr<pmtf::pmt>;
