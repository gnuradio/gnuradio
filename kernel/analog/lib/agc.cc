#include <gnuradio/kernel/analog/agc.h>

namespace gr {
namespace kernel {
namespace analog {

template <typename T>
T agc<T>::scale(T input)
{
    T output = input * _gain;
    _gain += (_reference - fabsf(output)) * _rate;
    if (_max_gain > 0.0 && _gain > _max_gain)
        _gain = _max_gain;
    return output;
}

template <>
gr_complex agc<gr_complex>::scale(gr_complex input)
{
    gr_complex output = input * _gain;

    _gain += _rate * (_reference - std::sqrt(output.real() * output.real() +
                                             output.imag() * output.imag()));
    if (_max_gain > 0.0 && _gain > _max_gain) {
        _gain = _max_gain;
    }
    return output;
}


template class agc<float>;
template class agc<gr_complex>;

} // namespace analog
} // namespace kernel
} // namespace gr
