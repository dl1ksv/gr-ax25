/* -*- c++ -*- */

#define AFSK_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "afsk_swig_doc.i"

%{
#include "afsk/afsk1200.h"
#include "afsk/ax25decode.h"
#include "afsk/aprs2inet.h"
%}


%include "afsk/afsk1200.h"
GR_SWIG_BLOCK_MAGIC2(afsk, afsk1200);
%include "afsk/ax25decode.h"
GR_SWIG_BLOCK_MAGIC2(afsk, ax25decode);

%include "afsk/aprs2inet.h"
GR_SWIG_BLOCK_MAGIC2(afsk, aprs2inet);
