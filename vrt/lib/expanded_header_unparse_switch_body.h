  case 0:
    *n32_bit_words_header = 1;
    *n32_bit_words_trailer = 0;
    break;

  case 1:
    header[1] = htonl(h->stream_id);
    *n32_bit_words_header = 2;
    *n32_bit_words_trailer = 0;
    break;

  case 2:
    header[1] = htonl((uint32_t)((h->class_id >> 32) & 0xffffffff));
    header[2] = htonl((uint32_t)((h->class_id >>  0) & 0xffffffff));
    *n32_bit_words_header = 3;
    *n32_bit_words_trailer = 0;
    break;

  case 3:
    header[1] = htonl(h->stream_id);
    header[2] = htonl((uint32_t)((h->class_id >> 32) & 0xffffffff));
    header[3] = htonl((uint32_t)((h->class_id >>  0) & 0xffffffff));
    *n32_bit_words_header = 4;
    *n32_bit_words_trailer = 0;
    break;

  case 4:
    header[1] = htonl(h->integer_secs);
    *n32_bit_words_header = 2;
    *n32_bit_words_trailer = 0;
    break;

  case 5:
    header[1] = htonl(h->stream_id);
    header[2] = htonl(h->integer_secs);
    *n32_bit_words_header = 3;
    *n32_bit_words_trailer = 0;
    break;

  case 6:
    header[1] = htonl((uint32_t)((h->class_id >> 32) & 0xffffffff));
    header[2] = htonl((uint32_t)((h->class_id >>  0) & 0xffffffff));
    header[3] = htonl(h->integer_secs);
    *n32_bit_words_header = 4;
    *n32_bit_words_trailer = 0;
    break;

  case 7:
    header[1] = htonl(h->stream_id);
    header[2] = htonl((uint32_t)((h->class_id >> 32) & 0xffffffff));
    header[3] = htonl((uint32_t)((h->class_id >>  0) & 0xffffffff));
    header[4] = htonl(h->integer_secs);
    *n32_bit_words_header = 5;
    *n32_bit_words_trailer = 0;
    break;

  case 8:
    header[1] = htonl((uint32_t)((h->fractional_secs >> 32) & 0xffffffff));
    header[2] = htonl((uint32_t)((h->fractional_secs >>  0) & 0xffffffff));
    *n32_bit_words_header = 3;
    *n32_bit_words_trailer = 0;
    break;

  case 9:
    header[1] = htonl(h->stream_id);
    header[2] = htonl((uint32_t)((h->fractional_secs >> 32) & 0xffffffff));
    header[3] = htonl((uint32_t)((h->fractional_secs >>  0) & 0xffffffff));
    *n32_bit_words_header = 4;
    *n32_bit_words_trailer = 0;
    break;

  case 10:
    header[1] = htonl((uint32_t)((h->class_id >> 32) & 0xffffffff));
    header[2] = htonl((uint32_t)((h->class_id >>  0) & 0xffffffff));
    header[3] = htonl((uint32_t)((h->fractional_secs >> 32) & 0xffffffff));
    header[4] = htonl((uint32_t)((h->fractional_secs >>  0) & 0xffffffff));
    *n32_bit_words_header = 5;
    *n32_bit_words_trailer = 0;
    break;

  case 11:
    header[1] = htonl(h->stream_id);
    header[2] = htonl((uint32_t)((h->class_id >> 32) & 0xffffffff));
    header[3] = htonl((uint32_t)((h->class_id >>  0) & 0xffffffff));
    header[4] = htonl((uint32_t)((h->fractional_secs >> 32) & 0xffffffff));
    header[5] = htonl((uint32_t)((h->fractional_secs >>  0) & 0xffffffff));
    *n32_bit_words_header = 6;
    *n32_bit_words_trailer = 0;
    break;

  case 12:
    header[1] = htonl(h->integer_secs);
    header[2] = htonl((uint32_t)((h->fractional_secs >> 32) & 0xffffffff));
    header[3] = htonl((uint32_t)((h->fractional_secs >>  0) & 0xffffffff));
    *n32_bit_words_header = 4;
    *n32_bit_words_trailer = 0;
    break;

  case 13:
    header[1] = htonl(h->stream_id);
    header[2] = htonl(h->integer_secs);
    header[3] = htonl((uint32_t)((h->fractional_secs >> 32) & 0xffffffff));
    header[4] = htonl((uint32_t)((h->fractional_secs >>  0) & 0xffffffff));
    *n32_bit_words_header = 5;
    *n32_bit_words_trailer = 0;
    break;

  case 14:
    header[1] = htonl((uint32_t)((h->class_id >> 32) & 0xffffffff));
    header[2] = htonl((uint32_t)((h->class_id >>  0) & 0xffffffff));
    header[3] = htonl(h->integer_secs);
    header[4] = htonl((uint32_t)((h->fractional_secs >> 32) & 0xffffffff));
    header[5] = htonl((uint32_t)((h->fractional_secs >>  0) & 0xffffffff));
    *n32_bit_words_header = 6;
    *n32_bit_words_trailer = 0;
    break;

  case 15:
    header[1] = htonl(h->stream_id);
    header[2] = htonl((uint32_t)((h->class_id >> 32) & 0xffffffff));
    header[3] = htonl((uint32_t)((h->class_id >>  0) & 0xffffffff));
    header[4] = htonl(h->integer_secs);
    header[5] = htonl((uint32_t)((h->fractional_secs >> 32) & 0xffffffff));
    header[6] = htonl((uint32_t)((h->fractional_secs >>  0) & 0xffffffff));
    *n32_bit_words_header = 7;
    *n32_bit_words_trailer = 0;
    break;

  case 16:
    *n32_bit_words_header = 1;
    trailer[0] = htonl(h->trailer);
    *n32_bit_words_trailer = 1;
    break;

  case 17:
    header[1] = htonl(h->stream_id);
    *n32_bit_words_header = 2;
    trailer[0] = htonl(h->trailer);
    *n32_bit_words_trailer = 1;
    break;

  case 18:
    header[1] = htonl((uint32_t)((h->class_id >> 32) & 0xffffffff));
    header[2] = htonl((uint32_t)((h->class_id >>  0) & 0xffffffff));
    *n32_bit_words_header = 3;
    trailer[0] = htonl(h->trailer);
    *n32_bit_words_trailer = 1;
    break;

  case 19:
    header[1] = htonl(h->stream_id);
    header[2] = htonl((uint32_t)((h->class_id >> 32) & 0xffffffff));
    header[3] = htonl((uint32_t)((h->class_id >>  0) & 0xffffffff));
    *n32_bit_words_header = 4;
    trailer[0] = htonl(h->trailer);
    *n32_bit_words_trailer = 1;
    break;

  case 20:
    header[1] = htonl(h->integer_secs);
    *n32_bit_words_header = 2;
    trailer[0] = htonl(h->trailer);
    *n32_bit_words_trailer = 1;
    break;

  case 21:
    header[1] = htonl(h->stream_id);
    header[2] = htonl(h->integer_secs);
    *n32_bit_words_header = 3;
    trailer[0] = htonl(h->trailer);
    *n32_bit_words_trailer = 1;
    break;

  case 22:
    header[1] = htonl((uint32_t)((h->class_id >> 32) & 0xffffffff));
    header[2] = htonl((uint32_t)((h->class_id >>  0) & 0xffffffff));
    header[3] = htonl(h->integer_secs);
    *n32_bit_words_header = 4;
    trailer[0] = htonl(h->trailer);
    *n32_bit_words_trailer = 1;
    break;

  case 23:
    header[1] = htonl(h->stream_id);
    header[2] = htonl((uint32_t)((h->class_id >> 32) & 0xffffffff));
    header[3] = htonl((uint32_t)((h->class_id >>  0) & 0xffffffff));
    header[4] = htonl(h->integer_secs);
    *n32_bit_words_header = 5;
    trailer[0] = htonl(h->trailer);
    *n32_bit_words_trailer = 1;
    break;

  case 24:
    header[1] = htonl((uint32_t)((h->fractional_secs >> 32) & 0xffffffff));
    header[2] = htonl((uint32_t)((h->fractional_secs >>  0) & 0xffffffff));
    *n32_bit_words_header = 3;
    trailer[0] = htonl(h->trailer);
    *n32_bit_words_trailer = 1;
    break;

  case 25:
    header[1] = htonl(h->stream_id);
    header[2] = htonl((uint32_t)((h->fractional_secs >> 32) & 0xffffffff));
    header[3] = htonl((uint32_t)((h->fractional_secs >>  0) & 0xffffffff));
    *n32_bit_words_header = 4;
    trailer[0] = htonl(h->trailer);
    *n32_bit_words_trailer = 1;
    break;

  case 26:
    header[1] = htonl((uint32_t)((h->class_id >> 32) & 0xffffffff));
    header[2] = htonl((uint32_t)((h->class_id >>  0) & 0xffffffff));
    header[3] = htonl((uint32_t)((h->fractional_secs >> 32) & 0xffffffff));
    header[4] = htonl((uint32_t)((h->fractional_secs >>  0) & 0xffffffff));
    *n32_bit_words_header = 5;
    trailer[0] = htonl(h->trailer);
    *n32_bit_words_trailer = 1;
    break;

  case 27:
    header[1] = htonl(h->stream_id);
    header[2] = htonl((uint32_t)((h->class_id >> 32) & 0xffffffff));
    header[3] = htonl((uint32_t)((h->class_id >>  0) & 0xffffffff));
    header[4] = htonl((uint32_t)((h->fractional_secs >> 32) & 0xffffffff));
    header[5] = htonl((uint32_t)((h->fractional_secs >>  0) & 0xffffffff));
    *n32_bit_words_header = 6;
    trailer[0] = htonl(h->trailer);
    *n32_bit_words_trailer = 1;
    break;

  case 28:
    header[1] = htonl(h->integer_secs);
    header[2] = htonl((uint32_t)((h->fractional_secs >> 32) & 0xffffffff));
    header[3] = htonl((uint32_t)((h->fractional_secs >>  0) & 0xffffffff));
    *n32_bit_words_header = 4;
    trailer[0] = htonl(h->trailer);
    *n32_bit_words_trailer = 1;
    break;

  case 29:
    header[1] = htonl(h->stream_id);
    header[2] = htonl(h->integer_secs);
    header[3] = htonl((uint32_t)((h->fractional_secs >> 32) & 0xffffffff));
    header[4] = htonl((uint32_t)((h->fractional_secs >>  0) & 0xffffffff));
    *n32_bit_words_header = 5;
    trailer[0] = htonl(h->trailer);
    *n32_bit_words_trailer = 1;
    break;

  case 30:
    header[1] = htonl((uint32_t)((h->class_id >> 32) & 0xffffffff));
    header[2] = htonl((uint32_t)((h->class_id >>  0) & 0xffffffff));
    header[3] = htonl(h->integer_secs);
    header[4] = htonl((uint32_t)((h->fractional_secs >> 32) & 0xffffffff));
    header[5] = htonl((uint32_t)((h->fractional_secs >>  0) & 0xffffffff));
    *n32_bit_words_header = 6;
    trailer[0] = htonl(h->trailer);
    *n32_bit_words_trailer = 1;
    break;

  case 31:
    header[1] = htonl(h->stream_id);
    header[2] = htonl((uint32_t)((h->class_id >> 32) & 0xffffffff));
    header[3] = htonl((uint32_t)((h->class_id >>  0) & 0xffffffff));
    header[4] = htonl(h->integer_secs);
    header[5] = htonl((uint32_t)((h->fractional_secs >> 32) & 0xffffffff));
    header[6] = htonl((uint32_t)((h->fractional_secs >>  0) & 0xffffffff));
    *n32_bit_words_header = 7;
    trailer[0] = htonl(h->trailer);
    *n32_bit_words_trailer = 1;
    break;

