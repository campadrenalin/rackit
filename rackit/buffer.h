#pragma once
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

static const int BUFFER_SIZE = 2048;
typedef double Buffer[BUFFER_SIZE];

void fill_buffer(Buffer *buf, double value);
void Buffer_new(lua_State *L, double fill_value);
static int new_buffer (lua_State *L);
static int buffer_peek (lua_State *L);
static int buffer_fill (lua_State *L);

static const struct luaL_Reg BufferMetatable[] = {
    {"peek", buffer_peek},
    {"fill", buffer_fill},
    {NULL,NULL} // Sentinel value
};
