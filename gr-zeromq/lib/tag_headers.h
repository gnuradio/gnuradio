
#include <gnuradio/io_signature.h>
#include <gnuradio/block.h>
#include <sstream>
#include <cstring>

std::string gen_tag_header(uint64_t &offset, std::vector<gr::tag_t> &tags){
    std::stringstream ss;
    size_t ntags = tags.size();
    ss.write( reinterpret_cast< const char* >( offset ), sizeof(uint64_t) );  // offset
    ss.write( reinterpret_cast< const char* >( &ntags ), sizeof(size_t) );      // num tags
    std::stringbuf sb("");
    for(size_t i=0; i<tags.size(); i++){
        ss.write( reinterpret_cast< const char* >( &tags[i].offset ), sizeof(uint64_t) );   // offset
        sb.str("");
        pmt::serialize( tags[i].key, sb );                                           // key
        pmt::serialize( tags[i].value, sb );                                         // value
        pmt::serialize( tags[i].srcid, sb );                                         // srcid
        ss.write( sb.str().c_str() , sb.str().length() );   // offset
        }
    return ss.str();
}


