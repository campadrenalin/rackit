#pragma once
#import "core.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

const Sample AMPLITUDE = 13000;
const SDL_AudioFormat FORMAT_WANTED = AUDIO_S16SYS;
typedef Sint16 FORMAT;

static struct Module *mod_playing;
void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes) {
    FORMAT *buffer = (FORMAT*)raw_buffer;
    int length = bytes/2;

    struct Module *m = mod_playing;
    Buffer *out = Module_buffer(m, 0);
    Module_process(m, length, m->time+length);
    for (int i=0; i<length; i++)
        buffer[i] = (*out)[i]*AMPLITUDE;
}

// Allow this to be overridden in, for example, Lua context
#ifndef SDL_ERR
#define SDL_ERR(fmt) SDL_Log(fmt, SDL_GetError())
#endif

static SDL_AudioSpec want, have;

void sdl_init (void) {
    if (SDL_Init(SDL_INIT_AUDIO) != 0)
        return SDL_ERR("Failed to initialize SDL: %s");
    SDL_zero(want);
    want.freq = SAMPLE_RATE;
    want.format = FORMAT_WANTED;
    want.channels = 1;
    want.samples = BUFFER_SIZE;
    want.callback = audio_callback;
    want.userdata = &mod_playing;
    if (SDL_OpenAudio(&want, &have) != 0)
        return SDL_ERR("Failed to open audio: %s");
    // SDL_Log("Got: freq %d, format %d, channels %d, silence %d, samples %d, size %d",
            // have.freq, have.format, have.channels, have.silence, have.samples, have.size);
    if (want.format != have.format)
        return SDL_ERR("Failed to get the desired AudioSpec: %s");
    if (want.freq != have.freq)
        return SDL_ERR("Failed to get the desired frequency: %s");
}

void sdl_play(struct Module *m, int ms) {
    mod_playing = m;
    SDL_PauseAudio(0);
    SDL_Delay(ms);
    SDL_PauseAudio(1);
}

void sdl_finish() { SDL_CloseAudio(); }

