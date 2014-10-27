
#include <gnuradio/io_signature.h>
#include <gnuradio/block.h>
#include <sstream>
#include <cstring>

std::string gen_tag_header(uint64_t &offset, std::vector<gr::tag_t> &tags);
std::string parse_tag_header(std::string &buf_in, uint64_t &offset_out, std::vector<gr::tag_t> tags_out);
