/* -*- c++ -*- */
/*
 * Copyright 2005,2010 Free Software Foundation, Inc.
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

#include <gr_align_on_samplenumbers_ss.h>
#include <gr_io_signature.h>
#include <assert.h>
#include <stdexcept>
#include <string.h>
#include <cstdio>

//define ALIGN_ADVANCED_IMPLEMENTATION to have an alternative implementation of the align algoritm which exactly follows the align_interval spec.
//It is more resource intensive, less tested and probably not needed
//define ALIGN_ADVANCED_IMPLEMENTATION

//define DEBUG_TOCONSUME to see debug messages about the synchronisation part of this block
//define DEBUG_TOCONSUME
#ifdef DEBUG_TOCONSUME
#define tcPrintf if(dprint) printf
#else
#define tcPrintf //printf
#endif

#define ePrintf printf

gr_align_on_samplenumbers_ss_sptr
gr_make_align_on_samplenumbers_ss (int nchan, int align_interval)
{
  return gnuradio::get_initial_sptr(new gr_align_on_samplenumbers_ss (nchan,align_interval));
}

gr_align_on_samplenumbers_ss::gr_align_on_samplenumbers_ss (int nchan,int align_interval)
  : gr_block ("align_on_samplenumbers_ss",
        gr_make_io_signature (2, -1, sizeof (short)), //2, -1
        gr_make_io_signature (2, -1, sizeof (short))), //2,-1
  d_align_interval (align_interval),
  d_nchan(nchan),
  d_ninputs(0)
{
  if (d_align_interval<0)
    set_output_multiple (d_nchan*2);
  else
  {
    set_output_multiple (d_align_interval*d_nchan*2);
  }
    
}

gr_align_on_samplenumbers_ss::~gr_align_on_samplenumbers_ss()
{
  
}
void
gr_align_on_samplenumbers_ss::forecast (int noutput_items, gr_vector_int &ninput_items_required)
{
  //assert (0 == noutput_items % d_align_interval);
  unsigned ninputs = ninput_items_required.size();
  for (unsigned int i = 0; i < ninputs; i++)
    ninput_items_required[i] = std::max(noutput_items*d_nchan*2+ history() - 1,1024*d_nchan*2+ history() - 1);//TODO include the diffs found in determine input_items_required
}

bool
gr_align_on_samplenumbers_ss::check_topology (int ninputs, int noutputs)
{
  bool result=true;
  if(noutputs!=ninputs)
  {
    result=false;
    ePrintf("gr_align_on_samplenumbers_ss: ERROR noutputs %i != ninputs %i\n",noutputs,ninputs);
  }
  if(d_nchan<2)
  {
    result=false;
    ePrintf("gr_align_on_samplenumbers_ss: ERROR nchan %i<2 \n",d_nchan);
  }
  if((int)d_ninputs!=ninputs)
  {
    //Only resize and reset the status if there really changed something
    //Don't reset the status if the user just called stop() and start(), although maybe we should.
    d_state.resize(ninputs);
    d_ninputs=ninputs;
    for(unsigned int i=0;i<d_ninputs;i++)
    {
      d_state[i].sync_found=false;
      d_state[i].sync_end_found=false;
    }
    d_in_presync=false;
  }
  return result;
}

#ifdef ALIGN_ADVANCED_IMPLEMENTATION
int
gr_align_on_samplenumbers_ss::general_work (int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
#ifdef DEBUG_TOCONSUME
  static int dcount=0;
  bool dprint=false;
  dcount++;
  if(dcount>200)
  {
    dcount=0;
    dprint=true;
  } 
#endif
  const size_t item_size = output_signature()->sizeof_stream_item (0);
  const unsigned ninputs = input_items.size();
  const unsigned noutputs = output_items.size();

  int align_interval=d_align_interval*2*d_nchan;
  if(d_align_interval<0)
  {
    //align once per noutput_items
    align_interval=noutput_items;
    align_interval=align_interval/(2*d_nchan);
    align_interval=align_interval*(2*d_nchan);
  }

  int min_ninput_items=noutput_items;//numeric_limits<int>::max();
  int noutput_items_produced=0;
  for(unsigned int i=0;i<ninputs;i++)
  { 
    d_state[i].ninput_items=ninput_items[i];
    d_state[i].ninput_items_used=0;
    min_ninput_items=std::min(ninput_items[i],min_ninput_items);
  }
  for(int j=0;j<noutput_items-align_interval+1;j+=align_interval)
  {
    int common_end;
    if(min_ninput_items>=align_interval)
      common_end=align_interval;
    else
    {
      common_end=min_ninput_items/(d_nchan*2);
      common_end=common_end*(d_nchan*2);
    }
    if (common_end<=0) break;
    
    bool all_diffs_zero=true;
    //bool sync_found=false;
    int diff_comp_end_max=0;
    for(unsigned int i=0;i<ninputs;i++)
    {
      unsigned short * uin=&(((unsigned short*)input_items[i])[d_state[i].ninput_items_used]);
      unsigned int  x_high16bits = uin[0];
      unsigned int  x_low16bits = uin[1];
      d_state[i].ucounter_begin = x_high16bits<<16 | x_low16bits;
      d_state[i].diff=d_state[0].ucounter_begin-d_state[i].ucounter_begin;//Result is a signed value,Will wrap around on 32 bit boundary
      int common_last=std::max(common_end-d_nchan*2,0);
      x_high16bits = uin[d_nchan*2];
      x_low16bits = uin[d_nchan*2+1];
      unsigned int ucounter_begin2 = x_high16bits<<16 | x_low16bits;
#ifdef DEBUG_TOCONSUME
      if((d_state[i].ucounter_begin+1)!=(ucounter_begin2))
        if(ucounter_begin2==0)
          ePrintf("SYNC counters are 0\n");
        else
           ePrintf("Error: counter not continuous.\n ucounter_begin[%i]=%i +1 !=  ucounter_begin2=%i\n",i,d_state[i].ucounter_begin,ucounter_begin2);
#endif
      x_high16bits = uin[common_last];
      x_low16bits = uin[common_last+1];
      d_state[i].ucounter_end = x_high16bits<<16 | x_low16bits;
      d_state[i].diff_end=d_state[0].ucounter_end-d_state[i].ucounter_end;//Result is a signed value,Will wrap around on 32 bit boundary
      d_state[i].diff_comp_end=d_state[i].ucounter_end-d_state[0].ucounter_end;
      diff_comp_end_max=std::max(d_state[i].diff_comp_end,diff_comp_end_max);
#ifdef DEBUG_TOCONSUME
      if(d_state[i].diff>256000000 || d_state[i].diff_end>256000000 || d_state[i].diff_comp_end>256000000)
      {
        tcPrintf("diff[%i]=%i diff_end=%i diff_comp_end=%i\n",i,d_state[i].diff,d_state[i].diff_end,d_state[i].diff_comp_end);
      }
#endif
      all_diffs_zero=all_diffs_zero && (0==d_state[i].diff_end);
      if(d_state[i].ucounter_end<d_state[i].ucounter_begin+(unsigned)(common_last/(d_nchan*2))) //(unsigned)(common_last/(d_nchan*2)))
      {
        //printf("sync 1 ");// found ucounter_end[%i]=%i ucounter_begin[%i]=%i \n",i,d_state[i].ucounter_end,i,d_state[i].ucounter_begin);
        //sync_found=true;//sync_found or 32 bit counter  wraparound (0xffffffff -> 0x00000000)
        if(!d_in_presync)
        {
#ifdef DEBUG_TOCONSUME
          printf("presync START with %i\n",i);
#endif
         for(unsigned int k=0;k<ninputs;k++)
         {
          d_state[k].sync_found=false;
          d_state[i].sync_end_found=false;
         }
         d_in_presync=true;
         d_state[i].sync_found=true;    
        } else
        {
          //d_in_presync=true;
#ifdef DEBUG_TOCONSUME
          if(d_state[i].sync_found)
            printf("presync CONTINUE with %i\n",i);
          else
            printf("presync NEXT with %i\n",i);
#endif
          d_state[i].sync_found=true;  
          d_state[i].sync_end_found=false;  
        }             
      } else
      {
        if(d_in_presync && d_state[i].sync_found)
        {
          d_state[i].sync_end_found=true;
          bool all_syncs_found=true;
          for(unsigned int k=0;k<ninputs;k++)
            all_syncs_found=all_syncs_found && d_state[k].sync_end_found;
          d_in_presync=!all_syncs_found;
          if(!d_in_presync)
          {
            for(unsigned int k=0;k<ninputs;k++)
            {
              d_state[k].sync_found=false;
              d_state[i].sync_end_found=false;
            }
#ifdef DEBUG_TOCONSUME
            printf("presync END\n");
#endif
          }
        }
      }
    }
    if(d_in_presync || all_diffs_zero)
    {
      for(unsigned int i=0;i<ninputs;i++)
      {
         memcpy(&(((unsigned short*)output_items[i])[j]),&(((const unsigned short*)input_items[i])[d_state[i].ninput_items_used]),common_end*item_size);
         //consume(i,common_end);
         d_state[i].ninput_items-=common_end;
         d_state[i].ninput_items_used+=common_end;
         min_ninput_items=std::min(d_state[i].ninput_items,min_ninput_items);
#ifdef DEBUG_TOCONSUME
         if(common_end<256)
           tcPrintf("common_end %i\n",common_end);
#endif
      }
      //min_ninput_items-=common_end;
      noutput_items_produced+=common_end;
      //return common_end;
    } else
    {
      //printf("sync 2");
      for(unsigned int i=0;i<ninputs;i++)
      {
        int toconsume=std::min((d_state[i].diff_end+diff_comp_end_max)*d_nchan*2,d_state[i].ninput_items);
        toconsume=toconsume/(d_nchan*2);
        toconsume=toconsume*(d_nchan*2);
        d_state[i].ninput_items-=toconsume;
        d_state[i].ninput_items_used+=toconsume;
        min_ninput_items=std::min(d_state[i].ninput_items,min_ninput_items);
#ifdef DEBUG_TOCONSUME
      static int toconsume_counter=0;
      toconsume_counter++;
      //if(toconsume_counter>10)
      {
        tcPrintf("toconsume=%i diff_end[%i]*d_nchan*2=%i diff_comp_end_max*d_nchan*2=%i ninput_items[%i]=%i\n",toconsume,i,d_state[i].diff_end*d_nchan*2,diff_comp_end_max*d_nchan*2,i,ninput_items[i]);
        toconsume_counter=0;
      }
#endif
        //printf("toconsume[%i]=%i\n",i,toconsume);
        //consume(i,toconsume);//skip the difference in samplenumber items
      }
      //return 0;
    }
  }
  for(unsigned int i=0;i<ninputs;i++)
    consume(i,d_state[i].ninput_items_used);
#ifdef DEBUG_TOCONSUME
  if(noutput_items_produced<256)
    tcPrintf("noutput_items_produced %i\n",noutput_items_produced);
#endif
  return noutput_items_produced;
}


#else /*ALIGN_ADVANCED_IMPLEMENTATION*/
int
gr_align_on_samplenumbers_ss::general_work (int noutput_items,
        gr_vector_int &ninput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
{
#ifdef DEBUG_TOCONSUME
  static int dcount=0;
  bool dprint=false;
  dcount++;
  if(dcount>2000)
  {
    dcount=0;
    dprint=true;
  } 
#endif
  const size_t item_size = output_signature()->sizeof_stream_item (0);
  const unsigned ninputs = input_items.size();
  
  int common_end=noutput_items;
  //int diff_min=INT_MAX;
  //int diff_max=INT_MIN;
  for(unsigned int i=0;i<ninputs;i++)
  {
    unsigned short * uin=(unsigned short*)input_items[i];
    unsigned int  x_high16bits = uin[0];
    unsigned int  x_low16bits = uin[1];
    d_state[i].ucounter_begin = x_high16bits<<16 | x_low16bits;
    d_state[i].diff=d_state[0].ucounter_end-d_state[i].ucounter_end;//Result is a signed value,Will wrap around on 32 bit boundary
    x_high16bits = uin[d_nchan*2];
    x_low16bits = uin[d_nchan*2+1];
    unsigned int ucounter_begin2 = x_high16bits<<16 | x_low16bits;
    if((d_state[i].ucounter_begin+1)!=(ucounter_begin2)){
      if(ucounter_begin2==0)
      {
#ifdef DEBUG_TOCONSUME
        ePrintf("SYNC counters are 0\n");
#endif
      }
      else
      {
        ePrintf("Error: counter not continuous.\n ucounter_begin[%i]=%i +1 !=  ucounter_begin2=%i\n",i,d_state[i].ucounter_begin,ucounter_begin2);
      }
    }
      
    //diff_comp[i]=ucounter[i]-ucounter[0];
    //diff_min=std::min(diff[i],diff_min);
    //diff_max=std::max(diff[i],diff_max);
    common_end=std::max(std::min(ninput_items[i],common_end),0);
  }
  common_end=common_end/(d_nchan*2);
  common_end=common_end*(d_nchan*2);
#ifdef DEBUG_TOCONSUME
  if(common_end<d_nchan*2)
  {
    printf(" common_end %i\n",common_end);
    for(int j=0;j<ninputs;j++)
      printf("ninput_items[%i]=%i\n",j,ninput_items[j]);
  }
#endif
  bool all_diffs_zero=true;
  bool sync_found=false;
  int diff_comp_end_max=0;
  for(unsigned int i=0;i<ninputs;i++)
  {
    unsigned short * uin=(unsigned short*)input_items[i];
    int common_last=common_end-(d_nchan*2);
    unsigned int  x_high16bits = uin[common_last];
    unsigned int  x_low16bits = uin[common_last+1];
    d_state[i].ucounter_end = x_high16bits<<16 | x_low16bits;
    d_state[i].diff_end=d_state[0].ucounter_end-d_state[i].ucounter_end;//Result is a signed value,Will wrap around on 32 bit boundary
    d_state[i].diff_comp_end=d_state[i].ucounter_end-d_state[0].ucounter_end;
    //diff_end_min=std::min(diff_end[i],diff_end_min);
    //diff_end_max=std::max(diff_end[i],diff_end_max);
    diff_comp_end_max=std::max(d_state[i].diff_comp_end,diff_comp_end_max);
#ifdef DEBUG_TOCONSUME
    if(d_state[i].diff_end!=d_state[i].diff)
    {
      //samples_lost_or_syncstart=true;
      printf("Us%i %i %i ",i,d_state[i].diff_end,d_state[i].diff);
    }
#endif
    all_diffs_zero=all_diffs_zero && (0==d_state[i].diff_end);
    if((d_state[i].ucounter_end<d_state[i].ucounter_begin+(unsigned)(common_last/(d_nchan*2))) || (0==d_state[i].ucounter_end) || (0==d_state[i].ucounter_begin)) //(unsigned)(common_last/(d_nchan*2)))
    {
      sync_found=true;//sync_found or 32 bit counter  wraparound (0xffffffff -> 0x00000000)
#ifdef DEBUG_TOCONSUME
      tcPrintf("SYNC diff_end[%i]=%i ucounter_end[%i]=%i ucounter_begin[%i]=%i \n",i,d_state[i].diff_end,i,d_state[i].ucounter_end,i,d_state[i].ucounter_begin);
      tcPrintf("ucounter_end=%i < %i = ucounter_begin+(unsigned)(common_last/(d_nchan*2) \n",d_state[i].ucounter_end,d_state[i].ucounter_begin+(unsigned)(common_last/(d_nchan*2)));

      printf("ucounter_end[%i]=%i ucounter_begin[%i]=%i\n",i,d_state[i].ucounter_end,i,d_state[i].ucounter_begin);      
      int expected_sync_position=common_last - d_state[i].ucounter_end*(d_nchan*2);
      if(0==uin[expected_sync_position] && 0==uin[expected_sync_position+1])
      {
        printf("sync found on input %i at position %i \n",i,expected_sync_position);
        //sync_start[i]=expected_sync_position;
      } else
      {
        printf("sync found on input %i position unclear, expected at %i value there %i\n",i,expected_sync_position,uin[expected_sync_position]<<16 | uin[expected_sync_position+1]);
        //sync_start[i]=-1;
      }
    } else
    {
      tcPrintf("NOsync diff_end[%i]=%i ucounter_end[%i]=%i ucounter_begin[%i]=%i \n",i,d_state[i].diff_end,i,d_state[i].ucounter_end,i,d_state[i].ucounter_begin);
#endif    
    }
  }
  bool problem=false;
  for(unsigned int i=0;i<ninputs;i++)
    if((d_state[i].diff_end+diff_comp_end_max) >0x4000000)
      {
        problem=true;
        ePrintf("Warning: counter diff greater as 64 Million\n");
        ePrintf("         You might want to swap master and slave.\n");
        ePrintf("          i=%i,d_state[i].diff_end+diff_comp_end_max=%i,d_state[i].diff_end=%i,diff_comp_end_max=%i,ucounter[i]=%i,ucounter[0]=%i\n",
                          i,d_state[i].diff_end+diff_comp_end_max,d_state[i].diff_end,diff_comp_end_max,d_state[i].ucounter_end,d_state[0].ucounter_end);
        //ePrintf("        toconsume=%i\n",toconsume); 
      }
  if(sync_found || all_diffs_zero || problem)
  {
#ifdef DEBUG_TOCONSUME
    if(all_diffs_zero) tcPrintf("ZERO\n");
    if(sync_found) tcPrintf("SYNC\n");
#endif
    for(unsigned int i=0;i<ninputs;i++)
    {
       memcpy(output_items[i],input_items[i],common_end*item_size);
       consume(i,common_end);
#ifdef DEBUG_TOCONSUME
      if(common_end<256)
        tcPrintf("common_end %i\n",common_end);
#endif
    }
    return common_end;
  } else
  {
    //int minconsume=0;//common_end/(2*d_nchan*2);
    //min_consume=min_consume*d_nchan*2;  
    for(unsigned int i=0;i<ninputs;i++)
    {
      int toconsume=std::min((d_state[i].diff_end+diff_comp_end_max)*d_nchan*2,ninput_items[i]);
      toconsume=toconsume/(d_nchan*2);
      toconsume=toconsume*(d_nchan*2);
#ifdef DEBUG_TOCONSUME
      //printf("dcount %i\n",dcount);
      static int toconsume_counter=0;
      toconsume_counter++;
      //if(toconsume_counter>10)
      {
        tcPrintf("toconsume=%i diff_end[[%i]*d_nchan*2=%i diff_comp_end_max)*d_nchan*2=%i ninput_items[%i]=%i\n",
                  toconsume,i,d_state[i].diff_end*d_nchan*2,diff_comp_end_max*d_nchan*2,i,ninput_items[i]);
        toconsume_counter=0;
      }
#endif
      consume(i,toconsume);//skip the difference in samplenumber items
      //printf("toconsume%i %i diff_comp_end_max %i diff_end[[%i] %i\n",i,toconsume,diff_comp_end_max,i,d_state[i].diff_end);
    }
    return 0;
  }
  return -1;//Should never come here
}
#endif /*ALIGN_ADVANCED_IMPLEMENTATION*/
