#ifndef LFO_H
#define LFO_H


//==============================================================================
/**
   This class implements a LUT-based LFO with various waveforms and linear 
   interpolation.
   
   It uses 32-bit fixed-point phase and increment, where the 8 MSB
   represent the integer part of the number and the 24 LSB the fractionnal part
   
   @author		Remy Muller
   @date		20030822
*/
//==============================================================================


class LFO
{
public:

  /** type enumeration used to select the different waveforms*/
  //typedef 
  typedef enum {triangle, sinus, sawtooth, square, exponent, kNumWave} waveform_t;
  
  /** phase type */
  typedef unsigned __int64 phase_t;


  /**  @param samplerate the samplerate in Hz */
  LFO();
  virtual ~LFO();

  /** increments the phase and outputs the new LFO value.
      @return the new LFO value between [-1;+1] */ 
  double tick();

  /** change the current rate
      @param rate new rate in Hz */
  void setRate(const double rate);

  /** change the current samplerate
      @param samplerate new samplerate in Hz */
  void setSampleRate(const double samplerate_) {samplerate = (samplerate_>0.0) ? samplerate_ : 44100.0;}

  /** select the desired waveform for the LFO
      @param index tag of the waveform
   */
  void setWaveform(waveform_t index);



private:

  double samplerate;
  
  /** phase and phase increment
      the 8 Most Significant Bits represent the integer part,
      the 24 Least Significant Bits represent the fractionnal part.
      that way we can automatically adress the table with the MSB
      between 0-255 (which overflow automatically) and use the LSB 
      to determine the fractionnal part for linear interpolation with a precision of 
      \f[ 2^-24 \f] */
  phase_t phase,inc;
  
  /** table length is 256+1, with table[0] = table[256] 
      that way we can perform linear interpolation:
      \f[ val = (1-frac)*u[n] + frac*u[n+1] \f]
      even with n = 255.
      For n higher than 255, n is automatically  wrapped to 0-255*/
  double* table;//[257]; 
};

#endif	// #ifndef LFO_H
