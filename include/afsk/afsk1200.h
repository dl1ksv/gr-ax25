/* -*- c++ -*- */
/* 
 * Copyright 2013 Volker Schroer, DL1KSV.
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


#ifndef INCLUDED_AFSK_AFSK1200_H
#define INCLUDED_AFSK_AFSK1200_H

#include <afsk/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace afsk {

    /*!
     * \brief <+description of block+>
     * \ingroup afsk
     *
     */
    class AFSK_API afsk1200 : virtual public block
    {
     public:
      typedef boost::shared_ptr<afsk1200> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of afsk::afsk1200.
       *
       * To avoid accidental use of raw pointers, afsk::afsk1200's
       * constructor is in a private implementation
       * class. afsk::afsk1200::make is the public interface for
       * creating new instances.
       */
      static sptr make(int sample_rate,int debug_level=2);
    };

  } // namespace afsk
} // namespace gr

#endif /* INCLUDED_AFSK_AFSK1200_H */

