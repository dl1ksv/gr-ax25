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


#ifndef INCLUDED_AFSK_APRS2INET_H
#define INCLUDED_AFSK_APRS2INET_H

#include <afsk/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace afsk {

    /*!
     * \brief <+description of block+>
     * \ingroup afsk
     *
     */
    class AFSK_API aprs2inet : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<aprs2inet> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of afsk::aprs2inet.
       *
       * To avoid accidental use of raw pointers, afsk::aprs2inet's
       * constructor is in a private implementation
       * class. afsk::aprs2inet::make is the public interface for
       * creating new instances.
       */
      static sptr make(int sample_rate,int debug_level);
    };

  } // namespace afsk
} // namespace gr

#endif /* INCLUDED_AFSK_aprs2inet_H */

