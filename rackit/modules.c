#include "buffer.h"
#include "sdl_player.h"
const double PI = 3.141592653589793238463;
const double TAU = 2*PI;
const int AMPLITUDE = 13000;
const int SAMPLE_RATE = 44100;
const double sr = SAMPLE_RATE;
const SDL_AudioFormat FORMAT_WANTED = AUDIO_S16SYS;
typedef Sint16 FORMAT;

void print_stack(lua_State *L) {
    int n = lua_gettop(L); //number of arguments
    SDL_Log("Printing stack (%d items)", n);
    for (int i=1; i <= n; i++) {
        int t = lua_type(L, i);
        SDL_Log("%i => <%s> %s", i, lua_typename(L, t), lua_tostring(L, i));
    }
    SDL_Log("---");
}

void * get_table_userdata(lua_State *L, int pos) {
    lua_getfield(L, pos, "_userdata"); // +struct
    void *p = lua_touserdata(L, -1);
    lua_pop(L, 1); // -struct
    return p;
}

#include "modules/actor.c"
#include "modules/mixer.c"

Buffer buf_master;
Mixer mix_master = { 0, {}, &buf_master };

#include "modules/sine.c"
