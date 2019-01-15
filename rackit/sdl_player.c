#include "sdl_player.h"

void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes) {
    struct timespec start, end;

    FORMAT *buffer = (FORMAT*)raw_buffer;
    int length = bytes/2;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i=0; i<num_actors; i++) {
        Actor actor = actors[i];
        actor.callback(actor.data, length);
    }
    Mixer_process(&mix_master, length);
    // OscSine_process(&wave, length);
    for (int i=0; i<length; i++)
        buffer[i] = buf_master[i]*AMPLITUDE;
    clock_gettime(CLOCK_MONOTONIC, &end);

    time_t seconds = end.tv_sec - start.tv_sec;
    long nanos = (seconds * 1e9)  + end.tv_nsec - start.tv_nsec;
    // SDL_Log("Produced %d samples (%d bytes) in %f seconds, first value %d",
            // length, bytes, (double)nanos/1e9, buffer[0]);
}

int sdl_err(lua_State *L, const char* fmt) {
    return luaL_error(L, fmt, SDL_GetError());
}

static int sdl_init (lua_State *L) {
    if (SDL_Init(SDL_INIT_AUDIO) != 0)
        return sdl_err(L, "Failed to initialize SDL: %s");

    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = SAMPLE_RATE;
    want.format = FORMAT_WANTED;
    want.channels = 1;
    want.samples = BUFFER_SIZE;
    want.callback = audio_callback;
    if (SDL_OpenAudio(&want, &have) != 0)
        return sdl_err(L, "Failed to open audio: %s");
    // SDL_Log("Got: freq %d, format %d, channels %d, silence %d, samples %d, size %d",
            // have.freq, have.format, have.channels, have.silence, have.samples, have.size);
    if (want.format != have.format)
        return sdl_err(L, "Failed to get the desired AudioSpec");
    if (want.freq != have.freq)
        return sdl_err(L, "Failed to get the desired frequency");
    return 0;
}

static int sdl_play(lua_State *L) {
    SDL_PauseAudio(0);
    if (lua_gettop(L) == 1) {
        SDL_Delay((int)luaL_checknumber(L, 1));
        SDL_PauseAudio(1);
    }
    return 0;
}

static int sdl_pause(lua_State *L) {
    SDL_PauseAudio(1);
    return 0;
}

static int sdl_finish(lua_State *L) {
    SDL_CloseAudio();
    return 0;
}

