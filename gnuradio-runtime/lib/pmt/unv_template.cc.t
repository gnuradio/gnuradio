////////////////////////////////////////////////////////////////////////////
//                           pmt_@TAG@vector
////////////////////////////////////////////////////////////////////////////

namespace pmt {

static pmt_@TAG@vector *
_@TAG@vector(pmt_t x)
{
  return dynamic_cast<pmt_@TAG@vector*>(x.get());
}


pmt_@TAG@vector::pmt_@TAG@vector(size_t k, @TYPE@ fill)
  : d_v(k)
{
  for (size_t i = 0; i < k; i++)
    d_v[i] = fill;
}

pmt_@TAG@vector::pmt_@TAG@vector(size_t k, const @TYPE@ *data)
  : d_v(k)
{
  memcpy( &d_v[0], data, k * sizeof(@TYPE@) );
}

@TYPE@
pmt_@TAG@vector::ref(size_t k) const
{
  if (k >= length())
    throw out_of_range("pmt_@TAG@vector_ref", from_long(k));
  return d_v[k];
}

void
pmt_@TAG@vector::set(size_t k, @TYPE@ x)
{
  if (k >= length())
    throw out_of_range("pmt_@TAG@vector_set", from_long(k));
  d_v[k] = x;
}

const @TYPE@ *
pmt_@TAG@vector::elements(size_t &len)
{
  len = length();
  return &d_v[0];
}

@TYPE@ *
pmt_@TAG@vector::writable_elements(size_t &len)
{
  len = length();
  return &d_v[0];
}

const void*
pmt_@TAG@vector::uniform_elements(size_t &len)
{
  len = length() * sizeof(@TYPE@);
  return &d_v[0];
}

void*
pmt_@TAG@vector::uniform_writable_elements(size_t &len)
{
  len = length() * sizeof(@TYPE@);
  return &d_v[0];
}

bool
is_@TAG@vector(pmt_t obj)
{
  return obj->is_@TAG@vector();
}

pmt_t
make_@TAG@vector(size_t k, @TYPE@ fill)
{
  return pmt_t(new pmt_@TAG@vector(k, fill));
}

pmt_t
init_@TAG@vector(size_t k, const @TYPE@ *data)
{
  return pmt_t(new pmt_@TAG@vector(k, data));
}

pmt_t
init_@TAG@vector(size_t k, const std::vector< @TYPE@ > &data)
{

  return pmt_t(new pmt_@TAG@vector(k, &data[0]));
}

@TYPE@
@TAG@vector_ref(pmt_t vector, size_t k)
{
  if (!vector->is_@TAG@vector())
    throw wrong_type("pmt_@TAG@vector_ref", vector);
  return _@TAG@vector(vector)->ref(k);
}

void
@TAG@vector_set(pmt_t vector, size_t k, @TYPE@ obj)
{
  if (!vector->is_@TAG@vector())
    throw wrong_type("pmt_@TAG@vector_set", vector);
  _@TAG@vector(vector)->set(k, obj);
}

const @TYPE@ *
@TAG@vector_elements(pmt_t vector, size_t &len)
{
  if (!vector->is_@TAG@vector())
    throw wrong_type("pmt_@TAG@vector_elements", vector);
  return _@TAG@vector(vector)->elements(len);
}

const std::vector< @TYPE@ >
@TAG@vector_elements(pmt_t vector)
{
  if (!vector->is_@TAG@vector())
    throw wrong_type("pmt_@TAG@vector_elements", vector);
  size_t len;
  const @TYPE@ *array = _@TAG@vector(vector)->elements(len);
  const std::vector< @TYPE@ > vec(array, array+len);
  return vec;
}


@TYPE@ *
@TAG@vector_writable_elements(pmt_t vector, size_t &len)
{
  if (!vector->is_@TAG@vector())
    throw wrong_type("pmt_@TAG@vector_writable_elements", vector);
  return _@TAG@vector(vector)->writable_elements(len);
}

const std::string
pmt_@TAG@vector::string_ref(size_t k) const
{
  return boost::lexical_cast< std::string, @TYPE@ > (ref(k));
}

} /* namespace pmt */
