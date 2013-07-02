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
#include "afsk1200_impl.h"
#include <math.h>
#include <stdarg.h>
#include <stdio.h>

#define FREQ_MARK  1200
#define FREQ_SPACE 2200
#define BAUD       1200
#define SUBSAMP    2



namespace gr {
namespace afsk {
afsk1200::sptr
afsk1200::make(int sample_rate,int debug_level)
{
    return gnuradio::get_initial_sptr (new afsk1200_impl(sample_rate,debug_level));
}

/*
     * The private constructor
     */
afsk1200_impl::afsk1200_impl(int sample_rate, int debug_level)
    : gr::block("afsk1200",
                    gr::io_signature::make(1, 1, sizeof (float)),
                    gr::io_signature::make(1, 1, sizeof(char)))
{
    float f;
    int i;
    verbose_level=debug_level;
    d_sample_rate = sample_rate;
    d_corrlen     = d_sample_rate/BAUD;
    set_history(d_corrlen+1);
    d_sphaseinc   =  (0x10000u*BAUD*SUBSAMP/d_sample_rate);
    // Init the demodulator
    corr_mark_i = new float[d_corrlen];
    corr_mark_q = new float[d_corrlen];
    corr_space_i= new float[d_corrlen];
    corr_space_q= new float[d_corrlen];

    afsk12.dcd_shreg=0;
    afsk12.lasts    =0;
    afsk12.sphase   =0;
    afsk12.subsamp  =0;

    f=0.;
    for (i = 0; i < d_corrlen; i++) {
        f = 2.0*M_PI*FREQ_MARK*i/d_sample_rate;
        corr_mark_i[i] = cosf(f);
        corr_mark_q[i] = sinf(f);
    }
    f=0.;
    for (i = 0; i < d_corrlen; i++) {
        f = 2.0*M_PI*FREQ_SPACE*i/d_sample_rate;
        corr_space_i[i] = cosf(f);
        corr_space_q[i] = sinf(f);
    }
    memset(&hdlc, 0, sizeof(hdlc));

}

/*
     * Our virtual destructor.
     */
afsk1200_impl::~afsk1200_impl()
{
    delete corr_mark_i;
    delete corr_mark_q;
    delete corr_space_i;
    delete corr_space_q;
}

int
afsk1200_impl::general_work (int noutput_items,
                             gr_vector_int &ninput_items,
                             gr_vector_const_void_star &input_items,
                             gr_vector_void_star &output_items)
{
    float f;
    unsigned char curbit;
    int length;

    const float *in = (const float *) input_items[0];
    out = (char *) output_items[0];
    // Do <+signal processing+>
    length=noutput_items;
    d_numchars=0;
    if (afsk12.subsamp) {
        int numfill = SUBSAMP - afsk12.subsamp;
        if (length < numfill) {
            afsk12.subsamp += length;
            return length;
        }
        mark  += numfill;
        space += numfill;
        length -= numfill;
        afsk12.subsamp = 0;
    }
    for (; length >= SUBSAMP; length -= SUBSAMP) {
        mark += SUBSAMP;
        space += SUBSAMP;
        f = fsqr(mac(in, corr_mark_i, d_corrlen)) +
                fsqr(mac(in, corr_mark_q, d_corrlen)) -
                fsqr(mac(in, corr_space_i,d_corrlen)) -
                fsqr(mac(in, corr_space_q,d_corrlen));
        afsk12.dcd_shreg <<= 1;
        afsk12.dcd_shreg |= (f > 0);
        verbprintf(10, "%c", '0'+(afsk12.dcd_shreg & 1));
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
            verbprintf(9, " %c ", '0'+curbit);
            hdlc_rxbit( curbit);
        }
    }
    afsk12.subsamp = length;
    // Tell runtime system how many output items we produced.
    consume_each (noutput_items);
    return d_numchars;

}
void
afsk1200_impl::hdlc_rxbit(int bit)
{
    hdlc.rxbitstream <<= 1;
    hdlc.rxbitstream |= !!bit;
    if ((hdlc.rxbitstream & 0xff) == 0x7e) {
        if (hdlc.rxstate && (hdlc.rxptr - hdlc.rxbuf) > 2) {
            ax25_disp_packet(hdlc.rxbuf,hdlc.rxptr - hdlc.rxbuf);
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
            verbprintf(1, "Error: packet size too large\n");
            return;
        }
        *hdlc.rxptr++ = hdlc.rxbitbuf >> 1;
        hdlc.rxbitbuf = 0x80;
        return;
    }
    hdlc.rxbitbuf >>= 1;
}
void
afsk1200_impl::ax25_disp_packet(unsigned char *bp, unsigned int len)
{
    unsigned char v1=1,cmd=0;
    unsigned char i,j;

    if (!bp || len < 10)
        return;
    crc_ccitt1.reset();
    crc_ccitt1.process_bytes( bp, len );

    if (crc_ccitt1.checksum() != 0xf0b8) {
        if(verbose_level < 5)
            return;
        print_timestamp();
        verbprintf(5,"CRC Error\n");
    }
    else {
        print_timestamp();
        verbprintf(5,"No CRC Error\n");
    }
    len -= 2;
    if (bp[1] & 1) {
        /*
                     * FlexNet Header Compression
                     */
        v1 = 0;
        cmd = (bp[1] & 2) != 0;
        verbprintf(0, "AFSK1200: fm ? to ");
        i = (bp[2] >> 2) & 0x3f;
        if (i)
            verbprintf(0, "%c",i+0x20);
        i = ((bp[2] << 4) | ((bp[3] >> 4) & 0xf)) & 0x3f;
        if (i)
            verbprintf(0, "%c",i+0x20);
        i = ((bp[3] << 2) | ((bp[4] >> 6) & 3)) & 0x3f;
        if (i)
            verbprintf(0, "%c",i+0x20);
        i = bp[4] & 0x3f;
        if (i)
            verbprintf(0, "%c",i+0x20);
        i = (bp[5] >> 2) & 0x3f;
        if (i)
            verbprintf(0, "%c",i+0x20);
        i = ((bp[5] << 4) | ((bp[6] >> 4) & 0xf)) & 0x3f;
        if (i)
            verbprintf(0, "%c",i+0x20);
        verbprintf(0, "-%u QSO Nr %u", bp[6] & 0xf, (bp[0] << 6) | (bp[1] >> 2));
        bp += 7;
        len -= 7;
    } else {
        /*
                     * normal header
                     */
        if (len < 15)
            return;
        if ((bp[6] & 0x80) != (bp[13] & 0x80)) {
            v1 = 0;
            cmd = (bp[6] & 0x80);
        }
        verbprintf(0, "AFSK1200: fm ");
        for(i = 7; i < 13; i++)
            if ((bp[i] &0xfe) != 0x40)
                verbprintf(0, "%c",bp[i] >> 1);
        verbprintf(0, "-%u to ",(bp[13] >> 1) & 0xf);
        for(i = 0; i < 6; i++)
            if ((bp[i] &0xfe) != 0x40)
                verbprintf(0, "%c",bp[i] >> 1);
        verbprintf(0, "-%u",(bp[6] >> 1) & 0xf);
        bp += 14;
        len -= 14;
        if ((!(bp[-1] & 1)) && (len >= 7))
            verbprintf(0, " via ");
        while ((!(bp[-1] & 1)) && (len >= 7)) {
            for(i = 0; i < 6; i++)
                if ((bp[i] &0xfe) != 0x40)
                    verbprintf(0, "%c",bp[i] >> 1);
            verbprintf(0, "-%u",(bp[6] >> 1) & 0xf);
            bp += 7;
            len -= 7;
            if ((!(bp[-1] & 1)) && (len >= 7))
                verbprintf(0, ",");
        }
    }
    if(!len)
        return;
    i = *bp++;
    len--;
    j = v1 ? ((i & 0x10) ? '!' : ' ') :
             ((i & 0x10) ? (cmd ? '+' : '-') : (cmd ? '^' : 'v'));
    if (!(i & 1)) {
        /*
                 * Info frame
                 */
        verbprintf(0, " I%u%u%c",(i >> 5) & 7,(i >> 1) & 7,j);
    } else if (i & 2) {
        /*
             * U frame
             */
        switch (i & (~0x10)) {
        case 0x03:
            verbprintf(0, " UI%c",j);
            break;
        case 0x2f:
            verbprintf(0, " SABM%c",j);
            break;
        case 0x43:
            verbprintf(0, " DISC%c",j);
            break;
        case 0x0f:
            verbprintf(0, " DM%c",j);
            break;
        case 0x63:
            verbprintf(0, " UA%c",j);
            break;
        case 0x87:
            verbprintf(0, " FRMR%c",j);
            break;
        default:
            verbprintf(0, " unknown U (0x%x)%c",i & (~0x10),j);
            break;
        }
    } else {
        /*
             * supervisory
             */
        switch (i & 0xf) {
        case 0x1:
            verbprintf(0, " RR%u%c",(i >> 5) & 7,j);
            break;
        case 0x5:
            verbprintf(0, " RNR%u%c",(i >> 5) & 7,j);
            break;
        case 0x9:
            verbprintf(0, " REJ%u%c",(i >> 5) & 7,j);
            break;
        default:
            verbprintf(0, " unknown S (0x%x)%u%c", i & 0xf,
                       (i >> 5) & 7, j);
            break;
        }
    }
    if (!len) {
        verbprintf(0, "\n");
        return;
    }
    verbprintf(0, " pid=%02X\n", *bp++);
    len--;
    j = 0;
    while (len) {
        i = *bp++;
        if ((i >= 32) && (i < 128))
            verbprintf(0, "%c",i);
        else if (i == 13) {
            if (j)
                verbprintf(0, "\n");
            j = 0;
        } else
            verbprintf(0, ".");
        if (i >= 32)
            j = 1;
        len--;
    }
    if (j)
        verbprintf(0, "\n");
}

void
afsk1200_impl::verbprintf(int verb_level, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    if (verb_level <= verbose_level) {
        vsprintf(out, fmt, args);
    }
    va_end(args);
    int length=strlen(out);
    out +=length;
    d_numchars +=length;
}
void
afsk1200_impl::print_timestamp() {
    if(verbose_level >=1) {
    // Printe Date/time- Stamp on each packet
      time_t     now = time(0);
      struct tm  tstruct;
      tstruct = *localtime(&now);
      int numchars=strftime(out, 80, "%Y-%m-%d.%X", &tstruct);
      out +=numchars;
      *out = '\n';
      out++;
      d_numchars += numchars+1;
      }
    }
  } /* namespace afsk */
} /* namespace gr */

