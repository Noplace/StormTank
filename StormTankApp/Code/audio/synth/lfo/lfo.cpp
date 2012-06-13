//============================================================================
/**
	Implementation file for LFO.hpp
	
	@author		Remy Muller
	@date		20030822
*/
//============================================================================

#include <cmath>
#include "LFO.h"


#define kIndex 16
#define kIndexRange (1<<kIndex)
#define kPrecision 48
#define kPrecisionMask 0xFFFFFFFFFFFF
#define kPrecisionRange 281474976710656.0 //(double)(1<<kPrecision);

LFO::LFO()
  : samplerate(0),
    phase(0),
    inc(0)
{
  table = new double[kIndexRange+1];
  //setWaveform(LFO::triangle);   
  //setRate(440.0); //1Hz
}

LFO::~LFO() {
  delete [] table;
}

static const double k1Div24lowerBits = 1.0/(kPrecisionRange);

double LFO::tick()
{
  // the 8 MSB are the index in the table in the range 0-255 
  int i = phase >> kPrecision; 

  // and the kPrecision LSB are the fractionnal part
  double frac = (phase & kPrecisionMask) * k1Div24lowerBits;

  // increment the phase for the next tick
  phase += inc; // the phase overflow itself

  return table[i]*(1.0-frac) + table[i+1]*frac; // linear interpolation
}

void LFO::setRate(double rate)
{
  /** the rate in Hz is converted to a phase increment with the following formula
      \f[ inc = (256*rate/samplerate) * 2^kPrecision \f] */
  inc =  (phase_t)((double(kIndexRange) * rate / samplerate) * kPrecisionRange);
}

void LFO::setWaveform(waveform_t index)
{
  switch(index)
    {
    case sinus:
      {
	double pi = 4.0 * atan(1.0);

	int i;
	for(i=0;i<=kIndexRange;i++)
	  table[i] = sin(2.0*pi*(i/double(kIndexRange)));

	break;
      }
    case triangle:
      {
	int i;
	for(i=0;i<64;i++)
	  {
	    table[i]     =        i / 64.0;
	    table[i+64]  =   (64-i) / 64.0;
	    table[i+128] =      - i / 64.0;
	    table[i+192] = - (64-i) / 64.0;
	  }
	table[256] = 0.0;
	break;
      }
    case sawtooth:
      {
	int i;
	for(i=0;i<256;i++)
	  {
	    table[i] = 2.0*(i/255.0) - 1.0;
	  }
	table[256] = -1.0;
	break;
      }
    case square:
      {
	int i;
	for(i=0;i<128;i++)
	  {
	    table[i]     =  1.0;
	    table[i+128] = -1.0;
	  }
	table[256] = 1.0;
	break;
      }
    case exponent:
      {
	/* symetric exponent similar to triangle */
	int i;
	double e = exp(1.0);
	for(i=0;i<128;i++)
	  {
	    table[i] = 2.0 * ((exp(i/128.0) - 1.0) / (e - 1.0)) - 1.0  ;
	    table[i+128] = 2.0 * ((exp((128-i)/128.0) - 1.0) / (e - 1.0)) - 1.0  ;
	  }
	table[256] = -1.0;
	break;
      }
    default:
      {
	break;
      }
    } 
}
