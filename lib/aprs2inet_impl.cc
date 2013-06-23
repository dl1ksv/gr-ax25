/* -*- c++ -*- */
/* 
 * Copyright 2013 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "aprs2inet_impl.h"

#define BAUD       1200

namespace gr {
  namespace afsk {

    aprs2inet::sptr
    aprs2inet::make(int samp_rate,int debug_level)
    {
      return gnuradio::get_initial_sptr
        (new aprs2inet_impl(samp_rate, debug_level));
    }

    /*
     * The private constructor
     */
    aprs2inet_impl::aprs2inet_impl(int sample_rate,int debug_level)
      : gr::block("aprs2inet",
                  gr::io_signature::make(1,1, sizeof (float)),
                  gr::io_signature::make(1, 1, sizeof(char))) {
      verbose_level=debug_level;
      d_sample_rate = sample_rate;
      d_sphaseinc   =  (0x10000u*BAUD/d_sample_rate);
      afsk12.dcd_shreg=0;
      afsk12.lasts    =0;
      afsk12.sphase   =0;
      memset(&hdlc, 0, sizeof(hdlc));
    }

    /*
     * Our virtual destructor.
     */
    aprs2inet_impl::~aprs2inet_impl()
    {
    }

  //  void
  //
  //  aprs2inet_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
  //  {
  //      /* <+forecast+> e.g. ninput_items_required[0] = noutput_items */
  //  }

    int
    aprs2inet_impl::general_work(int noutput_items,
                                gr_vector_int &ninput_items,
                                gr_vector_const_void_star &input_items,
                                gr_vector_void_star &output_items)   {
      int i;
      unsigned char curbit;
      const float *bit = (const float *) input_items[0];
      out = (char *) output_items[0];

      d_numchars=0;
      for (i=0;i <noutput_items; i++) {
        afsk12.dcd_shreg <<= 1;
        afsk12.dcd_shreg |= (bit[i] > 0);
//        verbprintf(10, "%c", '0'+(afsk12.dcd_shreg & 1));
      /*
       * check if transition
       */
      if ((afsk12.dcd_shreg ^ (afsk12.dcd_shreg >> 1)) & 1) {
        if (afsk12.sphase < (0x8000u-(d_sphaseinc/2)))
            afsk12.sphase += d_sphaseinc/8;
        else
            afsk12.sphase -= d_sphaseinc/8;
      }
      afsk12.sphase += d_sphaseinc;
      if (afsk12.sphase >= 0x10000u) {
        afsk12.sphase &= 0xffffu;
        afsk12.lasts <<= 1;
        afsk12.lasts |= afsk12.dcd_shreg & 1;
        curbit = (afsk12.lasts ^ (afsk12.lasts >> 1) ^ 1) & 1;
         hdlc_rxbit( curbit);
      }
    }
    // Tell runtime system how many output items we consumed.
    consume_each (noutput_items);
    return d_numchars;
    }
    void
    aprs2inet_impl::hdlc_rxbit(int bit)
    {
      int i,pos;
      int len;
      unsigned char c;
      hdlc.rxbitstream <<= 1;
      hdlc.rxbitstream |= !!bit;
      len=hdlc.rxptr - hdlc.rxbuf;
      if ((hdlc.rxbitstream & 0xff) == 0x7e) {
          if (hdlc.rxstate && len > 15) {
              crc_ccitt1.reset();
              crc_ccitt1.process_bytes(hdlc.rxbuf,  len);
              if (crc_ccitt1.checksum() == 0xf0b8) // No Crc Error
               {
                    len -=2;
                  for(i=7; i< 13;i++) { // Source address
                      if ((hdlc.rxbuf[i] &0xfe) != 0x40)
                        out[d_numchars++]=hdlc.rxbuf[i]>>1;
                    }
                  out[d_numchars++]='-';
                  c=(hdlc.rxbuf[13]>>1 ) & 0x0f;
                  if (c < 10)
                    c += 48;
                  else
                    c += 54;
                  out[d_numchars++]=c;
                  out[d_numchars++]='>';

                  for(i = 0; i < 6; i++) { // Destination
                      if ((hdlc.rxbuf[i] &0xfe) != 0x40)
                        out[d_numchars++]=hdlc.rxbuf[i]>>1;
                    }
                  c=(hdlc.rxbuf[6]>>1 ) & 0x0f;
                  if ( c > 0)
                    {
                      out[d_numchars++]='-';
                      if (c < 10)
                        c += 48;
                      else
                        c += 54;
                      out[d_numchars++]=c;
                      out[d_numchars++]=':';
                    }
                  else
                  {

                    pos=14; // via
                    while ( (pos +7) <= len )
                    {
                        if( (hdlc.rxbuf[pos]  & 1))
                          {
                            out[d_numchars++]=':';
                            break;
                          }
                        out[d_numchars++]=',';
                        for(i=pos;i < pos+6;i++)
                          {
                            if ((hdlc.rxbuf[i] &0xfe) != 0x40)
                              out[d_numchars++]= hdlc.rxbuf[i] >> 1;
                          }
                        c=(hdlc.rxbuf[pos+6]>>1 ) & 0x0f;
                        if (c < 10)
                          c += 48;
                        else
                          c += 54;
                        out[d_numchars++]='-';
                        out[d_numchars++]=c;
                        pos +=7;
                    }
                   }
                  pos +=2;
                  for(i=pos;i < len;i++)
                    out[d_numchars++]= hdlc.rxbuf[i];
                  out[d_numchars++]='\n';
                }
             }
          hdlc.rxstate = 1;
          hdlc.rxptr = hdlc.rxbuf;
          hdlc.rxbitbuf = 0x80;
          return;
      }
      if ((hdlc.rxbitstream & 0x7f) == 0x7f) {
          hdlc.rxstate = 0;
          return;
      }
      if (!hdlc.rxstate)
          return;
      if ((hdlc.rxbitstream & 0x3f) == 0x3e) /* stuffed bit */
          return;
      if (hdlc.rxbitstream & 1)
          hdlc.rxbitbuf |= 0x100;
      if (hdlc.rxbitbuf & 1) {
          if (hdlc.rxptr >= hdlc.rxbuf+sizeof(hdlc.rxbuf)) {
              hdlc.rxstate = 0;
//              verbprintf(1, "Error: packet size too large\n");
              return;
          }
          *hdlc.rxptr++ = hdlc.rxbitbuf >> 1;
          hdlc.rxbitbuf = 0x80;
          return;
      }
      hdlc.rxbitbuf >>= 1;
  }
  } /* namespace afsk */
} /* namespace gr */

