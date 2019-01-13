#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

static int reverse (lua_State *L) {
    int n_args = lua_gettop(L);
    for (int i = n_args; i>0; i--)
        lua_pushvalue(L, i);
    return n_args;
}

static const struct luaL_Reg mylib[] = {
    {"reverse", reverse},
    {NULL,NULL} // Sentinel value
};

int luaopen_audio_lua (lua_State *L) {
    luaL_newlib(L, mylib);
    return 1;
}
