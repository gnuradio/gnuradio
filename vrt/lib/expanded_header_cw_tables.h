inline static size_t cw_header_len(int cw){
  static const size_t s_cw_header_len[32] = {
    1, 2, 3, 4, 2, 3, 4, 5, 3, 4, 5, 6, 4, 5, 6, 7, 1, 2, 3, 4, 2, 3, 4, 5, 3, 4, 5, 6, 4, 5, 6, 7, 
  };
  return s_cw_header_len[cw];
}

inline static size_t cw_trailer_len(int cw){
  static const size_t s_cw_trailer_len[32] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
  };
  return s_cw_trailer_len[cw];
}

