#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

static int reverse (lua_State *L) {
    int n_args = lua_gettop(L);
    for (int i = n_args; i>0; i--)
        lua_pushvalue(L, i);
    return n_args;
}

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

static const struct luaL_Reg TopLevel[] = {
    {"reverse", reverse},
    {"Buffer", new_buffer},
    {NULL,NULL} // Sentinel value
};

int luaopen_audio_lua (lua_State *L) {
    luaL_newlib(L, TopLevel);
    luaL_newmetatable(L, "Buffer");
    luaL_setfuncs(L, BufferMetatable, 0);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    lua_pop(L, 1);
    return 1;
}
