#include "encoder_turbo.h"

encoder_turbo::encoder_turbo
(int n_code_inputs,
 int n_code_outputs,
 const std::vector<encoder_convolutional*> &encoders,
 const std::vector<size_t> &interleavers)
{
  // need error checking on inputs

  d_encoders = encoders;
  d_interleavers = interleavers;
}

// dummy stuff for now to test std::vector<gr_streams_convolutional...> stuff

size_t
encoder_turbo::compute_n_input_bits
(size_t n_output_bits)
{
  return (0);
}

size_t
encoder_turbo::compute_n_output_bits
(size_t n_input_bits)
{
  return (0);
}

void
encoder_turbo::encode_private
(const char** in_buf,
 char** out_buf)
{
}

char
encoder_turbo::get_next_bit
(const char** in_buf,
 size_t code_input_n)
{
  return (0);
}

void
encoder_turbo::output_bit
(char t_out_bit,
 char** out_buf,
 size_t t_output_stream)
{
}
