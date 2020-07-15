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


#ifndef INCLUDED_AFSK_AX25DECODE_H
#define INCLUDED_AFSK_AX25DECODE_H

#include <ax25/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace ax25 {

    /*!
     * \brief <+description of block+>
     * \ingroup ax25
     *
     */
    class AFSK_API ax25decode : virtual public block
    {
     public:
      typedef std::shared_ptr<ax25decode> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of afsk::ax25decode.
       *
       * To avoid accidental use of raw pointers, afsk::ax25decode's
       * constructor is in a private implementation
       * class. afsk::ax25decode::make is the public interface for
       * creating new instances.
       */
      static sptr make(int sample_rate,int debug_level=5);
    };

  } // namespace afsk
} // namespace gr

#endif /* INCLUDED_AFSK_AX25DECODE_H */

