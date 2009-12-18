  case 0:
    h->stream_id = 0;
    h->class_id = 0;
    h->integer_secs = 0;
    h->fractional_secs = 0;
    h->trailer = 0;
    break;

  case 1:
    h->stream_id = ntohl(p[1]);
    h->class_id = 0;
    h->integer_secs = 0;
    h->fractional_secs = 0;
    h->trailer = 0;
    break;

  case 2:
    h->stream_id = 0;
    h->class_id = ((uint64_t)(ntohl(p[1])) << 32) | ntohl(p[2]);
    h->integer_secs = 0;
    h->fractional_secs = 0;
    h->trailer = 0;
    break;

  case 3:
    h->stream_id = ntohl(p[1]);
    h->class_id = ((uint64_t)(ntohl(p[2])) << 32) | ntohl(p[3]);
    h->integer_secs = 0;
    h->fractional_secs = 0;
    h->trailer = 0;
    break;

  case 4:
    h->stream_id = 0;
    h->class_id = 0;
    h->integer_secs = ntohl(p[1]);
    h->fractional_secs = 0;
    h->trailer = 0;
    break;

  case 5:
    h->stream_id = ntohl(p[1]);
    h->class_id = 0;
    h->integer_secs = ntohl(p[2]);
    h->fractional_secs = 0;
    h->trailer = 0;
    break;

  case 6:
    h->stream_id = 0;
    h->class_id = ((uint64_t)(ntohl(p[1])) << 32) | ntohl(p[2]);
    h->integer_secs = ntohl(p[3]);
    h->fractional_secs = 0;
    h->trailer = 0;
    break;

  case 7:
    h->stream_id = ntohl(p[1]);
    h->class_id = ((uint64_t)(ntohl(p[2])) << 32) | ntohl(p[3]);
    h->integer_secs = ntohl(p[4]);
    h->fractional_secs = 0;
    h->trailer = 0;
    break;

  case 8:
    h->stream_id = 0;
    h->class_id = 0;
    h->integer_secs = 0;
    h->fractional_secs = ((uint64_t)(ntohl(p[1])) << 32) | ntohl(p[2]);
    h->trailer = 0;
    break;

  case 9:
    h->stream_id = ntohl(p[1]);
    h->class_id = 0;
    h->integer_secs = 0;
    h->fractional_secs = ((uint64_t)(ntohl(p[2])) << 32) | ntohl(p[3]);
    h->trailer = 0;
    break;

  case 10:
    h->stream_id = 0;
    h->class_id = ((uint64_t)(ntohl(p[1])) << 32) | ntohl(p[2]);
    h->integer_secs = 0;
    h->fractional_secs = ((uint64_t)(ntohl(p[3])) << 32) | ntohl(p[4]);
    h->trailer = 0;
    break;

  case 11:
    h->stream_id = ntohl(p[1]);
    h->class_id = ((uint64_t)(ntohl(p[2])) << 32) | ntohl(p[3]);
    h->integer_secs = 0;
    h->fractional_secs = ((uint64_t)(ntohl(p[4])) << 32) | ntohl(p[5]);
    h->trailer = 0;
    break;

  case 12:
    h->stream_id = 0;
    h->class_id = 0;
    h->integer_secs = ntohl(p[1]);
    h->fractional_secs = ((uint64_t)(ntohl(p[2])) << 32) | ntohl(p[3]);
    h->trailer = 0;
    break;

  case 13:
    h->stream_id = ntohl(p[1]);
    h->class_id = 0;
    h->integer_secs = ntohl(p[2]);
    h->fractional_secs = ((uint64_t)(ntohl(p[3])) << 32) | ntohl(p[4]);
    h->trailer = 0;
    break;

  case 14:
    h->stream_id = 0;
    h->class_id = ((uint64_t)(ntohl(p[1])) << 32) | ntohl(p[2]);
    h->integer_secs = ntohl(p[3]);
    h->fractional_secs = ((uint64_t)(ntohl(p[4])) << 32) | ntohl(p[5]);
    h->trailer = 0;
    break;

  case 15:
    h->stream_id = ntohl(p[1]);
    h->class_id = ((uint64_t)(ntohl(p[2])) << 32) | ntohl(p[3]);
    h->integer_secs = ntohl(p[4]);
    h->fractional_secs = ((uint64_t)(ntohl(p[5])) << 32) | ntohl(p[6]);
    h->trailer = 0;
    break;

  case 16:
    h->stream_id = 0;
    h->class_id = 0;
    h->integer_secs = 0;
    h->fractional_secs = 0;
    h->trailer = ntohl(p[len-1]);
    break;

  case 17:
    h->stream_id = ntohl(p[1]);
    h->class_id = 0;
    h->integer_secs = 0;
    h->fractional_secs = 0;
    h->trailer = ntohl(p[len-1]);
    break;

  case 18:
    h->stream_id = 0;
    h->class_id = ((uint64_t)(ntohl(p[1])) << 32) | ntohl(p[2]);
    h->integer_secs = 0;
    h->fractional_secs = 0;
    h->trailer = ntohl(p[len-1]);
    break;

  case 19:
    h->stream_id = ntohl(p[1]);
    h->class_id = ((uint64_t)(ntohl(p[2])) << 32) | ntohl(p[3]);
    h->integer_secs = 0;
    h->fractional_secs = 0;
    h->trailer = ntohl(p[len-1]);
    break;

  case 20:
    h->stream_id = 0;
    h->class_id = 0;
    h->integer_secs = ntohl(p[1]);
    h->fractional_secs = 0;
    h->trailer = ntohl(p[len-1]);
    break;

  case 21:
    h->stream_id = ntohl(p[1]);
    h->class_id = 0;
    h->integer_secs = ntohl(p[2]);
    h->fractional_secs = 0;
    h->trailer = ntohl(p[len-1]);
    break;

  case 22:
    h->stream_id = 0;
    h->class_id = ((uint64_t)(ntohl(p[1])) << 32) | ntohl(p[2]);
    h->integer_secs = ntohl(p[3]);
    h->fractional_secs = 0;
    h->trailer = ntohl(p[len-1]);
    break;

  case 23:
    h->stream_id = ntohl(p[1]);
    h->class_id = ((uint64_t)(ntohl(p[2])) << 32) | ntohl(p[3]);
    h->integer_secs = ntohl(p[4]);
    h->fractional_secs = 0;
    h->trailer = ntohl(p[len-1]);
    break;

  case 24:
    h->stream_id = 0;
    h->class_id = 0;
    h->integer_secs = 0;
    h->fractional_secs = ((uint64_t)(ntohl(p[1])) << 32) | ntohl(p[2]);
    h->trailer = ntohl(p[len-1]);
    break;

  case 25:
    h->stream_id = ntohl(p[1]);
    h->class_id = 0;
    h->integer_secs = 0;
    h->fractional_secs = ((uint64_t)(ntohl(p[2])) << 32) | ntohl(p[3]);
    h->trailer = ntohl(p[len-1]);
    break;

  case 26:
    h->stream_id = 0;
    h->class_id = ((uint64_t)(ntohl(p[1])) << 32) | ntohl(p[2]);
    h->integer_secs = 0;
    h->fractional_secs = ((uint64_t)(ntohl(p[3])) << 32) | ntohl(p[4]);
    h->trailer = ntohl(p[len-1]);
    break;

  case 27:
    h->stream_id = ntohl(p[1]);
    h->class_id = ((uint64_t)(ntohl(p[2])) << 32) | ntohl(p[3]);
    h->integer_secs = 0;
    h->fractional_secs = ((uint64_t)(ntohl(p[4])) << 32) | ntohl(p[5]);
    h->trailer = ntohl(p[len-1]);
    break;

  case 28:
    h->stream_id = 0;
    h->class_id = 0;
    h->integer_secs = ntohl(p[1]);
    h->fractional_secs = ((uint64_t)(ntohl(p[2])) << 32) | ntohl(p[3]);
    h->trailer = ntohl(p[len-1]);
    break;

  case 29:
    h->stream_id = ntohl(p[1]);
    h->class_id = 0;
    h->integer_secs = ntohl(p[2]);
    h->fractional_secs = ((uint64_t)(ntohl(p[3])) << 32) | ntohl(p[4]);
    h->trailer = ntohl(p[len-1]);
    break;

  case 30:
    h->stream_id = 0;
    h->class_id = ((uint64_t)(ntohl(p[1])) << 32) | ntohl(p[2]);
    h->integer_secs = ntohl(p[3]);
    h->fractional_secs = ((uint64_t)(ntohl(p[4])) << 32) | ntohl(p[5]);
    h->trailer = ntohl(p[len-1]);
    break;

  case 31:
    h->stream_id = ntohl(p[1]);
    h->class_id = ((uint64_t)(ntohl(p[2])) << 32) | ntohl(p[3]);
    h->integer_secs = ntohl(p[4]);
    h->fractional_secs = ((uint64_t)(ntohl(p[5])) << 32) | ntohl(p[6]);
    h->trailer = ntohl(p[len-1]);
    break;

