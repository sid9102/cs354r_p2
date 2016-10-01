#include "Sound.h"
void my_audio_callback(void *userdata, Uint8 *stream, int len);
Uint32 tempALen;
Uint8 *tempAPos;

Sound::Sound() {
	// DOESNT DO ANYTHING
}
Sound::Sound(std::string audioFile) {
	SDL_Init(SDL_INIT_AUDIO);
	SDL_LoadWAV(audioFile.c_str(), &wav_spec, &wav_buffer, &wav_length);
	// set the callback function
	wav_spec.callback = my_audio_callback;
	wav_spec.userdata = NULL;
	// set our global static variables
	audio_pos = wav_buffer; // copy sound buffer
	audio_len = wav_length; // copy file length
	tempALen = audio_len;
	tempAPos = audio_pos;
}

void Sound::play() {
	SDL_OpenAudio(&wav_spec, NULL);
	SDL_PauseAudio(0);

	while (audio_len > 0) {
		SDL_Delay(100);
	}
}

void Sound::loop() {
	while (!isPaused) {
		Sound::play();
	}
}
void Sound::pause() {
	if (isPaused) {
		SDL_PauseAudio(0);
		isPaused = 1;
	}
	else {
		SDL_PauseAudio(1);
		isPaused = 0;
	}
}

void Sound::stop() {
	SDL_CloseAudio();
	SDL_FreeWAV(wav_buffer);
}



void my_audio_callback(void *userdata, Uint8 *stream, int len) {

	if (tempALen == 0)
		return;

	len = (len > tempALen ? tempALen : len);
	//SDL_memcpy (stream, audio_pos, len); 					// simply copy from one buffer into the other
	SDL_MixAudio(stream, tempAPos, len, SDL_MIX_MAXVOLUME);// mix from one buffer into another

	tempAPos += len;
	tempALen -= len;
}