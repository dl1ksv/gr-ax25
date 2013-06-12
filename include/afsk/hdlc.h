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


#ifndef INCLUDED_AFSK_HDLC_H
#define INCLUDED_AFSK_HDLC_H

#include <afsk/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace afsk {

    /*!
     * \brief <+description of block+>
     * \ingroup afsk
     *
     */
    class AFSK_API hdlc : virtual public block
    {
     public:
      typedef boost::shared_ptr<hdlc> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of afsk::hdlc.
       *
       * To avoid accidental use of raw pointers, afsk::hdlc's
       * constructor is in a private implementation
       * class. afsk::hdlc::make is the public interface for
       * creating new instances.
       */
      static sptr make(int sample_rate, int subsample,int debug_level=5);
    };

  } // namespace afsk
} // namespace gr

#endif /* INCLUDED_AFSK_HDLC_H */

