/*
    Example implementation of the PADsynth basic algorithm
    By: Nasca O. Paul, Tg. Mures, Romania
    This implementation and the algorithm are released under Public Domain
    Feel free to use it into your projects or your products ;-)

    This implementation is tested under GCC/Linux, but it's 
    very easy to port to other compiler/OS.
    
    P.S.
    Please note, that IFFT function depends on the FFTW library, so if you want 
    to use into commercial products, you must replace it with your IFFT routine
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "PADsynth.h"



#define number_harmonics 64
int test_padsynth(){
    srand(time(0));
    #define N (262144)
REALTYPE* sample = new REALTYPE[N];
    PADsynth synth(N,44100,number_harmonics);

    synth.setharmonic(0,0.0f);
    for (int i=1;i<number_harmonics;i++) {
	    synth.setharmonic(i,1.0f/i);
	    if ((i%2)==0) 
        synth.setharmonic(i,synth.getharmonic(i)*2.0f);
    };


    synth.synth(261.0f,40.0f,1,sample);
    //padsynth_basic_algorithm(N,44100,261.0,40.0,number_harmonics,A,sample);

    /* Output the data to the 16 bit, mono raw file */
    short* isample = new short[N];
    for (int i=0;i<N;i++) {
      isample[i] =(short)(sample[i]*32768.0);
      //*isample++ =(short)(sample[i]*32768.0);
    }
    FILE *f=fopen("sample.raw","w");fwrite(isample,N,2,f);fclose(f);
    delete [] isample;
    delete [] sample;
    return 0;
};
