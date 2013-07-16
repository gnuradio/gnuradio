/* -*- c++ -*- */
/* 
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef RPCREGISTERHELPERS_H
#define RPCREGISTERHELPERS_H

#include <stdio.h>
#include <sstream>
#include <iostream>
#include <gnuradio/rpcserver_booter_base.h>
#include <gnuradio/rpcmanager.h>
#include <gnuradio/rpcserver_selector.h>
#include <gnuradio/rpcserver_base.h>
#include <gnuradio/block_registry.h>

// Base classes
template<typename T, typename Tto> class rpcextractor_base
  : public virtual gr::messages::msg_accepter
{
public:
  rpcextractor_base(T* source, void (T::*func)(Tto)) :
    _source(source), _func(func) {;}
  ~rpcextractor_base() {;}

  void post(pmt::pmt_t which_port, pmt::pmt_t msg) {
    throw std::runtime_error("rpcextractor_base: no post defined for this data type.\n");
  }

protected:
  T* _source;
  void (T::*_func)(Tto);
};

template<typename T, typename Tto>
class rpcbasic_extractor : public virtual rpcextractor_base<T,Tto>
{
public:
  rpcbasic_extractor(T* source, void (T::*func)(Tto)) :
    rpcextractor_base<T,Tto>(source, func) 
  {;}
};

template<typename T, typename Tfrom>
class rpcinserter_base : public virtual gr::messages::msg_producer
{
public:
  rpcinserter_base(T* source, Tfrom (T::*func)()) : _source(source), _func(func) {;}
  rpcinserter_base() {;}

  pmt::pmt_t retrieve() { assert(0); return pmt::pmt_t(); }

protected:
  T* _source;
  Tfrom (T::*_func)();
};

template<typename T, typename Tfrom>
class rpcbasic_inserter :
  public virtual rpcinserter_base<T,Tfrom>
{
public:
  rpcbasic_inserter(T* source, Tfrom (T::*func)()const)
    : rpcinserter_base<T,Tfrom>(source, func)
  {;}

  rpcbasic_inserter(T* source, Tfrom (T::*func)())
    : rpcinserter_base<T,Tfrom>(source, func)
  {;}

  pmt::pmt_t retrieve() 
  {
    return pmt::mp((rpcinserter_base<T,Tfrom>::
		    _source->*rpcinserter_base<T,Tfrom>::_func)());
  }
};

// Specialized Extractor Templates
template<typename T>
class rpcbasic_extractor<T,char> : public virtual rpcextractor_base<T,char>
{
public:
  rpcbasic_extractor(T* source, void (T::*func)(char))
    : rpcextractor_base<T,char>(source, func)
  {;}

  void post(pmt::pmt_t which_port, pmt::pmt_t msg)
  {
    (rpcextractor_base<T,char>::_source->*rpcextractor_base<T,char>::_func)
      (static_cast<char>(pmt::to_long(msg)));
  }
};

template<typename T>
class rpcbasic_extractor<T,short> : public virtual rpcextractor_base<T,short>
{
public:
  rpcbasic_extractor(T* source, void (T::*func)(short))
    : rpcextractor_base<T,short>(source, func)
  {;}

  void post(pmt::pmt_t which_port, pmt::pmt_t msg)
  {
    (rpcextractor_base<T,short>::_source->*rpcextractor_base<T,char>::_func)
      (static_cast<short>(pmt::to_long(msg)));
  }
};

template<typename T>
class rpcbasic_extractor<T,double> : public virtual rpcextractor_base<T,double>
{
public:
  rpcbasic_extractor(T* source, void (T::*func)(double))
    : rpcextractor_base<T,double>(source, func)
  {;}

  void post(pmt::pmt_t which_port, pmt::pmt_t msg)
  {
    (rpcextractor_base<T,double>::_source->*rpcextractor_base<T,double>::_func)
      (pmt::to_double(msg));
  }
};

template<typename T>
class rpcbasic_extractor<T,float> : public virtual rpcextractor_base<T,float>
{
public:
  rpcbasic_extractor(T* source, void (T::*func)(float))
    : rpcextractor_base<T,float>(source, func)
  {;}

  void post(pmt::pmt_t which_port, pmt::pmt_t msg)
  {
    (rpcextractor_base<T,float>::_source->*rpcextractor_base<T,float>::_func)
      (pmt::to_double(msg));
  }
};

template<typename T>
class rpcbasic_extractor<T,long> : public virtual rpcextractor_base<T,long>
{
public:
  rpcbasic_extractor(T* source, void (T::*func)(long))
    : rpcextractor_base<T,long>(source, func)
  {;}

  void post(pmt::pmt_t which_port, pmt::pmt_t msg)
  {
    (rpcextractor_base<T,long>::_source->*rpcextractor_base<T,long>::_func)
      (pmt::to_long(msg));
  }
};

template<typename T>
class rpcbasic_extractor<T,int> : public virtual rpcextractor_base<T,int>
{
public:
  rpcbasic_extractor(T* source, void (T::*func)(int))
    : rpcextractor_base<T,int>(source, func)
  {;}

  void post(pmt::pmt_t which_port, pmt::pmt_t msg)
  {
    (rpcextractor_base<T,int>::_source->*rpcextractor_base<T,int>::_func)
      (pmt::to_long(msg));
  }
};

template<typename T>
class rpcbasic_extractor<T,bool> : public virtual rpcextractor_base<T,bool>
{
public:
  rpcbasic_extractor(T* source, void (T::*func)(bool))
    : rpcextractor_base<T,bool>(source, func)
  {;}

  void post(pmt::pmt_t which_port, pmt::pmt_t msg)
  {
    (rpcextractor_base<T,bool>::_source->*rpcextractor_base<T,bool>::_func)
      (pmt::to_bool(msg));
  }
};

template<typename T>
class rpcbasic_extractor<T,std::complex<float> >
  : public virtual rpcextractor_base<T,std::complex<float> >
{
public:
  rpcbasic_extractor(T* source, void (T::*func)(std::complex<float>))
    : rpcextractor_base<T,std::complex<float> >(source, func)
  {;}

  void post(pmt::pmt_t which_port, pmt::pmt_t msg)
  {
    std::complex<float> k = static_cast<std::complex<float> >(pmt::to_complex(msg));
    (rpcextractor_base<T,std::complex<float> >::
     _source->*rpcextractor_base<T,std::complex<float> >::_func)(k);
  }
};

template<typename T>
class rpcbasic_extractor<T,std::complex<double> >
  : public virtual rpcextractor_base<T,std::complex<double> >
{
public:
  rpcbasic_extractor(T* source, void (T::*func)(std::complex<double>))
    : rpcextractor_base<T,std::complex<double> >(source, func)
  {;}

  void post(pmt::pmt_t which_port, pmt::pmt_t msg)
  {
    (rpcextractor_base<T,std::complex<double> >::
     _source->*rpcextractor_base<T,std::complex<double> >::_func)(pmt::to_complex(msg));
  }
};

template<typename T>
class rpcbasic_extractor<T,std::string>
  : public virtual rpcextractor_base<T,std::string>
{
public:
  rpcbasic_extractor(T* source, void (T::*func)(std::string))
    : rpcextractor_base<T,std::string>(source, func)
  {;}

  void post(pmt::pmt_t which_port, pmt::pmt_t msg)
  {
    (rpcextractor_base<T,std::string>::
     _source->*rpcextractor_base<T,std::string>::_func)(pmt::symbol_to_string(msg)); 
  }
};

template<typename T>
class rpcbasic_inserter<T,uint64_t> : public virtual rpcinserter_base<T,uint64_t>
{
public:
  rpcbasic_inserter(T* source, uint64_t (T::*func)() const)
    : rpcinserter_base<T,uint64_t>(source, func)
  {;}

  rpcbasic_inserter(T* source, uint64_t (T::*func)())
    : rpcinserter_base<T,uint64_t>(source, func)
  {;}

  pmt::pmt_t retrieve()
  {
    return pmt::from_uint64((rpcinserter_base<T,uint64_t>::
			     _source->*rpcinserter_base<T,uint64_t>::_func)());
  }
};

template<typename T>
class rpcbasic_inserter<T,std::vector< signed char > >
  : public virtual rpcinserter_base<T,std::vector< signed char > >
{
public:
  rpcbasic_inserter(T* source, std::vector< signed char > (T::*func)() const)
    : rpcinserter_base<T,std::vector< signed char > >(source, func)
  {;}

  rpcbasic_inserter(T* source, std::vector< signed char > (T::*func)())
    : rpcinserter_base<T,std::vector< signed char > >(source, func)
  {;}

  pmt::pmt_t retrieve()
  {
    std::vector< signed char >
      vec((rpcinserter_base<T,std::vector< signed char > >::
	   _source->*rpcinserter_base<T,std::vector< signed char > >::_func)()); 
    return pmt::init_s8vector(vec.size(), &vec[0]);
  }
};

template<typename T>
class rpcbasic_inserter<T,std::vector< short > >
  : public virtual rpcinserter_base<T,std::vector< short > >
{
public:
  rpcbasic_inserter(T* source, std::vector< short > (T::*func)() const)
    : rpcinserter_base<T,std::vector< short > >(source, func)
  {;}

  rpcbasic_inserter(T* source, std::vector< short > (T::*func)())
    : rpcinserter_base<T,std::vector< short > >(source, func)
  {;}

  pmt::pmt_t retrieve()
  {
    std::vector< short >
      vec((rpcinserter_base<T,std::vector< short > >::
	   _source->*rpcinserter_base<T,std::vector< short > >::_func)()); 
    return pmt::init_s16vector(vec.size(), &vec[0]);
  }
};

template<typename T>
class rpcbasic_inserter<T,std::vector< int > >
  : public virtual rpcinserter_base<T,std::vector< int > >
{
public:
  rpcbasic_inserter(T* source, std::vector<int > (T::*func)() const)
    : rpcinserter_base<T,std::vector<int > >(source, func)
  {;}

  rpcbasic_inserter(T* source, std::vector<int > (T::*func)())
    : rpcinserter_base<T,std::vector<int > >(source, func)
  {;}

  pmt::pmt_t retrieve()
  {
    std::vector< int >
      vec((rpcinserter_base<T,std::vector<int > >::
	   _source->*rpcinserter_base<T,std::vector< int > >::_func)()); 
    return pmt::init_s32vector(vec.size(), &vec[0]);
  }
};

template<typename T>
class rpcbasic_inserter<T,std::vector< std::complex<float> > >
  : public virtual rpcinserter_base<T,std::vector< std::complex<float> > >
{
public:
  rpcbasic_inserter(T* source, std::vector<std::complex<float> > (T::*func)() const)
    : rpcinserter_base<T,std::vector<std::complex<float> > >(source, func)
  {;}

  rpcbasic_inserter(T* source, std::vector<std::complex<float> > (T::*func)())
    : rpcinserter_base<T,std::vector<std::complex<float> > >(source, func)
  {;}

  pmt::pmt_t retrieve()
  {
    std::vector< std::complex<float> >
      vec((rpcinserter_base<T,std::vector<std::complex<float> > >::
	   _source->*rpcinserter_base<T,std::vector< std::complex<float> > >::_func)()); 
    return pmt::init_c32vector(vec.size(), &vec[0]);
  }
};

template<typename T>
class rpcbasic_inserter<T,std::vector< float> >
  : public virtual rpcinserter_base<T,std::vector< float > >
{
public:
  rpcbasic_inserter(T* source, std::vector<float> (T::*func)() const)
    : rpcinserter_base<T,std::vector<float > >(source, func)
  {;}
  
  rpcbasic_inserter(T* source, std::vector<float> (T::*func)())
    : rpcinserter_base<T,std::vector<float> >(source, func)
  {;}

  pmt::pmt_t retrieve()
  {
    std::vector< float > vec((rpcinserter_base<T,std::vector<float> >::
	      _source->*rpcinserter_base<T,std::vector< float> >::_func)()); 
    return pmt::init_f32vector(vec.size(), &vec[0]);
  }
};

template<typename T> 
class rpcbasic_inserter<T,std::vector< uint8_t> > 
  : public virtual rpcinserter_base<T,std::vector< uint8_t > > {
public:
  rpcbasic_inserter(T* source, std::vector<uint8_t> (T::*func)() const) 
    : rpcinserter_base<T,std::vector<uint8_t > >(source, func) 
  {;}

  rpcbasic_inserter(T* source, std::vector<uint8_t> (T::*func)()) 
    : rpcinserter_base<T,std::vector<uint8_t> >(source, func) 
  {;}

  pmt::pmt_t retrieve() 
  {
    std::vector< uint8_t > vec((rpcinserter_base<T,std::vector<uint8_t> >::
        _source->*rpcinserter_base<T,std::vector< uint8_t> >::_func)());
    return pmt::init_u8vector(vec.size(), &vec[0]); 
  }
};

template<typename T> 
class rpcbasic_inserter<T,std::complex<float> > 
  : public virtual rpcinserter_base<T,std::complex<float > > {
public:
  rpcbasic_inserter(T* source, std::complex<float> (T::*func)() const) 
    : rpcinserter_base<T,std::complex<float> >(source, func) 
  {;}

  rpcbasic_inserter(T* source, std::complex<float> (T::*func)()) 
    : rpcinserter_base<T,std::complex<float> >(source, func) 
  {;}

  pmt::pmt_t retrieve() 
  {
    std::complex<float > k((rpcinserter_base<T,std::complex<float> >::
                             _source->*rpcinserter_base<T,std::complex<float> >::_func)());
    return pmt::from_complex(k);
  }
};

template<typename T> 
class rpcbasic_inserter<T,std::complex<double> > 
  : public virtual rpcinserter_base<T,std::complex<double > > {
public:
  rpcbasic_inserter(T* source, std::complex<double> (T::*func)() const) 
    : rpcinserter_base<T,std::complex<double> >(source, func) 
  {;}

  rpcbasic_inserter(T* source, std::complex<double> (T::*func)()) 
    : rpcinserter_base<T,std::complex<double> >(source, func) 
  {;}

  pmt::pmt_t retrieve()
  {
    std::complex<double > k((rpcinserter_base<T,std::complex<double> >::
                             _source->*rpcinserter_base<T,std::complex<double> >::_func)());
    return pmt::from_complex(k);
  }
};

template <typename T>
struct rpc_register_base
{
  rpc_register_base() {count++;}
protected: static int count;
};

// Base class to inherit from and create universal shared pointers.
class rpcbasic_base
{
public:
  rpcbasic_base() {}
  virtual ~rpcbasic_base() {};
};

typedef boost::shared_ptr<rpcbasic_base> rpcbasic_sptr;

template<typename T, typename Tto>
struct rpcbasic_register_set : public rpcbasic_base
{
  // Function used to add a 'set' RPC call using a basic_block's alias.
  rpcbasic_register_set(const std::string& block_alias,
			const char* functionbase,
			void (T::*function)(Tto), 
			const pmt::pmt_t &min, const pmt::pmt_t &max, const pmt::pmt_t &def,
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
    d_object = dynamic_cast<T*>(global_block_registry.block_lookup(pmt::intern(block_alias)).get());
#ifdef RPCSERVER_ENABLED
    callbackregister_base::configureCallback_t
      extractor(new rpcbasic_extractor<T,Tto>(d_object, function), 
		minpriv_, std::string(units_),
		display_, std::string(desc_), min, max, def);
    std::ostringstream oss(std::ostringstream::out);
    oss << block_alias << "::" << functionbase;
    d_id = oss.str();
    //std::cerr << "REGISTERING SET: " << d_id << "  " << desc_ << std::endl;
    rpcmanager::get()->i()->registerConfigureCallback(d_id, extractor);
#endif
  }

  // Function used to add a 'set' RPC call using a name and the object
  rpcbasic_register_set(const std::string& name,
			const char* functionbase,
                        T* obj,
			void (T::*function)(Tto), 
			const pmt::pmt_t &min, const pmt::pmt_t &max, const pmt::pmt_t &def,
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
#ifdef RPCSERVER_ENABLED
    callbackregister_base::configureCallback_t
      extractor(new rpcbasic_extractor<T,Tto>(d_object, function), 
		minpriv_, std::string(units_),
		display_, std::string(desc_), min, max, def);
    std::ostringstream oss(std::ostringstream::out);
    oss << name << "::" << functionbase;
    d_id = oss.str();
    //std::cerr << "REGISTERING SET: " << d_id << "  " << desc_ << std::endl;
    rpcmanager::get()->i()->registerConfigureCallback(d_id, extractor);
#endif
  }

  ~rpcbasic_register_set()
  {
#ifdef RPCSERVER_ENABLED
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
  T *d_object;
};


template<typename T, typename Tfrom>
class rpcbasic_register_get : public rpcbasic_base
{
public:
  // Function used to add a 'set' RPC call using a basic_block's alias.
  // primary constructor to allow for T get() functions
  rpcbasic_register_get(const std::string& block_alias,
			const char* functionbase,
			Tfrom (T::*function)(), 
			const pmt::pmt_t &min, const pmt::pmt_t &max, const pmt::pmt_t &def,
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
    d_object = dynamic_cast<T*>(global_block_registry.block_lookup(pmt::intern(block_alias)).get());
#ifdef RPCSERVER_ENABLED
    callbackregister_base::queryCallback_t
      inserter(new rpcbasic_inserter<T,Tfrom>(d_object, function), 
	       minpriv_, std::string(units_), display_, std::string(desc_), min, max, def);
    std::ostringstream oss(std::ostringstream::out);
    oss << block_alias << "::" << functionbase;
    d_id = oss.str();
    //std::cerr << "REGISTERING GET: " << d_id << "  " << desc_ << std::endl;
    rpcmanager::get()->i()->registerQueryCallback(d_id, inserter);
#endif
  }

	
  // alternate constructor to allow for T get() const functions
  rpcbasic_register_get(const std::string& block_alias,
			const char* functionbase,
			Tfrom (T::*function)() const, 
			const pmt::pmt_t &min, const pmt::pmt_t &max, const pmt::pmt_t &def,
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
    d_object = dynamic_cast<T*>(global_block_registry.block_lookup(pmt::intern(block_alias)).get());
#ifdef RPCSERVER_ENABLED
    callbackregister_base::queryCallback_t
      inserter(new rpcbasic_inserter<T,Tfrom>(d_object, (Tfrom (T::*)())function), 
	       minpriv_, std::string(units_), display_, std::string(desc_), min, max, def);
    std::ostringstream oss(std::ostringstream::out);
    oss << block_alias << "::" << functionbase;
    d_id = oss.str();
    //std::cerr << "REGISTERING GET CONST: " << d_id << "   " << desc_ << "   " << display_ << std::endl;
    rpcmanager::get()->i()->registerQueryCallback(d_id, inserter);
#endif
  }

  // Function used to add a 'set' RPC call using a name and the object
  // primary constructor to allow for T get() functions
  rpcbasic_register_get(const std::string& name,
			const char* functionbase,
                        T* obj,
			Tfrom (T::*function)(), 
			const pmt::pmt_t &min, const pmt::pmt_t &max, const pmt::pmt_t &def,
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
#ifdef RPCSERVER_ENABLED
    callbackregister_base::queryCallback_t
      inserter(new rpcbasic_inserter<T,Tfrom>(d_object, function), 
	       minpriv_, std::string(units_), display_, std::string(desc_), min, max, def);
    std::ostringstream oss(std::ostringstream::out);
    oss << name << "::" << functionbase;
    d_id = oss.str();
    //std::cerr << "REGISTERING GET: " << d_id << "  " << desc_ << std::endl;
    rpcmanager::get()->i()->registerQueryCallback(d_id, inserter);
#endif
  }

	
  // alternate constructor to allow for T get() const functions
  rpcbasic_register_get(const std::string& name,
			const char* functionbase,
                        T* obj,
			Tfrom (T::*function)() const, 
			const pmt::pmt_t &min, const pmt::pmt_t &max, const pmt::pmt_t &def,
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
#ifdef RPCSERVER_ENABLED
    callbackregister_base::queryCallback_t
      inserter(new rpcbasic_inserter<T,Tfrom>(d_object, (Tfrom (T::*)())function), 
	       minpriv_, std::string(units_), display_, std::string(desc_), min, max, def);
    std::ostringstream oss(std::ostringstream::out);
    oss << name << "::" << functionbase;
    d_id = oss.str();
    //std::cerr << "REGISTERING GET CONST: " << d_id << "   " << desc_ << "   " << display_ << std::endl;
    rpcmanager::get()->i()->registerQueryCallback(d_id, inserter);
#endif
  }

  ~rpcbasic_register_get()
  {
#ifdef RPCSERVER_ENABLED
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
  T *d_object;
};

/*
 * This class can wrap a pre-existing variable type for you
 * it will define the getter and rpcregister call for you.
 * 
 * It should be used for read-only getters.
 *
 */
template<typename Tfrom>
class rpcbasic_register_variable : public rpcbasic_base
{
protected:
  rpcbasic_register_get< rpcbasic_register_variable<Tfrom>, Tfrom > d_rpc_reg;
  Tfrom *d_variable;
  Tfrom get() { return *d_variable; }
public:
  // empty constructor which should never be called but needs to exist for ues in varous STL data structures
  void setptr(Tfrom* _variable){  rpcbasic_register_variable<Tfrom>::d_variable = _variable; }
  rpcbasic_register_variable() :
    d_rpc_reg("FAIL", "FAIL", this, &rpcbasic_register_variable::get,
	      pmt::PMT_NIL, pmt::PMT_NIL, pmt::PMT_NIL, DISPNULL,
	      "FAIL", "FAIL", RPC_PRIVLVL_MIN),
    d_variable(NULL)
  {
    throw std::runtime_error("ERROR: rpcbasic_register_variable called with no args. If this happens, someone has tried to use rpcbasic_register_variable incorrectly.");
  };

  rpcbasic_register_variable(const std::string& namebase,
			     const char* functionbase,
			     Tfrom *variable,
			     const pmt::pmt_t &min, const pmt::pmt_t &max, const pmt::pmt_t &def,
			     const char* units_ = "",
			     const char* desc_ = "",
			     priv_lvl_t minpriv_ = RPC_PRIVLVL_MIN,
			     DisplayType display_=DISPNULL) :
    d_rpc_reg(namebase, functionbase, this, &rpcbasic_register_variable::get,
	      min, max, def, units_, desc_, minpriv_, display_),
    d_variable(variable)
  {
    //std::cerr << "REGISTERING VAR: " << " " << desc_ << std::endl;
  }
};

template<typename Tfrom> class rpcbasic_register_variable_rw : public rpcbasic_register_variable<Tfrom> {
  private:
    rpcbasic_register_set< rpcbasic_register_variable_rw<Tfrom>, Tfrom > d_rpc_regset;
  public:
    // empty constructor which should never be called but needs to exist for ues in varous STL data structures
    rpcbasic_register_variable_rw()  :
            d_rpc_regset("FAIL","FAIL",this,&rpcbasic_register_variable<Tfrom>::get,pmt::PMT_NIL,pmt::PMT_NIL,pmt::PMT_NIL,DISPNULL,"FAIL","FAIL",RPC_PRIVLVL_MIN)
        {
        throw std::runtime_error("ERROR: rpcbasic_register_variable_rw called with no args. if this happens someone used rpcbasic_register_variable_rw incorrectly.\n");
        };
    void set(Tfrom _variable){  *(rpcbasic_register_variable<Tfrom>::d_variable) = _variable; }
    rpcbasic_register_variable_rw(
        const std::string& namebase,
        const char* functionbase,
        Tfrom *variable,
        const pmt::pmt_t &min, const pmt::pmt_t &max, const pmt::pmt_t &def,
        const char* units_ = "",
        const char* desc_ = "",
        priv_lvl_t minpriv = RPC_PRIVLVL_MIN,
        DisplayType display_=DISPNULL) :
            rpcbasic_register_variable<Tfrom>(namebase,functionbase,variable,min,max,def,units_,desc_),
            d_rpc_regset(namebase,functionbase,this,&rpcbasic_register_variable_rw::set,min,max,def,units_,desc_,minpriv,display_)
         {
        // no action
        }
};




#endif
