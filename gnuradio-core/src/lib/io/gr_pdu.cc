
#include <gr_pdu.h>

size_t gr_pdu_itemsize(gr_pdu_vector_type type){
    switch(type){
        case BYTE:
            return 1;
        case FLOAT:
            return sizeof(float);
        case COMPLEX:
            return sizeof(gr_complex);
        default:
            throw std::runtime_error("bad type!");
    }
}

bool gr_pdu_type_matches(gr_pdu_vector_type type, pmt::pmt_t v){
    switch(type){
        case BYTE:
            return pmt::pmt_is_u8vector(v);
        case FLOAT:
            return pmt::pmt_is_f32vector(v);
        case COMPLEX:
            return pmt::pmt_is_c32vector(v);
        default:
            throw std::runtime_error("bad type!");
    }
}

pmt::pmt_t gr_pdu_make_vector(gr_pdu_vector_type type, const uint8_t* buf, size_t items){
    switch(type){
        case BYTE:
            return pmt::pmt_init_u8vector(items, buf);
        case FLOAT:
            return pmt::pmt_init_f32vector(items, (const float*)buf);
        case COMPLEX:
            return pmt::pmt_init_c32vector(items, (const gr_complex*)buf);
        default:
            throw std::runtime_error("bad type!");
    }
}


