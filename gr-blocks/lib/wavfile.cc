/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/blocks/wavfile.h>
#include <cstring>
#include <stdint.h>
#include <boost/detail/endian.hpp> //BOOST_BIG_ENDIAN

namespace gr {
  namespace blocks {
#define VALID_COMPRESSION_TYPE 0x0001

    // Basically, this is the opposite of htonx() and ntohx()
    // Define host to/from worknet (little endian) short and long
#ifdef BOOST_BIG_ENDIAN

    static inline uint16_t __gri_wav_bs16(uint16_t x)
    {
      return (x>>8) | (x<<8);
    }

    static inline uint32_t __gri_wav_bs32(uint32_t x)
    {
      return (uint32_t(__gri_wav_bs16(uint16_t(x&0xfffful)))<<16) | (__gri_wav_bs16(uint16_t(x>>16)));
    }

    #define htowl(x) __gri_wav_bs32(x)
    #define wtohl(x) __gri_wav_bs32(x)
    #define htows(x) __gri_wav_bs16(x)
    #define wtohs(x) __gri_wav_bs16(x)

#else

    #define htowl(x) uint32_t(x)
    #define wtohl(x) uint32_t(x)
    #define htows(x) uint16_t(x)
    #define wtohs(x) uint16_t(x)

#endif // BOOST_BIG_ENDIAN

    // WAV files are always little-endian, so we need some byte switching macros
    static inline uint32_t host_to_wav(uint32_t x) { return htowl(x); }
    static inline uint16_t host_to_wav(uint16_t x) { return htows(x); }
    static inline int16_t  host_to_wav(int16_t x)  { return htows(x); }
    static inline uint32_t wav_to_host(uint32_t x) { return wtohl(x); }
    static inline uint16_t wav_to_host(uint16_t x) { return wtohs(x); }
    static inline int16_t  wav_to_host(int16_t x)  { return wtohs(x); }

    bool
    wavheader_parse(FILE *fp,
		    unsigned int &sample_rate_o,
		    int &nchans_o,
		    int &bytes_per_sample_o,
		    int &first_sample_pos_o,
		    unsigned int &samples_per_chan_o)
    {
      // _o variables take return values
      char str_buf[8] = {0};

      uint32_t file_size;
      uint32_t fmt_hdr_skip;
      uint16_t compression_type;
      uint16_t nchans;
      uint32_t sample_rate;
      uint32_t avg_bytes_per_sec;
      uint16_t block_align;
      uint16_t bits_per_sample;
      uint32_t chunk_size;

      size_t fresult;

      fresult = fread(str_buf, 1, 4, fp);
      if(fresult != 4 || strncmp(str_buf, "RIFF", 4) || feof(fp)) {
	return false;
      }

      fresult = fread(&file_size, 1, 4, fp);

      fresult = fread(str_buf, 1, 8, fp);
      if(fresult != 8 || strncmp(str_buf, "WAVEfmt ", 8) || feof(fp)) {
	return false;
      }

      fresult = fread(&fmt_hdr_skip, 1, 4, fp);

      fresult = fread(&compression_type, 1, 2, fp);
      if(wav_to_host(compression_type) != VALID_COMPRESSION_TYPE) {
	return false;
      }

      fresult = fread(&nchans,            1, 2, fp);
      fresult = fread(&sample_rate,       1, 4, fp);
      fresult = fread(&avg_bytes_per_sec, 1, 4, fp);
      fresult = fread(&block_align,       1, 2, fp);
      fresult = fread(&bits_per_sample,   1, 2, fp);

      if(ferror(fp)) {
	return false;
      }

      fmt_hdr_skip    = wav_to_host(fmt_hdr_skip);
      nchans          = wav_to_host(nchans);
      sample_rate     = wav_to_host(sample_rate);
      bits_per_sample = wav_to_host(bits_per_sample);

      if(bits_per_sample != 8 && bits_per_sample != 16) {
	return false;
      }

      fmt_hdr_skip -= 16;
      if(fmt_hdr_skip) {
	if (fseek(fp, fmt_hdr_skip, SEEK_CUR) != 0) {
	  return false;
	}
      }

      // find data chunk
      fresult = fread(str_buf, 1, 4, fp);
      // keep parsing chunk until we hit the data chunk
      while(fresult != 4 || strncmp(str_buf, "data", 4))
      {
	// all good?
	if(fresult != 4 || ferror(fp) || feof(fp)) {
	  return false;
	}
	// get chunk body size and skip
	fresult = fread(&chunk_size, 1, 4, fp);
	if(fresult != 4 || ferror(fp) || feof(fp)) {
	  return false;
        }
	chunk_size = wav_to_host(chunk_size);
	if(fseek(fp, chunk_size, SEEK_CUR) != 0) {
	  return false;
	}
        // read next chunk type
        fresult = fread(str_buf, 1, 4, fp);
      }

      fresult = fread(&chunk_size, 1, 4, fp);
      if(ferror(fp)) {
	return false;
      }

      // More byte swapping
      chunk_size = wav_to_host(chunk_size);

      // Output values
      sample_rate_o      = (unsigned)sample_rate;
      nchans_o           = (int)nchans;
      bytes_per_sample_o = (int)(bits_per_sample / 8);
      first_sample_pos_o = (int)ftell(fp);
      samples_per_chan_o = (unsigned)(chunk_size / (bytes_per_sample_o * nchans));
      return true;
    }


    short int
    wav_read_sample(FILE *fp, int bytes_per_sample)
    {
      int16_t buf_16bit;

      if(fread(&buf_16bit, bytes_per_sample, 1, fp) != 1) {
	return 0;
      }
      if(bytes_per_sample == 1) {
	return (short)buf_16bit;
      }
      return (short)wav_to_host(buf_16bit);
    }


    bool
    wavheader_write(FILE *fp,
		    unsigned int sample_rate,
		    int nchans,
		    int bytes_per_sample)
    {
      const int header_len = 44;
      char wav_hdr[header_len] = "RIFF\0\0\0\0WAVEfmt \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0data\0\0\0";
      uint16_t nchans_f        = (uint16_t) nchans;
      uint32_t sample_rate_f   = (uint32_t) sample_rate;
      uint16_t block_align     = bytes_per_sample * nchans;
      uint32_t avg_bytes       = sample_rate * block_align;
      uint16_t bits_per_sample = bytes_per_sample * 8;

      nchans_f        = host_to_wav(nchans_f);
      sample_rate_f   = host_to_wav(sample_rate_f);
      block_align     = host_to_wav(block_align);
      avg_bytes       = host_to_wav(avg_bytes);
      bits_per_sample = host_to_wav(bits_per_sample);

      wav_hdr[16] = 0x10; // no extra bytes
      wav_hdr[20] = 0x01; // no compression
      memcpy((void*)(wav_hdr + 22), (void*)&nchans_f,        2);
      memcpy((void*)(wav_hdr + 24), (void*)&sample_rate_f,   4);
      memcpy((void*)(wav_hdr + 28), (void*)&avg_bytes,       4);
      memcpy((void*)(wav_hdr + 32), (void*)&block_align,     2);
      memcpy((void*)(wav_hdr + 34), (void*)&bits_per_sample, 2);

      fwrite(&wav_hdr, 1, header_len, fp);
      if(ferror(fp)) {
	return false;
      }

      return true;
    }


    void
    wav_write_sample(FILE *fp, short int sample, int bytes_per_sample)
    {
      void *data_ptr;
      unsigned char buf_8bit;
      int16_t       buf_16bit;

      if(bytes_per_sample == 1) {
	buf_8bit = (unsigned char)sample;
	data_ptr = (void*)&buf_8bit;
      }
      else {
	buf_16bit = host_to_wav((int16_t) sample);
	data_ptr  = (void *) &buf_16bit;
      }

      fwrite(data_ptr, 1, bytes_per_sample, fp);
    }


    bool
    wavheader_complete(FILE *fp, unsigned int byte_count)
    {
      uint32_t chunk_size = (uint32_t)byte_count;
      chunk_size = host_to_wav(chunk_size);

      if (fseek(fp, 40, SEEK_SET) != 0) {
	return false;
      }
      fwrite(&chunk_size, 1, 4, fp);

      chunk_size = (uint32_t)byte_count + 36; // fmt chunk and data header
      chunk_size = host_to_wav(chunk_size);
      if (fseek(fp, 4, SEEK_SET) != 0) {
	return false;
      }

      fwrite(&chunk_size, 1, 4, fp);

      if(ferror(fp)) {
	return false;
      }

      return true;
    }

  } /* namespace blocks */
} /* namespace gr */
