/*****************************************************************************************************************
* Copyright (c) 2012 Khalid Ali Al-Kooheji                                                                       *
*                                                                                                                *
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and              *
* associated documentation files (the "Software"), to deal in the Software without restriction, including        *
* without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell        *
* copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the       *
* following conditions:                                                                                          *
*                                                                                                                *
* The above copyright notice and this permission notice shall be included in all copies or substantial           *
* portions of the Software.                                                                                      *
*                                                                                                                *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT          *
* LIMITED TO THE WARRANTIES OF MERCHANTABILITY, * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.          *
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, * DAMAGES OR OTHER LIABILITY,      *
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE            *
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                                         *
*****************************************************************************************************************/
#ifndef AUDIO_SYNTH_TYPES_H
#define AUDIO_SYNTH_TYPES_H

#define _2_POW_12TH 1.0594630943592952645618252949463f
#define _LN_2 0.69314718055994530941723212145818f
#define _LN_2_DIV_12 0.05776226504666210911810267678818f

namespace audio {
namespace synth {

struct NoteData {
    real_t freq,base_freq; //Polyphony note polyphony
    real_t velocity;
    int note;
    bool on;
};

const int note_count = 127;

enum Notes {
  kNoteA0=21,kNoteBb0,kNoteB0,
  kNoteC1,kNoteDb1,kNoteD1,kNoteEb1,kNoteE1,kNoteF1,kNoteGb1,kNoteG1,kNoteAb1,kNoteA1,kNoteBb1,kNoteB1,
  kNoteC2,kNoteDb2,kNoteD2,kNoteEb2,kNoteE2,kNoteF2,kNoteGb2,kNoteG2,kNoteAb2,kNoteA2,kNoteBb2,kNoteB2,
  kNoteC3,kNoteDb3,kNoteD3,kNoteEb3,kNoteE3,kNoteF3,kNoteGb3,kNoteG3,kNoteAb3,kNoteA3,kNoteBb3,kNoteB3,
  kNoteC4,kNoteDb4,kNoteD4,kNoteEb4,kNoteE4,kNoteF4,kNoteGb4,kNoteG4,kNoteAb4,kNoteA4,kNoteBb4,kNoteB4,
  kNoteC5,kNoteDb5,kNoteD5,kNoteEb5,kNoteE5,kNoteF5,kNoteGb5,kNoteG5,kNoteAb5,kNoteA5,kNoteBb5,kNoteB5,
  kNoteC6,kNoteDb6,kNoteD6,kNoteEb6,kNoteE6,kNoteF6,kNoteGb6,kNoteG6,kNoteAb6,kNoteA6,kNoteBb6,kNoteB6,
  kNoteC7,kNoteDb7,kNoteD7,kNoteEb7,kNoteE7,kNoteF7,kNoteGb7,kNoteG7,kNoteAb7,kNoteA7,kNoteBb7,kNoteB7,
  kNoteC8,kNoteDb8,kNoteD8,kNoteEb8,kNoteE8,kNoteF8,kNoteGb8,kNoteG8,kNoteAb8,kNoteA8,kNoteBb8,kNoteB8,
};

}
}

#endif