#include "buffer.h"

void fill_buffer(Buffer *buf, double value) {
    for (int i=0; i<BUFFER_SIZE-1; i++)
        (*buf)[i] = value;
}

void Buffer_new(lua_State *L, double fill_value) {
    void *userdata = lua_newuserdata(L, sizeof(Buffer));
    Buffer *buf = userdata;
    fill_buffer(buf, fill_value);
    luaL_getmetatable(L, "Buffer");
    lua_setmetatable(L, -2);
}

static int new_buffer (lua_State *L) {
    double fill_value = 0;
    if (lua_gettop(L) == 1)
        fill_value = luaL_checknumber(L, 1);
    Buffer_new(L, fill_value);
    return 1;
}

static int buffer_peek (lua_State *L) {
    void *userdata = lua_touserdata(L, -1);
    Buffer *buf = userdata;
    lua_pushnumber(L, (*buf)[0]);
    return 1;
}
