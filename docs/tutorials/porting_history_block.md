# Porting a Block with History

Since GR4 no longer exposes `history` to the scheduler, this tutorial goes through the 
steps to outline how to make a block with no `history` behave as if it did

We will port the `moving_average` block as an example

## Creating the block


``` 
cd $GR_PREFIX/src/gnuradio/blocklib/filter
python3 $GR_PREFIX/src/gnuradio/utils/modtool/create_block.py moving_average --templated --intree
```

## The .yml

```yml
module: filter
block: moving_average
label: Moving Average
blocktype: block
```

Restrict the `typekeys` to float and complex:
```yml
typekeys:
  - id: T
    type: class
    options: 
      - cf32
      - rf32
```

Add in the parameters:
```yml
parameters:
-   id: length
    label: Length
    dtype: size
    settable: true
-   id: scale
    label: Scale
    dtype: T
    settable: true
-   id: max_iter
    label: Max Iter
    dtype: size
    settable: false
    default: 4096
-   id: vlen
    label: Vector Length
    dtype: size
    settable: false
    default: 1
```

The defaults given for the ports, are actually fine:
```yml
ports:
-   domain: stream
    id: in
    direction: input
    type: typekeys/T
    shape: parameters/vlen

-   domain: stream
    id: out
    direction: output
    type: typekeys/T
    shape: parameters/vlen
```

Now, build to autogenerate the files

## Implementation header - moving_average_cpu.h

When porting to 4.0, we don't need the type of `d_updated` variables and `d_new...` since 
changes to parameters happen in between `work` calls and can be handled in the 
`on_parameter_change` callback.  We also don't need private variables for parameters that 
are changeable, such as length and scale

3.x:
```cpp
class moving_average_impl : public moving_average<T>
{
private:
    int d_length;
    T d_scale;
    volk::vector<T> d_scales;
    int d_max_iter;
    const unsigned int d_vlen;
    volk::vector<T> d_sum;


    int d_new_length;
    T d_new_scale;
    bool d_updated;
```

4.0:
```cpp

#include <volk/volk_alloc.hh>
#include <algorithm>
#include <vector>
...
private:
    volk::vector<T> d_scales;
    size_t d_max_iter;
    const size_t d_vlen;
    volk::vector<T> d_sum;

    bool d_init_padding = false;
    size_t d_padding_samps = 0;
    std::vector<T> d_padding_vec;
```

In the 4.0 header, we maintain a vector that will act as the padding zeros that 
`history` used to handle

## Implementation cpp - moving_average_cpu.cc

Let's start by copying the volk helper functions for vectorized operations, but use
`if constexpr` from c++17/20 to make the code a bit cleaner

```cpp
namespace {
template <typename T>
inline void volk_add(T* out, const T* add, unsigned int num)
{
    if constexpr (std::is_same_v<T, float>) {
        volk_32f_x2_add_32f(out, out, add, num);
    }
    else if constexpr (std::is_same_v<T, gr_complex>) {
        volk_32fc_x2_add_32fc(out, out, add, num);
    }
    else {
        for (unsigned int elem = 0; elem < num; elem++) {
            out[elem] += add[elem];
        }
    }
}


template <typename T>
inline void volk_sub(T* out, const T* sub, unsigned int num)
{

    if constexpr (std::is_same_v<T, float>) {
        volk_32f_x2_subtract_32f(out, out, sub, num);
    }
    else if constexpr (std::is_same_v<T, gr_complex>) {
        // std::complex is required to be implemented by adjacent and non-padded
        // imaginary and real values, so it's safe to treat as such.
        auto fout = reinterpret_cast<T::value_type*>(out);
        auto fsub = reinterpret_cast<const T::value_type*>(sub);

        // But just to be sure, let's double-check that.
        static_assert(
            sizeof(gr_complex) == 2 * sizeof(gr_complex::value_type),
            "Can't happen: sizeof(gr_complex) != 2*sizeof(gr_complex::value_type)");

        volk_32f_x2_subtract_32f(fout, fout, fsub, 2 * num);
    }
    else {
        for (unsigned int elem = 0; elem < num; elem++) {
            out[elem] -= sub[elem];
        }
    }
}

template <typename T>
inline void volk_mul(T* out, const T* in, const T* scale, unsigned int num)
{
    if constexpr (std::is_same_v<T, float>) {
        volk_32f_x2_multiply_32f(out, in, scale, num);
    }
    else if constexpr (std::is_same_v<T, gr_complex>) {
        volk_32fc_x2_multiply_32fc(out, in, scale, num);
    }
    else {
        for (unsigned int elem = 0; elem < num; elem++) {
            out[elem] = in[elem] * scale[elem];
        }
    }
}
} // namespace

```

## The constructor
Update the initializers - don't forget the parameter values come in as an `args` struct

```cpp
      d_max_iter(args.max_iter),
      d_vlen(args.vlen),
```

Now all we need to do is declare that we will not be consuming length-1 samples in each work 
call, and set up the padding buffer (d_scales is same as before)
```cpp
template <class T>
moving_average_cpu<T>::moving_average_cpu(
    const typename moving_average<T>::block_args& args)
    : INHERITED_CONSTRUCTORS(T),
      d_max_iter(args.max_iter),
      d_vlen(args.vlen),
{
    // this->set_history(length);
    this->declare_noconsume(args.length - 1);

    // we store this vector so that work() doesn't spend its time allocating and freeing
    // vector storage
    if (d_vlen > 1) {
        d_sum = volk::vector<T>(d_vlen);
        d_scales = volk::vector<T>(d_vlen, args.scale);
    }

    d_padding_samps = this->noconsume();
    d_padding_vec.resize(d_padding_samps * 2 + 1);
}
```

## The enforce_constraints() method

This method is not required by default, so we will have to add it to both the `_cpu.h` and the 
`_cpu.cc`.  It should be a private method, and fortunately has the same signature as `work`
so just copy that and change the name

```cpp
private:
work_return_t enforce_constraints(work_io& wio) override;
```

All we need to do in the enforce constraints is guarantee that when work 
gets called, the input and output sizing reflect the noconsume() value
that was set

```cpp
template <class T>
work_return_t moving_average_cpu<T>::enforce_constraints(work_io& wio)
{
    auto noutput_items = wio.outputs()[0].n_items;
    auto ninput_items = wio.inputs()[0].n_items;

    // Constrain inputs based on outputs
    if (ninput_items <= this->noconsume() || noutput_items <= this->noconsume()) {
        return work_return_t::INSUFFICIENT_INPUT_ITEMS;
    }

    size_t nin = std::min(ninput_items - this->noconsume(), noutput_items);

    wio.inputs()[0].n_items = nin;
    wio.outputs()[0].n_items = nin;

    return work_return_t::OK;
}
```

### The work() method

Calls to get the pointers is much more straightforward (GR3.x commented out here).  
Since noutput_items is not a `work()` parameter
```cpp
    // const T* in = static_cast<const T*>(input_items[0]);
    // T* out = static_cast<T*>(output_items[0]);
    auto in = wio.inputs()[0].items<T>();
    auto out = wio.outputs()[0].items<T>();

    auto noutput_items = wio.outputs()[0].n_items;
```

We can grab the latest value of our dynamic parameters as below:
```cpp
    auto length = pmtf::get_as<size_t>(*this->param_length);
    auto scale = pmtf::get_as<T>(*this->param_scale);
```    

Instead of returning the number of produced items, we must explicitly
produce and consume
```cpp
    //return num_iter;
    wio.consume_each(num_iter);
    wio.produce_each(num_iter);
    return work_return_t::OK;
```


## QA Test

Copy qa_moving_average.py into the block directory

Edit the qa file to reflect that some blocks have moved from `blocks` to `streamops` or `filter`