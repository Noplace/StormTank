// Music

#ifndef _4K_SONANT_MUSIC_
#define _4K_SONANT_MUSIC_

#include "../audio/sonant/sonant.h"

static song songdata = {
	{ // Instruments
		{ // 0
			// Oscillator 1
			5, // Octave knob
			0, // Detune knob
			0, // Actual detune knob
			0, // Multiply freq by envelope
			192, // Volume knob
			0, // Wave form
			// Oscillator 2
			5, // Octave knob
			0, // Detune knob
			12, // Actual detune knob
			0, // Multiply freq by envelope
			192, // Volume knob
			3, // Wave form
			// Noise oscillator
			0, // Amount of noise to add
			// Envelope
			200, // Attack
			2000, // Sustain
			20000, // Release
			200, // Master volume knob
			// Effects
			2, // Hi/lo/bandpass or notch toggle
			11025, // FX Frequency
			240, // FX Resonance
			6, // Delay time
			120, // Delay amount
			0, // Panning frequency
			0, // Panning amount
			// LFO
			0, // Modify osc1 freq (FM) toggle
			1, // Modify fx freq toggle
			2, // LFO freq
			210, // LFO amount
			0, // LFO waveform
			// Patterns
			{1,2,1,2,1,2,1,2,1,2,1,2,0,3,7,3,7,3,7,3,7,3,7,3,7,0,4,5,4,5,4,5,4,5,0,3,6,3,6,3,6,3,6,0,0,0,0,0}, // Pattern order
			//{0}, // Mute
			{ // Columns
				{{153,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,155,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{153,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,152,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{155,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,152,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{155,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,152,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{155,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,154,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{150,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,153,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{157,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,153,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
			}
		},
		{ // 1
			// Oscillator 1
			6, // Octave knob
			7, // Detune knob
			8, // Actual detune knob
			0, // Multiply freq by envelope
			200, // Volume knob
			0, // Wave form
			// Oscillator 2
			6, // Octave knob
			0, // Detune knob
			0, // Actual detune knob
			0, // Multiply freq by envelope
			200, // Volume knob
			3, // Wave form
			// Noise oscillator
			0, // Amount of noise to add
			// Envelope
			200, // Attack
			2000, // Sustain
			16000, // Release
			180, // Master volume knob
			// Effects
			2, // Hi/lo/bandpass or notch toggle
			11025, // FX Frequency
			240, // FX Resonance
			6, // Delay time
			160, // Delay amount
			0, // Panning frequency
			0, // Panning amount
			// LFO
			0, // Modify osc1 freq (FM) toggle
			1, // Modify fx freq toggle
			4, // LFO freq
			210, // LFO amount
			0, // LFO waveform
			// Patterns
			{0,0,0,0,1,2,1,2,1,2,1,2,0,3,4,3,4,3,4,3,4,3,4,3,4,3,5,6,5,6,5,6,5,6,6,3,4,3,4,3,4,3,4,3,4,3,4,0}, // Pattern order
			//{0}, // Mute
			{ // Columns
				{{153,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,150,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,153,  0,  0,  0,  0,  0,  0,  0,  0,  0,155,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{148,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,146,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{150,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,148,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{150,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,148,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,155,  0,  0,  0,  0,  0,  0,  0,  0,  0,150,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
			}
		},
		{ // 2
			// Oscillator 1
			7, // Octave knob
			0, // Detune knob
			0, // Actual detune knob
			0, // Multiply freq by envelope
			200, // Volume knob
			2, // Wave form
			// Oscillator 2
			7, // Octave knob
			0, // Detune knob
			14, // Actual detune knob
			0, // Multiply freq by envelope
			200, // Volume knob
			3, // Wave form
			// Noise oscillator
			0, // Amount of noise to add
			// Envelope
			200, // Attack
			2000, // Sustain
			16000, // Release
			122, // Master volume knob
			// Effects
			2, // Hi/lo/bandpass or notch toggle
			11025, // FX Frequency
			240, // FX Resonance
			6, // Delay time
			190, // Delay amount
			6, // Panning frequency
			81, // Panning amount
			// LFO
			0, // Modify osc1 freq (FM) toggle
			1, // Modify fx freq toggle
			4, // LFO freq
			210, // LFO amount
			0, // LFO waveform
			// Patterns
			{1,2,1,2,0,0,0,0,1,2,1,2,0,3,4,3,4,3,4,3,4,3,4,3,4,3,5,6,5,7,5,6,5,7,0,3,4,3,4,3,4,3,4,3,4,3,4,0}, // Pattern order
			//{0}, // Mute
			{ // Columns
				{{  0,  0,  0,  0,153,  0,  0,  0,  0,  0,153,  0,  0,  0,155,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,153,  0,  0,  0,  0,  0,153,  0,155,  0,152,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{152,  0,  0,  0,  0,  0,153,  0,  0,  0,  0,  0,150,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{153,  0,  0,  0,  0,  0,155,  0,  0,  0,  0,  0,152,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,157,  0,  0,  0,  0,  0,157,  0,  0,  0,152,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,155,  0,  0,  0,  0,  0,155,  0,157,  0,154,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{159,  0,157,  0,162,  0,  0,  0,  0,  0,155,  0,157,  0,154,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
			}
		},
		{ // 3
			// Oscillator 1
			8, // Octave knob
			0, // Detune knob
			0, // Actual detune knob
			0, // Multiply freq by envelope
			200, // Volume knob
			0, // Wave form
			// Oscillator 2
			8, // Octave knob
			7, // Detune knob
			8, // Actual detune knob
			0, // Multiply freq by envelope
			200, // Volume knob
			3, // Wave form
			// Noise oscillator
			0, // Amount of noise to add
			// Envelope
			36800, // Attack
			36800, // Sustain
			36800, // Release
			20, // Master volume knob
			// Effects
			4, // Hi/lo/bandpass or notch toggle
			11025, // FX Frequency
			240, // FX Resonance
			6, // Delay time
			100, // Delay amount
			3, // Panning frequency
			254, // Panning amount
			// LFO
			0, // Modify osc1 freq (FM) toggle
			1, // Modify fx freq toggle
			6, // LFO freq
			210, // LFO amount
			0, // LFO waveform
			// Patterns
			{0,0,0,0,0,0,0,0,1,2,1,2,0,0,0,0,0,3,4,3,4,5,6,7,6,0,0,0,0,0,8,9,10,9,0,3,4,3,4,3,4,3,4,0,0,0,0,0}, // Pattern order
			//{0}, // Mute
			{ // Columns
				{{148,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,150,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{148,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,145,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{155,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,150,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{153,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,148,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{146,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,146,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{150,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,148,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{150,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,146,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{150,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,152,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{155,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,150,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{148,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,148,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
			}
		},
		{ // 4
			// Oscillator 1
			8, // Octave knob
			7, // Detune knob
			0, // Actual detune knob
			0, // Multiply freq by envelope
			200, // Volume knob
			1, // Wave form
			// Oscillator 2
			8, // Octave knob
			7, // Detune knob
			14, // Actual detune knob
			0, // Multiply freq by envelope
			200, // Volume knob
			2, // Wave form
			// Noise oscillator
			0, // Amount of noise to add
			// Envelope
			2000, // Attack
			6800, // Sustain
			2000, // Release
			70, // Master volume knob
			// Effects
			3, // Hi/lo/bandpass or notch toggle
			11025, // FX Frequency
			254, // FX Resonance
			6, // Delay time
			120, // Delay amount
			5, // Panning frequency
			214, // Panning amount
			// LFO
			0, // Modify osc1 freq (FM) toggle
			1, // Modify fx freq toggle
			7, // LFO freq
			255, // LFO amount
			1, // LFO waveform
			// Patterns
			{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,1,2,1,2,1,2,0,0,0,0,0}, // Pattern order
			//{0}, // Mute
			{ // Columns
				{{155,  0,150,  0,143,  0,153,  0,157,  0,158,  0,155,  0,150,  0,155,  0,150,  0,155,  0,158,  0,160,  0,157,  0,150,  0,153,  0,}},
				{{155,  0,150,  0,143,  0,153,  0,157,  0,158,  0,155,  0,150,  0,155,  0,150,  0,155,  0,158,  0,160,  0,157,  0,160,  0,162,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
			}
		},
		{ // 5
			// Oscillator 1
			7, // Octave knob
			0, // Detune knob
			0, // Actual detune knob
			1, // Multiply freq by envelope
			255, // Volume knob
			0, // Wave form
			// Oscillator 2
			7, // Octave knob
			0, // Detune knob
			0, // Actual detune knob
			1, // Multiply freq by envelope
			255, // Volume knob
			0, // Wave form
			// Noise oscillator
			0, // Amount of noise to add
			// Envelope
			50, // Attack
			150, // Sustain
			4800, // Release
			200, // Master volume knob
			// Effects
			2, // Hi/lo/bandpass or notch toggle
			600, // FX Frequency
			254, // FX Resonance
			0, // Delay time
			0, // Delay amount
			0, // Panning frequency
			0, // Panning amount
			// LFO
			0, // Modify osc1 freq (FM) toggle
			0, // Modify fx freq toggle
			0, // LFO freq
			0, // LFO amount
			0, // LFO waveform
			// Patterns
			{0,0,0,0,1,1,1,1,1,1,1,1,3,1,2,1,2,1,2,1,2,4,4,4,4,3,1,1,1,1,1,1,1,1,4,1,2,1,2,1,2,1,2,4,4,4,4,0}, // Pattern order
			//{0}, // Mute
			{ // Columns
				{{147,  0,  0,  0,147,147,  0,147,  0,  0,147,  0,  0,  0,147,  0,  0,  0,147,  0,147,  0,  0,147,  0,  0,147,  0,  0,  0,147,  0,}},
				{{147,  0,  0,  0,147,147,  0,147,  0,  0,147,  0,  0,  0,147,  0,  0,  0,147,  0,147,  0,  0,  0,147,147,147,147,147,147,147,147,}},
				{{147,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,147,147,147,147,147,147,147,147,}},
				{{147,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
			}
		},
		{ // 6
			// Oscillator 1
			8, // Octave knob
			0, // Detune knob
			0, // Actual detune knob
			1, // Multiply freq by envelope
			160, // Volume knob
			0, // Wave form
			// Oscillator 2
			8, // Octave knob
			0, // Detune knob
			0, // Actual detune knob
			1, // Multiply freq by envelope
			160, // Volume knob
			0, // Wave form
			// Noise oscillator
			210, // Amount of noise to add
			// Envelope
			50, // Attack
			200, // Sustain
			6800, // Release
			160, // Master volume knob
			// Effects
			4, // Hi/lo/bandpass or notch toggle
			11025, // FX Frequency
			254, // FX Resonance
			6, // Delay time
			32, // Delay amount
			5, // Panning frequency
			61, // Panning amount
			// LFO
			0, // Modify osc1 freq (FM) toggle
			1, // Modify fx freq toggle
			4, // LFO freq
			60, // LFO amount
			0, // LFO waveform
			// Patterns
			{0,0,0,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,0,0,0,0}, // Pattern order
			//{0}, // Mute
			{ // Columns
				{{  0,  0,  0,  0,147,  0,  0,  0,  0,  0,  0,  0,147,  0,  0,  0,  0,  0,  0,  0,147,  0,  0,  0,  0,  0,  0,  0,  0,147,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
			}
		},
		{ // 7
			// Oscillator 1
			0, // Octave knob
			0, // Detune knob
			0, // Actual detune knob
			0, // Multiply freq by envelope
			0, // Volume knob
			0, // Wave form
			// Oscillator 2
			0, // Octave knob
			0, // Detune knob
			0, // Actual detune knob
			0, // Multiply freq by envelope
			0, // Volume knob
			0, // Wave form
			// Noise oscillator
			255, // Amount of noise to add
			// Envelope
			50, // Attack
			150, // Sustain
			4800, // Release
			20, // Master volume knob
			// Effects
			1, // Hi/lo/bandpass or notch toggle
			11025, // FX Frequency
			254, // FX Resonance
			6, // Delay time
			16, // Delay amount
			7, // Panning frequency
			254, // Panning amount
			// LFO
			0, // Modify osc1 freq (FM) toggle
			1, // Modify fx freq toggle
			6, // LFO freq
			255, // LFO amount
			0, // LFO waveform
			// Patterns
			{0,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,0}, // Pattern order
			//{0}, // Mute
			{ // Columns
				{{  0,147,147,  0,147,147,147,147,147,  0,  0,147,147,  0,147,147,147,  0,147,  0,147,147,  0,147,147,  0,147,  0,147,147,147,147,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
				{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},
			}
		},
	}
};
// Empty column
//{{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,}},

#define _4K_SONANT_ROWLEN_ 4625 // In sample lengths
//#define _4K_SONANT_FASTFORWARD_ 0 // In pattern lengths
#define _4K_SONANT_ENDPATTERN_ 50 // End pattern

#endif
