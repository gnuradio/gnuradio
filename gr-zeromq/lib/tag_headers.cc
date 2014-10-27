
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

std::string parse_tag_header(std::string &buf_in, uint64_t &offset_out, std::vector<gr::tag_t> tags_out){
    std::istringstream iss( buf_in );
    size_t   rcv_ntags;
    iss.read( (char*)&offset_out, sizeof(uint64_t ) );
    iss.read( (char*)&rcv_ntags,  sizeof(size_t   ) );
    for(size_t i=0; i<rcv_ntags; i++){
        gr::tag_t newtag;       
        iss.read( (char*)&newtag.offset, sizeof(uint64_t ) );
        std::stringbuf sb( iss.str() );
        newtag.key   = pmt::deserialize( sb );
        newtag.value = pmt::deserialize( sb );
        newtag.srcid = pmt::deserialize( sb );
        tags_out.push_back(newtag);
        iss.str(sb.str());
        }
    return iss.str();
}
