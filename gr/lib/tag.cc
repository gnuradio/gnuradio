#include <gnuradio/tag.h>

namespace gr {

bool tag_t::operator==(const tag_t& rhs) const
{
    return (rhs.offset() == offset() && rhs.map() == map());
}
bool tag_t::operator!=(const tag_t& rhs) const
{
    return (rhs.offset() != offset() && rhs.map() != map());
}

size_t tag_t::serialize(std::streambuf& sb) const
{
    size_t ret = 0;
    std::ostream ss(&sb);
    ss.write((const char*)&_offset, sizeof(uint64_t));
    ret += sizeof(uint64_t);
    ret += pmtf::pmt(_map).serialize(sb);

    return ret;
}

tag_t tag_t::deserialize(std::streambuf& sb)
{
    uint64_t tmp_offset;
    sb.sgetn((char*)&(tmp_offset), sizeof(uint64_t));
    return tag_t(tmp_offset, pmtf::pmt::deserialize(sb));
}


} // namespace gr
