/* -*- c++ -*- */
/*
 * Copyright 2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef RPCREGISTERHELPERS_H
#define RPCREGISTERHELPERS_H

#include <gnuradio/rpcmanager.h>
#include <gnuradio/rpcserver_base.h>
#include <gnuradio/rpcserver_booter_base.h>
#include <gnuradio/rpcserver_selector.h>
#include <sstream>

// Fixes circular dependency issue before including block_registry.h
class rpcbasic_base;
typedef std::shared_ptr<rpcbasic_base> rpcbasic_sptr;

#include <gnuradio/block_registry.h>


/*********************************************************************
 *   RPC Extractor Base Classes
 ********************************************************************/

/*!
 *\brief Base class for registering a ControlPort Extractor. Acts as
 *       a message acceptor.
 */
template <typename T, typename Tto>
class rpcextractor_base : public virtual gr::messages::msg_accepter
{
public:
    rpcextractor_base(T* source, void (T::*func)(Tto)) : _source(source), _func(func)
    {
        ;
    }
    ~rpcextractor_base() override { ; }

    void post(pmt::pmt_t which_port, pmt::pmt_t msg) override
    {
        (void)which_port;
        (void)msg;
        throw std::runtime_error(
            "rpcextractor_base: no post defined for this data type.");
    }

protected:
    T* _source;
    void (T::*_func)(Tto);
};

template <typename T>
class rpcextractor_base<T, void> : public virtual gr::messages::msg_accepter
{
public:
    rpcextractor_base(T* source, void (T::*func)()) : _source(source), _func(func) { ; }
    ~rpcextractor_base() override { ; }

    void post(pmt::pmt_t which_port, pmt::pmt_t msg) override
    {
        (void)which_port;
        (void)msg;
        throw std::runtime_error(
            "rpcextractor_base: no post defined for this data type.");
    }

protected:
    T* _source;
    void (T::*_func)();
};

/*!
 * \brief Templated parent class for registering a ControlPort Extractor.
 */
template <typename T, typename Tto>
class rpcbasic_extractor : public virtual rpcextractor_base<T, Tto>
{
public:
    rpcbasic_extractor(T* source, void (T::*func)(Tto))
        : rpcextractor_base<T, Tto>(source, func)
    {
        ;
    }
};


/*********************************************************************
 *   RPC Inserter Base Classes
 ********************************************************************/

/*!
 * \brief Base class for registering a ControlPort Inserter. Produces a
 *        message.
 */
template <typename T, typename Tfrom>
class rpcinserter_base : public virtual gr::messages::msg_producer
{
public:
    rpcinserter_base(T* source, Tfrom (T::*func)()) : _source(source), _func(func) { ; }
    rpcinserter_base() { ; }

    pmt::pmt_t retrieve() override
    {
        assert(0);
        return pmt::pmt_t();
    }

protected:
    T* _source;
    Tfrom (T::*_func)();
};


/*!
 * \brief Templated parent class for registering a ControlPort
 * Inserter.
 */
template <typename T, typename Tfrom>
class rpcbasic_inserter : public virtual rpcinserter_base<T, Tfrom>
{
public:
    rpcbasic_inserter(T* source, Tfrom (T::*func)() const)
        : rpcinserter_base<T, Tfrom>(source, func)
    {
        ;
    }

    rpcbasic_inserter(T* source, Tfrom (T::*func)())
        : rpcinserter_base<T, Tfrom>(source, func)
    {
        ;
    }

    pmt::pmt_t retrieve()
    {
        return pmt::mp(
            (rpcinserter_base<T, Tfrom>::_source->*rpcinserter_base<T, Tfrom>::_func)());
    }
};


/*********************************************************************
 *   RPC Handler Base Classes
 ********************************************************************/

/*!
 *\brief Base class for registering a ControlPort Handler. Acts as
 *       a message acceptor.
 */
template <typename T>
class rpchandler_base : public virtual gr::messages::msg_accepter
{
public:
    rpchandler_base(T* source, const char* handler) : _source(source), _handler(handler)
    {
        ;
    }
    ~rpchandler_base() override { ; }

    void post(pmt::pmt_t which_port, pmt::pmt_t msg) override
    {
        _source->post(which_port, msg);
    }

protected:
    T* _source;
    const char* _handler;
};


/*!
 * \brief Templated parent class for registering a ControlPort Extractor.
 */
template <typename T>
class rpcbasic_handler : public virtual rpchandler_base<T>
{
public:
    rpcbasic_handler(T* source, const char* handler) : rpchandler_base<T>(source, handler)
    {
        ;
    }
};


/*********************************************************************
 *   RPC Specialized Extractors
 ********************************************************************/

/*!
 * \brief Specialized extractor class to make calls to functions that
 * do not take data (enable, reset, start, etc.).
 */
template <typename T>
class rpcbasic_extractor<T, void> : public virtual rpcextractor_base<T, void>
{
public:
    rpcbasic_extractor(T* source, void (T::*func)())
        : rpcextractor_base<T, void>(source, func)
    {
        ;
    }

    void post(pmt::pmt_t which_port, pmt::pmt_t msg)
    {
        (void)which_port;
        (void)msg;
        (rpcextractor_base<T, void>::_source->*rpcextractor_base<T, void>::_func)();
    }
};

/*!
 * \brief Specialized extractor class for char data.
 */
template <typename T>
class rpcbasic_extractor<T, char> : public virtual rpcextractor_base<T, char>
{
public:
    rpcbasic_extractor(T* source, void (T::*func)(char))
        : rpcextractor_base<T, char>(source, func)
    {
        ;
    }

    void post(pmt::pmt_t which_port, pmt::pmt_t msg)
    {
        (void)which_port;
        (rpcextractor_base<T, char>::_source->*rpcextractor_base<T, char>::_func)(
            static_cast<char>(pmt::to_long(msg)));
    }
};

/*!
 * \brief Specialized extractor class for short data.
 */
template <typename T>
class rpcbasic_extractor<T, short> : public virtual rpcextractor_base<T, short>
{
public:
    rpcbasic_extractor(T* source, void (T::*func)(short))
        : rpcextractor_base<T, short>(source, func)
    {
        ;
    }

    void post(pmt::pmt_t which_port, pmt::pmt_t msg)
    {
        (void)which_port;
        (rpcextractor_base<T, short>::_source->*rpcextractor_base<T, short>::_func)(
            static_cast<short>(pmt::to_long(msg)));
    }
};

/*!
 * \brief Specialized extractor class for double data.
 */
template <typename T>
class rpcbasic_extractor<T, double> : public virtual rpcextractor_base<T, double>
{
public:
    rpcbasic_extractor(T* source, void (T::*func)(double))
        : rpcextractor_base<T, double>(source, func)
    {
        ;
    }

    void post(pmt::pmt_t which_port, pmt::pmt_t msg)
    {
        (void)which_port;
        (rpcextractor_base<T, double>::_source->*rpcextractor_base<T, double>::_func)(
            pmt::to_double(msg));
    }
};

/*!
 * \brief Specialized extractor class for float data.
 */
template <typename T>
class rpcbasic_extractor<T, float> : public virtual rpcextractor_base<T, float>
{
public:
    rpcbasic_extractor(T* source, void (T::*func)(float))
        : rpcextractor_base<T, float>(source, func)
    {
        ;
    }

    void post(pmt::pmt_t which_port, pmt::pmt_t msg)
    {
        (void)which_port;
        (rpcextractor_base<T, float>::_source->*rpcextractor_base<T, float>::_func)(
            pmt::to_double(msg));
    }
};

/*!
 * \brief Specialized extractor class for long data.
 */
template <typename T>
class rpcbasic_extractor<T, long> : public virtual rpcextractor_base<T, long>
{
public:
    rpcbasic_extractor(T* source, void (T::*func)(long))
        : rpcextractor_base<T, long>(source, func)
    {
        ;
    }

    void post(pmt::pmt_t which_port, pmt::pmt_t msg)
    {
        (void)which_port;
        (rpcextractor_base<T, long>::_source->*rpcextractor_base<T, long>::_func)(
            pmt::to_long(msg));
    }
};

/*!
 * \brief Specialized extractor class for int data.
 */
template <typename T>
class rpcbasic_extractor<T, int> : public virtual rpcextractor_base<T, int>
{
public:
    rpcbasic_extractor(T* source, void (T::*func)(int))
        : rpcextractor_base<T, int>(source, func)
    {
        ;
    }

    void post(pmt::pmt_t which_port, pmt::pmt_t msg)
    {
        (void)which_port;
        (rpcextractor_base<T, int>::_source->*rpcextractor_base<T, int>::_func)(
            pmt::to_long(msg));
    }
};

/*!
 * \brief Specialized extractor class for bool data.
 */
template <typename T>
class rpcbasic_extractor<T, bool> : public virtual rpcextractor_base<T, bool>
{
public:
    rpcbasic_extractor(T* source, void (T::*func)(bool))
        : rpcextractor_base<T, bool>(source, func)
    {
        ;
    }

    void post(pmt::pmt_t which_port, pmt::pmt_t msg)
    {
        (void)which_port;
        (rpcextractor_base<T, bool>::_source->*rpcextractor_base<T, bool>::_func)(
            pmt::to_bool(msg));
    }
};

/*!
 * \brief Specialized extractor class for complex (float) data.
 */
template <typename T>
class rpcbasic_extractor<T, std::complex<float>>
    : public virtual rpcextractor_base<T, std::complex<float>>
{
public:
    rpcbasic_extractor(T* source, void (T::*func)(std::complex<float>))
        : rpcextractor_base<T, std::complex<float>>(source, func)
    {
        ;
    }

    void post(pmt::pmt_t which_port, pmt::pmt_t msg)
    {
        (void)which_port;
        std::complex<float> k = static_cast<std::complex<float>>(pmt::to_complex(msg));
        (rpcextractor_base<T, std::complex<float>>::_source
             ->*rpcextractor_base<T, std::complex<float>>::_func)(k);
    }
};

/*!
 * \brief Specialized extractor class for complex (double) data.
 */
template <typename T>
class rpcbasic_extractor<T, std::complex<double>>
    : public virtual rpcextractor_base<T, std::complex<double>>
{
public:
    rpcbasic_extractor(T* source, void (T::*func)(std::complex<double>))
        : rpcextractor_base<T, std::complex<double>>(source, func)
    {
        ;
    }

    void post(pmt::pmt_t which_port, pmt::pmt_t msg)
    {
        (void)which_port;
        (rpcextractor_base<T, std::complex<double>>::_source
             ->*rpcextractor_base<T, std::complex<double>>::_func)(pmt::to_complex(msg));
    }
};

/*!
 * \brief Specialized extractor class for string data.
 */
template <typename T>
class rpcbasic_extractor<T, std::string>
    : public virtual rpcextractor_base<T, std::string>
{
public:
    rpcbasic_extractor(T* source, void (T::*func)(std::string))
        : rpcextractor_base<T, std::string>(source, func)
    {
        ;
    }

    void post(pmt::pmt_t which_port, pmt::pmt_t msg)
    {
        (void)which_port;
        (rpcextractor_base<T, std::string>::_source
             ->*rpcextractor_base<T, std::string>::_func)(pmt::symbol_to_string(msg));
    }
};


/*********************************************************************
 *   RPC Specialized Inserters
 ********************************************************************/

/*!
 * \brief Specialized inserter class for uint64_t data.
 */
template <typename T>
class rpcbasic_inserter<T, uint64_t> : public virtual rpcinserter_base<T, uint64_t>
{
public:
    rpcbasic_inserter(T* source, uint64_t (T::*func)() const)
        : rpcinserter_base<T, uint64_t>(source, func)
    {
        ;
    }

    rpcbasic_inserter(T* source, uint64_t (T::*func)())
        : rpcinserter_base<T, uint64_t>(source, func)
    {
        ;
    }

    pmt::pmt_t retrieve()
    {
        return pmt::from_uint64((rpcinserter_base<T, uint64_t>::_source
                                     ->*rpcinserter_base<T, uint64_t>::_func)());
    }
};

/*!
 * \brief Specialized inserter class for vectors of signed char data.
 */
template <typename T>
class rpcbasic_inserter<T, std::vector<signed char>>
    : public virtual rpcinserter_base<T, std::vector<signed char>>
{
public:
    rpcbasic_inserter(T* source, std::vector<signed char> (T::*func)() const)
        : rpcinserter_base<T, std::vector<signed char>>(source, func)
    {
        ;
    }

    rpcbasic_inserter(T* source, std::vector<signed char> (T::*func)())
        : rpcinserter_base<T, std::vector<signed char>>(source, func)
    {
        ;
    }

    pmt::pmt_t retrieve()
    {
        std::vector<signed char> vec(
            (rpcinserter_base<T, std::vector<signed char>>::_source
                 ->*rpcinserter_base<T, std::vector<signed char>>::_func)());
        return pmt::init_s8vector(vec.size(), &vec[0]);
    }
};

/*!
 * \brief Specialized inserter class for vectors of short data.
 */
template <typename T>
class rpcbasic_inserter<T, std::vector<short>>
    : public virtual rpcinserter_base<T, std::vector<short>>
{
public:
    rpcbasic_inserter(T* source, std::vector<short> (T::*func)() const)
        : rpcinserter_base<T, std::vector<short>>(source, func)
    {
        ;
    }

    rpcbasic_inserter(T* source, std::vector<short> (T::*func)())
        : rpcinserter_base<T, std::vector<short>>(source, func)
    {
        ;
    }

    pmt::pmt_t retrieve()
    {
        std::vector<short> vec((rpcinserter_base<T, std::vector<short>>::_source
                                    ->*rpcinserter_base<T, std::vector<short>>::_func)());
        return pmt::init_s16vector(vec.size(), &vec[0]);
    }
};

/*!
 * \brief Specialized inserter class for vectors of int data.
 */
template <typename T>
class rpcbasic_inserter<T, std::vector<int>>
    : public virtual rpcinserter_base<T, std::vector<int>>
{
public:
    rpcbasic_inserter(T* source, std::vector<int> (T::*func)() const)
        : rpcinserter_base<T, std::vector<int>>(source, func)
    {
        ;
    }

    rpcbasic_inserter(T* source, std::vector<int> (T::*func)())
        : rpcinserter_base<T, std::vector<int>>(source, func)
    {
        ;
    }

    pmt::pmt_t retrieve()
    {
        std::vector<int> vec((rpcinserter_base<T, std::vector<int>>::_source
                                  ->*rpcinserter_base<T, std::vector<int>>::_func)());
        return pmt::init_s32vector(vec.size(), &vec[0]);
    }
};

/*!
 * \brief Specialized inserter class for vectors of int64_t data.
 */
template <typename T>
class rpcbasic_inserter<T, std::vector<int64_t>>
    : public virtual rpcinserter_base<T, std::vector<int64_t>>
{
public:
    rpcbasic_inserter(T* source, std::vector<int64_t> (T::*func)() const)
        : rpcinserter_base<T, std::vector<int64_t>>(source, func)
    {
        ;
    }

    rpcbasic_inserter(T* source, std::vector<int64_t> (T::*func)())
        : rpcinserter_base<T, std::vector<int64_t>>(source, func)
    {
        ;
    }

    pmt::pmt_t retrieve()
    {
        std::vector<int64_t> vec(
            (rpcinserter_base<T, std::vector<int64_t>>::_source
                 ->*rpcinserter_base<T, std::vector<int64_t>>::_func)());
        return pmt::init_s64vector(vec.size(), &vec[0]);
    }
};

/*!
 * \brief Specialized inserter class for vectors of complex (float) data.
 */
template <typename T>
class rpcbasic_inserter<T, std::vector<std::complex<float>>>
    : public virtual rpcinserter_base<T, std::vector<std::complex<float>>>
{
public:
    rpcbasic_inserter(T* source, std::vector<std::complex<float>> (T::*func)() const)
        : rpcinserter_base<T, std::vector<std::complex<float>>>(source, func)
    {
        ;
    }

    rpcbasic_inserter(T* source, std::vector<std::complex<float>> (T::*func)())
        : rpcinserter_base<T, std::vector<std::complex<float>>>(source, func)
    {
        ;
    }

    pmt::pmt_t retrieve()
    {
        std::vector<std::complex<float>> vec(
            (rpcinserter_base<T, std::vector<std::complex<float>>>::_source
                 ->*rpcinserter_base<T, std::vector<std::complex<float>>>::_func)());
        return pmt::init_c32vector(vec.size(), &vec[0]);
    }
};

/*!
 * \brief Specialized inserter class for vectors of float data.
 */
template <typename T>
class rpcbasic_inserter<T, std::vector<float>>
    : public virtual rpcinserter_base<T, std::vector<float>>
{
public:
    rpcbasic_inserter(T* source, std::vector<float> (T::*func)() const)
        : rpcinserter_base<T, std::vector<float>>(source, func)
    {
        ;
    }

    rpcbasic_inserter(T* source, std::vector<float> (T::*func)())
        : rpcinserter_base<T, std::vector<float>>(source, func)
    {
        ;
    }

    pmt::pmt_t retrieve()
    {
        std::vector<float> vec((rpcinserter_base<T, std::vector<float>>::_source
                                    ->*rpcinserter_base<T, std::vector<float>>::_func)());
        return pmt::init_f32vector(vec.size(), &vec[0]);
    }
};

/*!
 * \brief Specialized inserter class for vectors of uint8_t data.
 */
template <typename T>
class rpcbasic_inserter<T, std::vector<uint8_t>>
    : public virtual rpcinserter_base<T, std::vector<uint8_t>>
{
public:
    rpcbasic_inserter(T* source, std::vector<uint8_t> (T::*func)() const)
        : rpcinserter_base<T, std::vector<uint8_t>>(source, func)
    {
        ;
    }

    rpcbasic_inserter(T* source, std::vector<uint8_t> (T::*func)())
        : rpcinserter_base<T, std::vector<uint8_t>>(source, func)
    {
        ;
    }

    pmt::pmt_t retrieve()
    {
        std::vector<uint8_t> vec(
            (rpcinserter_base<T, std::vector<uint8_t>>::_source
                 ->*rpcinserter_base<T, std::vector<uint8_t>>::_func)());
        return pmt::init_u8vector(vec.size(), &vec[0]);
    }
};

/*!
 * \brief Specialized inserter class for complex (float) data.
 */
template <typename T>
class rpcbasic_inserter<T, std::complex<float>>
    : public virtual rpcinserter_base<T, std::complex<float>>
{
public:
    rpcbasic_inserter(T* source, std::complex<float> (T::*func)() const)
        : rpcinserter_base<T, std::complex<float>>(source, func)
    {
        ;
    }

    rpcbasic_inserter(T* source, std::complex<float> (T::*func)())
        : rpcinserter_base<T, std::complex<float>>(source, func)
    {
        ;
    }

    pmt::pmt_t retrieve()
    {
        std::complex<float> k((rpcinserter_base<T, std::complex<float>>::_source
                                   ->*rpcinserter_base<T, std::complex<float>>::_func)());
        return pmt::from_complex(k);
    }
};

/*!
 * \brief Specialized inserter class for complex (double) data.
 */
template <typename T>
class rpcbasic_inserter<T, std::complex<double>>
    : public virtual rpcinserter_base<T, std::complex<double>>
{
public:
    rpcbasic_inserter(T* source, std::complex<double> (T::*func)() const)
        : rpcinserter_base<T, std::complex<double>>(source, func)
    {
        ;
    }

    rpcbasic_inserter(T* source, std::complex<double> (T::*func)())
        : rpcinserter_base<T, std::complex<double>>(source, func)
    {
        ;
    }

    pmt::pmt_t retrieve()
    {
        std::complex<double> k(
            (rpcinserter_base<T, std::complex<double>>::_source
                 ->*rpcinserter_base<T, std::complex<double>>::_func)());
        return pmt::from_complex(k);
    }
};

/*!
 * \brief Base class for registering a ControlPort function.
 */
template <typename T>
struct rpc_register_base {
    rpc_register_base() { count++; }

protected:
    static int count;
};

/*!
 * Base class to inherit from and create universal shared pointers.
 */
class rpcbasic_base
{
public:
    rpcbasic_base() {}
    virtual ~rpcbasic_base(){};
};


/*********************************************************************
 *   RPC Register Set Classes
 ********************************************************************/

/*!
 * \brief Registers a 'set' function to set a parameter over
 * ControlPort.
 *
 * \details
 *
 * This class allows us to remotely set a value or parameter of the
 * block over ControlPort. The set occurs by calling a setter accessor
 * function of the class, usually set_[variable](), which is passed in
 * as \p function.
 *
 * We can set the (expected) minimum (\p min), maximum (\p max), and
 * default (\p def) of the variables being set. These values are not
 * enforced, however, but can be useful for setting up graphs and
 * other ways of bounding the data.
 *
 * This class also allows us to provide information to the user about
 * the variable being set, such as an appropriate unit (\p units_) as
 * well as a description (\p desc_) about what the variable does.
 *
 * The privilege (\p minpriv_) level is the minimum privilege level a
 * remote must identify with to be able to call this function.
 *
 * We also provide display hints (\p display_), which can be used by
 * the ControlPort client application to know how to best display or
 * even print the data. This is a mask of options for variables set in
 * rpccallbackregister_base.h. The mask is defined by one of the
 * "DisplayType Plotting Types" and or'd with any of the "DisplayType
 * Options" features. See "Display Options" in \ref page_ctrlport for
 * details.
 */
template <typename T, typename Tto>
struct rpcbasic_register_set : public rpcbasic_base {
    /*!
     * \brief Adds the ability to set the variable over ControlPort.
     *
     * \details
     *
     * This constructor is specifically for gr::block's to use to add
     * settable variables to ControlPort. Generally meant to be used
     * in gr::block::setup_rpc.
     *
     * Uses the block's alias to create the ControlPort interface. This
     * alias is cross-referenced by the global_block_registry (static
     * variable of type gr::block_registry) to get the pointer to the
     * block.
     *
     * \param block_alias  Block's alias; use alias() to get it from the block.
     * \param functionbase The name of the function that we'll access over ControlPort
     * \param function     A function pointer to the real function accessed when called
     *                     something like: &[block class]\::set_[variable]()
     * \param min          Expected minimum value the parameter can hold
     * \param max          Expected maximum value the parameter can hold
     * \param def          Expected default value the parameter can hold
     * \param units_       A string to describe what units to represent the variable with
     * \param desc_        A string to describing the variable.
     * \param minpriv_     The required minimum privilege level
     * \param display_     The display mask
     */
    rpcbasic_register_set(const std::string& block_alias,
                          [[maybe_unused]] const char* functionbase,
                          [[maybe_unused]] void (T::*function)(Tto),
                          const pmt::pmt_t& min,
                          const pmt::pmt_t& max,
                          const pmt::pmt_t& def,
                          const char* units_ = "",
                          const char* desc_ = "",
                          priv_lvl_t minpriv_ = RPC_PRIVLVL_MIN,
                          DisplayType display_ = DISPNULL)
    {
        d_min = min;
        d_max = max;
        d_def = def;
        d_units = units_;
        d_desc = desc_;
        d_minpriv = minpriv_;
        d_display = display_;
        d_object = dynamic_cast<T*>(
            global_block_registry.block_lookup(pmt::intern(block_alias)).get());
#ifdef GR_RPCSERVER_ENABLED
        callbackregister_base::configureCallback_t extractor(
            new rpcbasic_extractor<T, Tto>(d_object, function),
            minpriv_,
            std::string(units_),
            display_,
            std::string(desc_),
            min,
            max,
            def);
        std::ostringstream oss(std::ostringstream::out);
        oss << block_alias << "::" << functionbase;
        d_id = oss.str();
        // std::cerr << "REGISTERING SET: " << d_id << "  " << desc_ << std::endl;
        rpcmanager::get()->i()->registerConfigureCallback(d_id, extractor);
#endif
    }

    /*!
     * \brief Adds the ability to set the variable over ControlPort.
     *
     * \details
     *
     * Allows us to add non gr::block related objects to
     * ControlPort. Instead of using the block's alias, we give it a \p
     * name and the actual pointer to the object as \p obj. We just need
     * to make sure that the pointer to this object is always valid.
     *
     * \param name         Name of the object being set up for ControlPort access
     * \param functionbase The name of the function that we'll access over ControlPort
     * \param obj          A pointer to the object itself
     * \param function     A function pointer to the real function accessed when called
     *                     something like: &[block class]\::set_[variable]()
     * \param min          Expected minimum value the parameter can hold
     * \param max          Expected maximum value the parameter can hold
     * \param def          Expected default value the parameter can hold
     * \param units_       A string to describe what units to represent the variable with
     * \param desc_        A string to describing the variable.
     * \param minpriv_     The required minimum privilege level
     * \param display_     The display mask
     */
    rpcbasic_register_set([[maybe_unused]] const std::string& name,
                          [[maybe_unused]] const char* functionbase,
                          T* obj,
                          [[maybe_unused]] void (T::*function)(Tto),
                          const pmt::pmt_t& min,
                          const pmt::pmt_t& max,
                          const pmt::pmt_t& def,
                          const char* units_ = "",
                          const char* desc_ = "",
                          priv_lvl_t minpriv_ = RPC_PRIVLVL_MIN,
                          DisplayType display_ = DISPNULL)
    {
        d_min = min;
        d_max = max;
        d_def = def;
        d_units = units_;
        d_desc = desc_;
        d_minpriv = minpriv_;
        d_display = display_;
        d_object = obj;
#ifdef GR_RPCSERVER_ENABLED
        callbackregister_base::configureCallback_t extractor(
            new rpcbasic_extractor<T, Tto>(d_object, function),
            minpriv_,
            std::string(units_),
            display_,
            std::string(desc_),
            min,
            max,
            def);
        std::ostringstream oss(std::ostringstream::out);
        oss << name << "::" << functionbase;
        d_id = oss.str();
        // std::cerr << "REGISTERING SET: " << d_id << "  " << desc_ << std::endl;
        rpcmanager::get()->i()->registerConfigureCallback(d_id, extractor);
#endif
    }

    ~rpcbasic_register_set() override
    {
#ifdef GR_RPCSERVER_ENABLED
        rpcmanager::get()->i()->unregisterConfigureCallback(d_id);
#endif
    }


    pmt::pmt_t min() const { return d_min; }
    pmt::pmt_t max() const { return d_max; }
    pmt::pmt_t def() const { return d_def; }
    std::string units() const { return d_units; }
    std::string description() const { return d_desc; }
    priv_lvl_t privilege_level() const { return d_minpriv; }
    DisplayType default_display() const { return d_display; }

    void set_min(pmt::pmt_t p) { d_min = p; }
    void set_max(pmt::pmt_t p) { d_max = p; }
    void set_def(pmt::pmt_t p) { d_def = p; }
    void units(std::string u) { d_units = u; }
    void description(std::string d) { d_desc = d; }
    void privilege_level(priv_lvl_t p) { d_minpriv = p; }
    void default_display(DisplayType d) { d_display = d; }

private:
    std::string d_id;
    pmt::pmt_t d_min, d_max, d_def;
    std::string d_units, d_desc;
    priv_lvl_t d_minpriv;
    DisplayType d_display;
    T* d_object;
};


/*********************************************************************
 *   RPC Register Trigger Classes
 ********************************************************************/

/*!
 * \brief Registers a 'trigger' function to trigger an action over
 * ControlPort.
 *
 * \details
 *
 * This class allows us to set up triggered events or function calls
 * over ControlPort. When used from a ControlPort client, the \p
 * function established here will be activated. Generally, this is
 * meant to enable some kind of trigger or action that a block or
 * object will perform, such as a reset, start, stop, etc.
 *
 * Simpler than the rpcbasic_register_set class, the constructor here
 * only takes a few parameters, mostly because there is not actual
 * variable associated with these function calls. It takes in the
 * information to set up the pointer to the object that has the \p
 * function, a ControlPort name (\p functionbase) for the triggered
 * action, a description (\p desc_), and a privilege level (\p
 * minpriv_).
 */
template <typename T>
struct rpcbasic_register_trigger : public rpcbasic_base {
    /*!
     * \brief Adds the ability to trigger a function over ControlPort.
     *
     * \details
     *
     * This constructor is specifically for gr::block's to use to add
     * trigger functions to ControlPort. Generally meant to be used
     * in gr::block::setup_rpc.
     *
     * Uses the block's alias to create the ControlPort interface. This
     * alias is cross-referenced by the global_block_registry (static
     * variable of type gr::block_registry) to get the pointer to the
     * block.
     *
     * \param block_alias  Block's alias; use alias() to get it from the block.
     * \param functionbase The name of the function that we'll access over ControlPort
     * \param function     A function pointer to the real function accessed when called
     *                     something like: &[block class]\::set_[variable]
     * \param desc_        A string to describing the variable.
     * \param minpriv_     The required minimum privilege level
     */
    rpcbasic_register_trigger(const std::string& block_alias,
                              [[maybe_unused]] const char* functionbase,
                              [[maybe_unused]] void (T::*function)(),
                              const char* desc_ = "",
                              priv_lvl_t minpriv_ = RPC_PRIVLVL_MIN)
    {
        d_desc = desc_;
        d_minpriv = minpriv_;
        d_object = dynamic_cast<T*>(
            global_block_registry.block_lookup(pmt::intern(block_alias)).get());
#ifdef GR_RPCSERVER_ENABLED
        callbackregister_base::configureCallback_t extractor(
            new rpcbasic_extractor<T, void>(d_object, function),
            minpriv_,
            std::string(desc_));
        std::ostringstream oss(std::ostringstream::out);
        oss << block_alias << "::" << functionbase;
        d_id = oss.str();
        // std::cerr << "REGISTERING TRIGGER: " << d_id << "  " << desc_ << std::endl;
        rpcmanager::get()->i()->registerConfigureCallback(d_id, extractor);
#endif
    }

    /*!
     * \brief Adds the ability to trigger a function over ControlPort.
     *
     * \details
     *
     * Allows us to add non gr::block related objects to
     * ControlPort. Instead of using the block's alias, we give it a \p
     * name and the actual pointer to the object as \p obj. We just need
     * to make sure that the pointer to this object is always valid.
     *
     * \param name         Name of the object being set up for ControlPort access
     * \param functionbase The name of the function that we'll access over ControlPort
     * \param obj          A pointer to the object itself
     * \param function     A function pointer to the real function accessed when called
     *                     something like: &[block class]\::set_[variable]
     * \param desc_        A string to describing the variable.
     * \param minpriv_     The required minimum privilege level
     */
    rpcbasic_register_trigger([[maybe_unused]] const std::string& name,
                              [[maybe_unused]] const char* functionbase,
                              T* obj,
                              [[maybe_unused]] void (T::*function)(),
                              const char* desc_ = "",
                              priv_lvl_t minpriv_ = RPC_PRIVLVL_MIN)
    {
        d_desc = desc_;
        d_minpriv = minpriv_;
        d_object = obj;
#ifdef GR_RPCSERVER_ENABLED
        callbackregister_base::configureCallback_t extractor(
            new rpcbasic_extractor<T, void>(d_object, function),
            minpriv_,
            std::string(desc_));
        std::ostringstream oss(std::ostringstream::out);
        oss << name << "::" << functionbase;
        d_id = oss.str();
        // std::cerr << "REGISTERING TRIGGER: " << d_id << "  " << desc_ << std::endl;
        rpcmanager::get()->i()->registerConfigureCallback(d_id, extractor);
#endif
    }

    ~rpcbasic_register_trigger() override
    {
#ifdef GR_RPCSERVER_ENABLED
        rpcmanager::get()->i()->unregisterConfigureCallback(d_id);
#endif
    }


    std::string description() const { return d_desc; }
    priv_lvl_t privilege_level() const { return d_minpriv; }

    void description(std::string d) { d_desc = d; }
    void privilege_level(priv_lvl_t p) { d_minpriv = p; }

private:
    std::string d_id;
    std::string d_desc;
    priv_lvl_t d_minpriv;
    T* d_object;
};


/*********************************************************************
 *   RPC Register Get Classes
 ********************************************************************/

/*!
 * \brief Registers a 'get' function to get a parameter over
 * ControlPort.
 *
 * \details
 *
 * This class allows us to remotely get a value or parameter of the
 * block over ControlPort. The get occurs by calling a getter accessor
 * function of the class, usually [variable](), which is passed in
 * as \p function.
 *
 * We can set the (expected) minimum (\p min), maximum (\p max), and
 * default (\p def) of the variables we will get. These values are not
 * enforced, however, but can be useful for setting up graphs and
 * other ways of bounding the data.
 *
 * This class also allows us to provide information to the user about
 * the variable, such as an appropriate unit (\p units_) as well as a
 * description (\p desc_) about what the variable does.
 *
 * The privilege (\p minpriv_) level is the minimum privilege level a
 * remote must identify with to be able to call this function.
 *
 * We also provide display hints (\p display_), which can be used by
 * the ControlPort client application to know how to best display or
 * even print the data. This is a mask of options for variables set in
 * rpccallbackregister_base.h. The mask is defined by one of the
 * "DisplayType Plotting Types" and or'd with any of the "DisplayType
 * Options" features. See "Display Options" in \ref page_ctrlport for
 * details.
 */
template <typename T, typename Tfrom>
class rpcbasic_register_get : public rpcbasic_base
{
public:
    /*!
     * \brief Adds the ability to get the variable over ControlPort.
     *
     * \details
     *
     * This constructor is specifically for gr::block's to use to add
     * gettable variables to ControlPort. Generally meant to be used
     * in gr::block::setup_rpc.
     *
     * Uses the block's alias to create the ControlPort interface. This
     * alias is cross-referenced by the global_block_registry (static
     * variable of type gr::block_registry) to get the pointer to the
     * block.
     *
     * \param block_alias  Block's alias; use alias() to get it from the block.
     * \param functionbase The name of the function that we'll access over ControlPort
     * \param function     A function pointer to the real function accessed when called
     *                     something like: &[block class]\::[variable]()
     * \param min          Expected minimum value the parameter can hold
     * \param max          Expected maximum value the parameter can hold
     * \param def          Expected default value the parameter can hold
     * \param units_       A string to describe what units to represent the variable with
     * \param desc_        A string to describing the variable.
     * \param minpriv_     The required minimum privilege level
     * \param display_     The display mask
     */
    rpcbasic_register_get(const std::string& block_alias,
                          [[maybe_unused]] const char* functionbase,
                          [[maybe_unused]] Tfrom (T::*function)(),
                          const pmt::pmt_t& min,
                          const pmt::pmt_t& max,
                          const pmt::pmt_t& def,
                          const char* units_ = "",
                          const char* desc_ = "",
                          priv_lvl_t minpriv_ = RPC_PRIVLVL_MIN,
                          DisplayType display_ = DISPNULL)
    {
        d_min = min;
        d_max = max;
        d_def = def;
        d_units = units_;
        d_desc = desc_;
        d_minpriv = minpriv_;
        d_display = display_;
        d_object = dynamic_cast<T*>(
            global_block_registry.block_lookup(pmt::intern(block_alias)).get());
#ifdef GR_RPCSERVER_ENABLED
        callbackregister_base::queryCallback_t inserter(
            new rpcbasic_inserter<T, Tfrom>(d_object, function),
            minpriv_,
            std::string(units_),
            display_,
            std::string(desc_),
            min,
            max,
            def);
        std::ostringstream oss(std::ostringstream::out);
        oss << block_alias << "::" << functionbase;
        d_id = oss.str();
        // std::cerr << "REGISTERING GET: " << d_id << "  " << desc_ << std::endl;
        rpcmanager::get()->i()->registerQueryCallback(d_id, inserter);
#endif
    }


    /*!
     * \brief Same as rpcbasic_register_get::rpcbasic_register_get that allows using
     * '[variable]() const' getter functions.
     */
    rpcbasic_register_get(const std::string& block_alias,
                          [[maybe_unused]] const char* functionbase,
                          [[maybe_unused]] Tfrom (T::*function)() const,
                          const pmt::pmt_t& min,
                          const pmt::pmt_t& max,
                          const pmt::pmt_t& def,
                          const char* units_ = "",
                          const char* desc_ = "",
                          priv_lvl_t minpriv_ = RPC_PRIVLVL_MIN,
                          DisplayType display_ = DISPNULL)
    {
        d_min = min;
        d_max = max;
        d_def = def;
        d_units = units_;
        d_desc = desc_;
        d_minpriv = minpriv_;
        d_display = display_;
        d_object = dynamic_cast<T*>(
            global_block_registry.block_lookup(pmt::intern(block_alias)).get());
#ifdef GR_RPCSERVER_ENABLED
        callbackregister_base::queryCallback_t inserter(
            new rpcbasic_inserter<T, Tfrom>(d_object, (Tfrom(T::*)())function),
            minpriv_,
            std::string(units_),
            display_,
            std::string(desc_),
            min,
            max,
            def);
        std::ostringstream oss(std::ostringstream::out);
        oss << block_alias << "::" << functionbase;
        d_id = oss.str();
        // std::cerr << "REGISTERING GET CONST: " << d_id << "   " << desc_ << "   " <<
        // display_ << std::endl;
        rpcmanager::get()->i()->registerQueryCallback(d_id, inserter);
#endif
    }


    /*!
     * \brief Adds the ability to get the variable over ControlPort.
     *
     * \details
     *
     * Allows us to add non gr::block related objects to
     * ControlPort. Instead of using the block's alias, we give it a \p
     * name and the actual pointer to the object as \p obj. We just need
     * to make sure that the pointer to this object is always valid.
     *
     * \param name         Name of the object being set up for ControlPort access
     * \param functionbase The name of the function that we'll access over ControlPort
     * \param obj          A pointer to the object itself
     * \param function     A function pointer to the real function accessed when called
     *                     something like: &[block class]\::set_[variable]()
     * \param min          Expected minimum value the parameter can hold
     * \param max          Expected maximum value the parameter can hold
     * \param def          Expected default value the parameter can hold
     * \param units_       A string to describe what units to represent the variable with
     * \param desc_        A string to describing the variable.
     * \param minpriv_     The required minimum privilege level
     * \param display_     The display mask
     */
    rpcbasic_register_get([[maybe_unused]] const std::string& name,
                          [[maybe_unused]] const char* functionbase,
                          T* obj,
                          [[maybe_unused]] Tfrom (T::*function)(),
                          const pmt::pmt_t& min,
                          const pmt::pmt_t& max,
                          const pmt::pmt_t& def,
                          const char* units_ = "",
                          const char* desc_ = "",
                          priv_lvl_t minpriv_ = RPC_PRIVLVL_MIN,
                          DisplayType display_ = DISPNULL)
    {
        d_min = min;
        d_max = max;
        d_def = def;
        d_units = units_;
        d_desc = desc_;
        d_minpriv = minpriv_;
        d_display = display_;
        d_object = obj;
#ifdef GR_RPCSERVER_ENABLED
        callbackregister_base::queryCallback_t inserter(
            new rpcbasic_inserter<T, Tfrom>(d_object, function),
            minpriv_,
            std::string(units_),
            display_,
            std::string(desc_),
            min,
            max,
            def);
        std::ostringstream oss(std::ostringstream::out);
        oss << name << "::" << functionbase;
        d_id = oss.str();
        // std::cerr << "REGISTERING GET: " << d_id << "  " << desc_ << std::endl;
        rpcmanager::get()->i()->registerQueryCallback(d_id, inserter);
#endif
    }


    /*!
     * \brief Same as above that allows using '[variable]() const'
     * getter functions.
     */
    rpcbasic_register_get([[maybe_unused]] const std::string& name,
                          [[maybe_unused]] const char* functionbase,
                          T* obj,
                          [[maybe_unused]] Tfrom (T::*function)() const,
                          const pmt::pmt_t& min,
                          const pmt::pmt_t& max,
                          const pmt::pmt_t& def,
                          const char* units_ = "",
                          const char* desc_ = "",
                          priv_lvl_t minpriv_ = RPC_PRIVLVL_MIN,
                          DisplayType display_ = DISPNULL)
    {
        d_min = min;
        d_max = max;
        d_def = def;
        d_units = units_;
        d_desc = desc_;
        d_minpriv = minpriv_;
        d_display = display_;
        d_object = obj;
#ifdef GR_RPCSERVER_ENABLED
        callbackregister_base::queryCallback_t inserter(
            new rpcbasic_inserter<T, Tfrom>(d_object, (Tfrom(T::*)())function),
            minpriv_,
            std::string(units_),
            display_,
            std::string(desc_),
            min,
            max,
            def);
        std::ostringstream oss(std::ostringstream::out);
        oss << name << "::" << functionbase;
        d_id = oss.str();
        // std::cerr << "REGISTERING GET CONST: " << d_id << "   " << desc_ << "   " <<
        // display_ << std::endl;
        rpcmanager::get()->i()->registerQueryCallback(d_id, inserter);
#endif
    }

    ~rpcbasic_register_get() override
    {
#ifdef GR_RPCSERVER_ENABLED
        rpcmanager::get()->i()->unregisterQueryCallback(d_id);
#endif
    }

    pmt::pmt_t min() const { return d_min; }
    pmt::pmt_t max() const { return d_max; }
    pmt::pmt_t def() const { return d_def; }
    std::string units() const { return d_units; }
    std::string description() const { return d_desc; }
    priv_lvl_t privilege_level() const { return d_minpriv; }
    DisplayType default_display() const { return d_display; }

    void set_min(pmt::pmt_t p) { d_min = p; }
    void set_max(pmt::pmt_t p) { d_max = p; }
    void set_def(pmt::pmt_t p) { d_def = p; }
    void units(std::string u) { d_units = u; }
    void description(std::string d) { d_desc = d; }
    void privilege_level(priv_lvl_t p) { d_minpriv = p; }
    void default_display(DisplayType d) { d_display = d; }

private:
    std::string d_id;
    pmt::pmt_t d_min, d_max, d_def;
    std::string d_units, d_desc;
    priv_lvl_t d_minpriv;
    DisplayType d_display;
    T* d_object;
};


/*********************************************************************
 *   RPC Register Variable Classes
 ********************************************************************/

/*!
 * \brief Registers a read-only function to get a parameter over ControlPort.
 *
 * \details
 *
 * This class allows us to remotely get a value or parameter of the
 * block over ControlPort. Unlike the rpcbasic_register_get class,
 * this version is passed the variable directly and establishes a
 * getter for us, so there is no need to have a getter function
 * already in the object.
 *
 * This version is for read-only get access.
 *
 * We can set the (expected) minimum (\p min), maximum (\p max), and
 * default (\p def) of the variables we will get. These values are not
 * enforced, however, but can be useful for setting up graphs and
 * other ways of bounding the data.
 *
 * This class also allows us to provide information to the user about
 * the variable, such as an appropriate unit (\p units_) as well as a
 * description (\p desc_) about what the variable does.
 *
 * The privilege (\p minpriv_) level is the minimum privilege level a
 * remote must identify with to be able to call this function.
 *
 * We also provide display hints (\p display_), which can be used by
 * the ControlPort client application to know how to best display or
 * even print the data. This is a mask of options for variables set in
 * rpccallbackregister_base.h. The mask is defined by one of the
 * "DisplayType Plotting Types" and or'd with any of the "DisplayType
 * Options" features. See "Display Options" in \ref page_ctrlport for
 * details.
 */
template <typename Tfrom>
class rpcbasic_register_variable : public rpcbasic_base
{
protected:
    rpcbasic_register_get<rpcbasic_register_variable<Tfrom>, Tfrom> d_rpc_reg;
    Tfrom* d_variable;
    Tfrom get() { return *d_variable; }

public:
    void setptr(Tfrom* _variable)
    {
        rpcbasic_register_variable<Tfrom>::d_variable = _variable;
    }

    /*! Empty constructor which should never be called but needs to
     *  exist for us in various STL data structures
     */
    rpcbasic_register_variable()
        : d_rpc_reg("FAIL",
                    "FAIL",
                    this,
                    &rpcbasic_register_variable::get,
                    pmt::PMT_NIL,
                    pmt::PMT_NIL,
                    pmt::PMT_NIL,
                    DISPNULL,
                    "FAIL",
                    "FAIL",
                    RPC_PRIVLVL_MIN),
          d_variable(NULL)
    {
        throw std::runtime_error(
            "ERROR: rpcbasic_register_variable called with no args. If this happens, "
            "someone has tried to use rpcbasic_register_variable incorrectly.");
    };

    /*!
     * \brief Adds the ability to get the variable over ControlPort.
     *
     * \details
     *
     * Creates a new getter accessor function to read \p variable.
     *
     * \param namebase     Name of the object being set up for ControlPort access
     * \param functionbase The name of the function that we'll access over ControlPort
     * \param variable     A pointer to the variable, possibly as a member of a class
     * \param min          Expected minimum value the parameter can hold
     * \param max          Expected maximum value the parameter can hold
     * \param def          Expected default value the parameter can hold
     * \param units_       A string to describe what units to represent the variable with
     * \param desc_        A string to describe the variable.
     * \param minpriv_     The required minimum privilege level
     * \param display_     The display mask
     */
    rpcbasic_register_variable(const std::string& namebase,
                               const char* functionbase,
                               Tfrom* variable,
                               const pmt::pmt_t& min,
                               const pmt::pmt_t& max,
                               const pmt::pmt_t& def,
                               const char* units_ = "",
                               const char* desc_ = "",
                               priv_lvl_t minpriv_ = RPC_PRIVLVL_MIN,
                               DisplayType display_ = DISPNULL)
        : d_rpc_reg(namebase,
                    functionbase,
                    this,
                    &rpcbasic_register_variable::get,
                    min,
                    max,
                    def,
                    units_,
                    desc_,
                    minpriv_,
                    display_),
          d_variable(variable)
    {
        // std::cerr << "REGISTERING VAR: " << " " << desc_ << std::endl;
    }
};


/*!
 * \brief Registers a read/write function to get and set a parameter
 * over ControlPort.
 *
 * \details
 *
 * This class allows us to remotely get and/or set a value or
 * parameter of the block over ControlPort. Unlike the
 * rpcbasic_register_get class, this version is passed the variable
 * directly and establishes a getter for us, so there is no need to
 * have a getter function already in the object.
 *
 * This version establishes both get and set functions and so provides
 * read/write access to the variable.
 *
 * We can set the (expected) minimum (\p min), maximum (\p max), and
 * default (\p def) of the variables we will get. These values are not
 * enforced, however, but can be useful for setting up graphs and
 * other ways of bounding the data.
 *
 * This class also allows us to provide information to the user about
 * the variable, such as an appropriate unit (\p units_) as well as a
 * description (\p desc_) about what the variable does.
 *
 * The privilege (\p minpriv_) level is the minimum privilege level a
 * remote must identify with to be able to call this function.
 *
 * We also provide display hints (\p display_), which can be used by
 * the ControlPort client application to know how to best display or
 * even print the data. This is a mask of options for variables set in
 * rpccallbackregister_base.h. The mask is defined by one of the
 * "DisplayType Plotting Types" and or'd with any of the "DisplayType
 * Options" features. See "Display Options" in \ref page_ctrlport for
 * details.
 */
template <typename Tfrom>
class rpcbasic_register_variable_rw : public rpcbasic_register_variable<Tfrom>
{
private:
    rpcbasic_register_set<rpcbasic_register_variable_rw<Tfrom>, Tfrom> d_rpc_regset;

public:
    /*! Empty constructor which should never be called but needs to
     *  exist for us in various STL data structures.
     */
    rpcbasic_register_variable_rw()
        : d_rpc_regset("FAIL",
                       "FAIL",
                       this,
                       &rpcbasic_register_variable<Tfrom>::get,
                       pmt::PMT_NIL,
                       pmt::PMT_NIL,
                       pmt::PMT_NIL,
                       DISPNULL,
                       "FAIL",
                       "FAIL",
                       RPC_PRIVLVL_MIN)
    {
        throw std::runtime_error(
            "ERROR: rpcbasic_register_variable_rw called with no args. if this happens "
            "someone used rpcbasic_register_variable_rw incorrectly.");
    };

    void set(Tfrom _variable)
    {
        *(rpcbasic_register_variable<Tfrom>::d_variable) = _variable;
    }

    /*!
     * \brief Adds the ability to set and get the variable over ControlPort.
     *
     * \details
     *
     * Creates new getter and setter accessor functions to read and write \p variable.
     *
     * \param namebase     Name of the object being set up for ControlPort access
     * \param functionbase The name of the function that we'll access over ControlPort
     * \param variable     A pointer to the variable, possibly as a member of a class
     * \param min          Expected minimum value the parameter can hold
     * \param max          Expected maximum value the parameter can hold
     * \param def          Expected default value the parameter can hold
     * \param units_       A string to describe what units to represent the variable with
     * \param desc_        A string to describing the variable.
     * \param minpriv      The required minimum privilege level
     * \param display_     The display mask
     */
    rpcbasic_register_variable_rw(const std::string& namebase,
                                  const char* functionbase,
                                  Tfrom* variable,
                                  const pmt::pmt_t& min,
                                  const pmt::pmt_t& max,
                                  const pmt::pmt_t& def,
                                  const char* units_ = "",
                                  const char* desc_ = "",
                                  priv_lvl_t minpriv = RPC_PRIVLVL_MIN,
                                  DisplayType display_ = DISPNULL)
        : rpcbasic_register_variable<Tfrom>(
              namebase, functionbase, variable, min, max, def, units_, desc_),
          d_rpc_regset(namebase,
                       functionbase,
                       this,
                       &rpcbasic_register_variable_rw::set,
                       min,
                       max,
                       def,
                       units_,
                       desc_,
                       minpriv,
                       display_)
    {
        // no action
    }
};


/*!
 * \brief Registers a message handler function to post a message to a
 * block's handler.
 */
template <typename T>
class rpcbasic_register_handler : public rpcbasic_base
{
public:
    /*!
     * \brief Adds the ability to pass a message over ControlPort.
     *
     * \details
     * This makes any message handler function available over
     * ControlPort. Since message handlers always take in a single PMT
     * message input, this interface provides a very generic way of
     * setting values in a block in a flowgraph.
     *
     * \param block_alias  Alias of the block
     * \param handler      The name of the message port in the block
     * \param units_       A string to describe what units to represent the variable with
     * \param desc_        A string to describing the variable.
     * \param minpriv_     The required minimum privilege level
     * \param display_     The display mask
     */
    rpcbasic_register_handler(const std::string& block_alias,
                              [[maybe_unused]] const char* handler,
                              const char* units_ = "",
                              const char* desc_ = "",
                              priv_lvl_t minpriv_ = RPC_PRIVLVL_MIN,
                              DisplayType display_ = DISPNULL)
    {
        d_units = units_;
        d_desc = desc_;
        d_minpriv = minpriv_;
        d_display = display_;
        d_object = dynamic_cast<T*>(
            global_block_registry.block_lookup(pmt::intern(block_alias)).get());
#ifdef GR_RPCSERVER_ENABLED
        callbackregister_base::handlerCallback_t inserter(
            new rpcbasic_handler<T>(d_object, handler),
            minpriv_,
            std::string(units_),
            display_,
            std::string(desc_),
            0,
            0,
            0);
        std::ostringstream oss(std::ostringstream::out);
        oss << block_alias << "::" << handler;
        d_id = oss.str();
        // std::cerr << "REGISTERING GET: " << d_id << "  " << desc_ << std::endl;
        rpcmanager::get()->i()->registerHandlerCallback(d_id, inserter);
#endif
    }

    ~rpcbasic_register_handler() override
    {
#ifdef GR_RPCSERVER_ENABLED
        rpcmanager::get()->i()->unregisterHandlerCallback(d_id);
#endif
    }

    std::string units() const { return d_units; }
    std::string description() const { return d_desc; }
    priv_lvl_t privilege_level() const { return d_minpriv; }
    DisplayType default_display() const { return d_display; }

    void units(std::string u) { d_units = u; }
    void description(std::string d) { d_desc = d; }
    void privilege_level(priv_lvl_t p) { d_minpriv = p; }
    void default_display(DisplayType d) { d_display = d; }

private:
    std::string d_id;
    std::string d_units, d_desc;
    priv_lvl_t d_minpriv;
    DisplayType d_display;
    T* d_object;
};


#endif
