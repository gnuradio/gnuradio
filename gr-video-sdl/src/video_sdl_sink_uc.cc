/* -*- c++ -*- */
/*
 * Copyright 2006,2010 Free Software Foundation, Inc.
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

#include <SDL.h>

#include <video_sdl_sink_uc.h>
#include <gr_io_signature.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdexcept>
#include <string.h>



video_sdl_sink_uc::video_sdl_sink_uc (double framerate,int width, int height,unsigned int format,int dst_width,int dst_height)
  : gr_sync_block ("video_sdl_sink_uc",
		   gr_make_io_signature (1, 3, sizeof (unsigned char)),
		   gr_make_io_signature (0, 0, 0)),
    d_chunk_size (width*height),
    d_framerate(framerate),
    d_wanted_frametime_ms(0),
    d_width(width),
    d_height (height),
    d_dst_width(dst_width),
    d_dst_height(dst_height),
    d_format(format), 
    d_current_line(0),
    d_screen(NULL),
    d_image(NULL),
    d_avg_delay(0.0),
    d_wanted_ticks(0)
{
  if(framerate<=0.0) 
    d_wanted_frametime_ms=0;//Go as fast as possible
  else
    d_wanted_frametime_ms=(int)(1000.0/framerate);
  if(dst_width<0) d_dst_width=d_width;
  if(dst_height<0) d_dst_height=d_height;
  if(0==format) d_format=IMGFMT_YV12;
  
  atexit(SDL_Quit);//check if this is the way to do this
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
    std::cerr << "video_sdl_sink_uc: Couldn't initialize SDL:" << SDL_GetError() << " \n SDL_Init(SDL_INIT_VIDEO) failed\n";
    throw std::runtime_error ("video_sdl_sink_uc");
  };

  /* accept any depth */
  d_screen = SDL_SetVideoMode(dst_width, dst_height, 0, SDL_SWSURFACE|SDL_RESIZABLE|SDL_ANYFORMAT);//SDL_DOUBLEBUF |SDL_SWSURFACE| SDL_HWSURFACE||SDL_FULLSCREEN
  if ( d_screen == NULL ) {
    std::cerr << "Unable to set SDL video mode: " << SDL_GetError() <<"\n SDL_SetVideoMode() Failed \n";
    exit(1);
  }
  if ( d_image ) {
      SDL_FreeYUVOverlay(d_image);
    }
  /* Initialize and create the YUV Overlay used for video out */
  if (!(d_image = SDL_CreateYUVOverlay (d_width, d_height, SDL_YV12_OVERLAY, d_screen))) {
    std::cerr << "SDL: Couldn't create a YUV overlay: \n"<< SDL_GetError() <<"\n";
    throw std::runtime_error ("video_sdl_sink_uc");
  }

  printf("SDL screen_mode %d bits-per-pixel\n",
  	 d_screen->format->BitsPerPixel);
  printf("SDL overlay_mode %i \n",
  	 d_image->format);
  d_chunk_size = std::min(1,16384/width); //width*16;
  d_chunk_size = d_chunk_size*width;
  //d_chunk_size = (int) (width);
  set_output_multiple (d_chunk_size);
  /* Set the default playback area */
  d_dst_rect.x = 0;
  d_dst_rect.y = 0;
  d_dst_rect.w = d_dst_width;
  d_dst_rect.h = d_dst_height;
  //clear the surface to grey
  if ( SDL_LockYUVOverlay( d_image ) ) {
    std::cerr << "SDL: Couldn't lock YUV overlay: \n"<< SDL_GetError() <<"\n";
    throw std::runtime_error ("video_sdl_sink_uc");
  }
  memset(d_image->pixels[0], 128, d_image->pitches[0]*d_height);  
  memset(d_image->pixels[1], 128, d_image->pitches[1]*d_height/2);  
  memset(d_image->pixels[2], 128, d_image->pitches[2]*d_height/2);  
  SDL_UnlockYUVOverlay( d_image );
}

video_sdl_sink_uc::~video_sdl_sink_uc ()
{
  SDL_Quit();
}

video_sdl_sink_uc_sptr
video_sdl_make_sink_uc (double framerate,int width, int height,unsigned int format,int dst_width,int dst_height)
{
  return gnuradio::get_initial_sptr(new video_sdl_sink_uc (framerate, width, height,format,dst_width,dst_height));
}

void
video_sdl_sink_uc::copy_line_pixel_interleaved(unsigned char *dst_pixels_u,unsigned char *dst_pixels_v,const unsigned char * src_pixels,int src_width)
{
  for(int i=0;i<src_width;i++)
  {
    dst_pixels_u[i]=src_pixels[i*2];
    dst_pixels_v[i]=src_pixels[i*2+1];
  }
  return;
}

void
video_sdl_sink_uc::copy_line_line_interleaved(unsigned char *dst_pixels_u,unsigned char *dst_pixels_v,const unsigned char * src_pixels,int src_width)
{
  memcpy(dst_pixels_u, src_pixels, src_width); 
  memcpy(dst_pixels_v, src_pixels+src_width, src_width); 
  return;
}

void
video_sdl_sink_uc::copy_line_single_plane(unsigned char *dst_pixels,const unsigned char * src_pixels,int src_width)
{
  memcpy(dst_pixels, src_pixels, src_width); 
  return;
}

void
video_sdl_sink_uc::copy_line_single_plane_dec2(unsigned char *dst_pixels,const unsigned char * src_pixels,int src_width)
{
  for(int i=0,j=0;i<src_width;i+=2,j++)
  {
    dst_pixels[j]=(unsigned char)src_pixels[i];
  }
  return;
}

int
video_sdl_sink_uc::copy_plane_to_surface (int plane,int noutput_items,
		      const unsigned char * src_pixels)
{
    const int first_dst_plane=(12==plane ||1122==plane)?1:plane;
    const int second_dst_plane=(12==plane ||1122==plane)?2:plane;
    int current_line=(0==plane)?d_current_line:d_current_line/2;
    unsigned char * dst_pixels = (unsigned char *)d_image->pixels[first_dst_plane];
    dst_pixels=&dst_pixels[current_line*d_image->pitches[first_dst_plane]];
    unsigned char * dst_pixels_2 = (unsigned char *)d_image->pixels[second_dst_plane];
    dst_pixels_2=&dst_pixels_2[current_line*d_image->pitches[second_dst_plane]];
    int src_width=(0==plane || 12==plane || 1122==plane)?d_width:d_width/2;
    int noutput_items_produced=0;
    int max_height=(0==plane)?d_height-1:d_height/2-1;
    for (int i = 0; i < noutput_items; i += src_width){
        //output one line at a time
        if(12==plane)
        {
          copy_line_pixel_interleaved(dst_pixels,dst_pixels_2,src_pixels,src_width);
          dst_pixels_2 += d_image->pitches[second_dst_plane];
        }
        else if (1122==plane)
          {
            copy_line_line_interleaved(dst_pixels,dst_pixels_2,src_pixels,src_width);
            dst_pixels_2 += d_image->pitches[second_dst_plane];
            src_pixels += src_width;
          }
        else if (0==plane)
          copy_line_single_plane(dst_pixels,src_pixels,src_width);
        else /* 1==plane || 2==plane*/
          copy_line_single_plane_dec2(dst_pixels,src_pixels,src_width);//decimate by two horizontally
        src_pixels += src_width;
        dst_pixels += d_image->pitches[first_dst_plane];
        noutput_items_produced+=src_width;
        current_line++;
        if (current_line>max_height)
        {
          //Start new frame
          //TODO, do this all in a seperate thread
          current_line=0;
          dst_pixels=d_image->pixels[first_dst_plane];
          dst_pixels_2=d_image->pixels[second_dst_plane];
          if(0==plane)
          {
            SDL_DisplayYUVOverlay(d_image, &d_dst_rect);
            //SDL_Flip(d_screen);
            unsigned int ticks=SDL_GetTicks();//milliseconds
            d_wanted_ticks+=d_wanted_frametime_ms;
            float avg_alpha=0.1;
            int time_diff=d_wanted_ticks-ticks;
            d_avg_delay=time_diff*avg_alpha +d_avg_delay*(1.0-avg_alpha);
          }
        }
      }
    if(0==plane) d_current_line=current_line;
  return noutput_items_produced;
}

int
video_sdl_sink_uc::work (int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
{
  unsigned char *src_pixels_0,*src_pixels_1,*src_pixels_2;
  int noutput_items_produced=0;
  int plane;
  int delay=(int)d_avg_delay;
  if(0==d_wanted_ticks)
    d_wanted_ticks=SDL_GetTicks();
  if(delay>0)
    SDL_Delay((unsigned int)delay);//compensate if running too fast

  if ( SDL_LockYUVOverlay( d_image ) ) {
    return 0;
  } 
  switch (input_items.size ()){
  case 3:		// first channel=Y, second channel is  U , third channel is V
    src_pixels_0 = (unsigned char *) input_items[0];
    src_pixels_1 = (unsigned char *) input_items[1];
    src_pixels_2 = (unsigned char *) input_items[2];
    for (int i = 0; i < noutput_items; i += d_chunk_size){
      copy_plane_to_surface (1,d_chunk_size, src_pixels_1);
      copy_plane_to_surface (2,d_chunk_size, src_pixels_2);
      noutput_items_produced+=copy_plane_to_surface (0,d_chunk_size, src_pixels_0);
      src_pixels_0 += d_chunk_size;
      src_pixels_1 += d_chunk_size;
      src_pixels_2 += d_chunk_size;      
    }
    break;
  case 2:
    if(1) //if(pixel_interleaved_uv)
    {
      // first channel=Y, second channel is alternating pixels U and V
      src_pixels_0 = (unsigned char *) input_items[0];
      src_pixels_1 = (unsigned char *) input_items[1];
      for (int i = 0; i < noutput_items; i += d_chunk_size){
        copy_plane_to_surface (12,d_chunk_size/2, src_pixels_1);
        noutput_items_produced+=copy_plane_to_surface (0,d_chunk_size, src_pixels_0);
        src_pixels_0 += d_chunk_size;
        src_pixels_1 += d_chunk_size;     
      }
    } else
    {
      // first channel=Y, second channel is alternating lines U and V
      src_pixels_0 = (unsigned char *) input_items[0];
      src_pixels_1 = (unsigned char *) input_items[1];
      for (int i = 0; i < noutput_items; i += d_chunk_size){
        copy_plane_to_surface (1222,d_chunk_size/2, src_pixels_1);
        noutput_items_produced+=copy_plane_to_surface (0,d_chunk_size, src_pixels_0);
        src_pixels_0 += d_chunk_size;
        src_pixels_1 += d_chunk_size;     
      }
    }
    break;
  case 1:		// grey (Y) input
    /* Y component */
    plane=0;
    src_pixels_0 = (unsigned char *) input_items[plane];
    for (int i = 0; i < noutput_items; i += d_chunk_size){
      noutput_items_produced+=copy_plane_to_surface (plane,d_chunk_size, src_pixels_0);
      src_pixels_0 += d_chunk_size;
    }
    break;
  default: //0 or more then 3 channels
     std::cerr << "video_sdl_sink_uc: Wrong number of channels: ";
     std::cerr <<"1, 2 or 3 channels are supported.\n  Requested number of channels is "<< input_items.size () <<"\n";
    throw std::runtime_error ("video_sdl_sink_uc");
  }

  SDL_UnlockYUVOverlay( d_image );
  return noutput_items_produced;
}
