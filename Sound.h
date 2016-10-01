#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#ifndef __Sound_h_
#define __Sound_h_

class Sound
{
public:
	Sound();
	Sound(std::string audioFile);
	void play();
	void loop();
	void pause();
	void stop();
private:
	// local variables
	bool isPaused;
	Uint8 *audio_pos; // global pointer to the audio buffer to be played
	Uint32 audio_len; // remaining length of the sample we have to play
	Uint32 wav_length; // length of our sample
	Uint8 *wav_buffer; // buffer containing our audio file
	SDL_AudioSpec wav_spec; // the specs of our piece of music
};

#endif // #ifndef __Sound_h_
