
////////////////////////////////////////////////////////////////////////////
//                           pmt_@TAG@vector
////////////////////////////////////////////////////////////////////////////

class pmt_@TAG@vector : public pmt_uniform_vector
{
  std::vector< @TYPE@ >	d_v;

public:
  pmt_@TAG@vector(size_t k, @TYPE@ fill);
  pmt_@TAG@vector(size_t k, const @TYPE@ *data);
  // ~pmt_@TAG@vector();

  bool is_@TAG@vector() const { return true; }
  size_t length() const { return d_v.size(); }
  size_t itemsize() const { return sizeof(@TYPE@); }
  @TYPE@ ref(size_t k) const;
  void set(size_t k, @TYPE@ x);
  const @TYPE@ *elements(size_t &len);
  @TYPE@ *writable_elements(size_t &len);
  const void *uniform_elements(size_t &len);
  void *uniform_writable_elements(size_t &len);
  virtual const std::string string_ref(size_t k) const;
};
