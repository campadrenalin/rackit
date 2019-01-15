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
    {NULL,NULL} // Sentinel value
};

void register_class(lua_State *L, const char *name, const struct luaL_Reg methods[]) {
    luaL_newmetatable(L, name);   // +mt

    int has_index = 0;
    for (int i=0; methods[i].name != NULL; i++)
        if (strncmp(methods[i].name, "__index", 7) == 0)
            has_index = 1;

    if (has_index == 0) {
        lua_pushstring(L, "__index"); // +"__index"
        lua_pushvalue(L, -2);         // +mt
        lua_settable(L, -3);          // -"__index" -mt
    }
    luaL_setfuncs(L, methods, 0);
    lua_pop(L, 1);
}

int luaopen_rackit (lua_State *L) {
    luaL_newlib(L, TopLevel);
    register_class(L, "Buffer", BufferMetatable);
    register_class(L, "OscSine", OscSineMetatable);
    return 1;
}
