#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

static const int BUFFER_SIZE = 2048;
typedef double Buffer[BUFFER_SIZE];

void fill_buffer(Buffer *buf, double value) {
    for (int i=0; i<BUFFER_SIZE-1; i++)
        (*buf)[i] = value;
}

static int new_buffer (lua_State *L) {
    double fill_value = 0;
    if (lua_gettop(L) == 1)
        fill_value = luaL_checknumber(L, 1);

    void *userdata = lua_newuserdata(L, sizeof(Buffer));
    Buffer *buf = userdata;
    fill_buffer(buf, fill_value);
    luaL_getmetatable(L, "Buffer");
    lua_setmetatable(L, -2);
    return 1;
}

static int buffer_peek (lua_State *L) {
    void *userdata = lua_touserdata(L, -1);
    Buffer *buf = userdata;
    lua_pushnumber(L, (*buf)[0]);
    return 1;
}

static const struct luaL_Reg BufferMetatable[] = {
    {"peek", buffer_peek},
    {NULL,NULL} // Sentinel value
};

#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include "math.h"
const double PI = 3.141592653589793238463;
const double TAU = 2*PI;
const int AMPLITUDE = 13000;
const int SAMPLE_RATE = 44100;
const double sr = SAMPLE_RATE;
const SDL_AudioFormat FORMAT_WANTED = AUDIO_S16SYS;
typedef Sint16 FORMAT;
Buffer master;

typedef struct {
    double phase;
    double freq;
    Buffer *out;
} OscSine;

void OscSine_process(OscSine *osc, int length) {
    Buffer *out = osc->out;
    double p = 0;
    double f = osc->freq*TAU;
    for(int i=0; i<length; i++) {
        double t = (double)i/sr;
        p = t*f;
        (*out)[i] = sin(osc->phase + p);
    }
    osc->phase += p;
    // SDL_Log("Phase: %f", osc->phase);
}

OscSine wave = {0, 440, &master};

#include "time.h"
void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes) {
    struct timespec start, end;

    FORMAT *buffer = (FORMAT*)raw_buffer;
    int length = bytes/2;

    clock_gettime(CLOCK_MONOTONIC, &start);
    SDL_zero(master);
    OscSine_process(&wave, length);
    for (int i=0; i<length; i++)
        buffer[i] = master[i]*AMPLITUDE;
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

static const struct luaL_Reg TopLevel[] = {
    {"sdl_init",   sdl_init},
    {"sdl_play",   sdl_play},
    {"sdl_pause",  sdl_pause},
    {"sdl_finish", sdl_finish},
    {"Buffer", new_buffer},
    {NULL,NULL} // Sentinel value
};

void register_class(lua_State *L, const char *name, const struct luaL_Reg methods[]) {
    luaL_newmetatable(L, name);
    luaL_setfuncs(L, methods, 0);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    lua_pop(L, 1);
}

int luaopen_audio_lua (lua_State *L) {
    luaL_newlib(L, TopLevel);
    register_class(L, "Buffer", BufferMetatable);
    return 1;
}
