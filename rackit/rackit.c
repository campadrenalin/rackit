#include "buffer.c"
#include "modules.c"
#include "sdl_player.c"

static const struct luaL_Reg TopLevel[] = {
    {"sdl_init",   sdl_init},
    {"sdl_play",   sdl_play},
    {"sdl_pause",  sdl_pause},
    {"sdl_finish", sdl_finish},
    {"Buffer", new_buffer},
    {"OscSine", OscSine_new},
    {"OscSaw", OscSaw_new},
    {"OscSquare", OscSquare_new},
    {"LFO", LFO_new},
    {"MixMaster", MixMaster},
    {NULL,NULL} // Sentinel value
};

int luaopen_rackit (lua_State *L) {
    luaL_newlib(L, TopLevel);
    Buffer_register(L);
    OscSine_register(L);
    OscSaw_register(L);
    OscSquare_register(L);
    LFO_register(L);
    return 1;
}
