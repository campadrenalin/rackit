#include "math.h"

typedef struct {
    double phase;
    double freq;
    Buffer *out;
} OscSine;

void OscSine_process(void *raw_osc, int length) {
    OscSine *osc = raw_osc;
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

static int OscSine_meta_newindex(lua_State *L) {
    // osc_table | index | value
    OscSine *osc = get_table_userdata(L, -3);
    const char *index = luaL_checkstring(L, -2);

    if (strncmp(index, "freq", 4) == 0) {
        osc->freq = luaL_checknumber(L, -1); // Set in struct
        lua_setfield(L, -3, "_freq"); // -value
    } else if (strncmp(index, "buffer", 6) == 0) {
        osc->out = lua_touserdata(L, -1); // Set in struct
        lua_setfield(L, -3, "_buffer"); // -value
    } else {
        lua_rawset(L, -3);
    }

    return 0;
}

static int OscSine_meta_index(lua_State *L) {
    // osc_table | index
    const char *index = luaL_checkstring(L, 2);
    if (strncmp(index, "freq", 4) == 0) {
        lua_getfield(L, -2, "_freq");
    } else if (strncmp(index, "buffer", 6) == 0) {
        lua_getfield(L, -2, "_buffer");
    } else {
        lua_rawget(L, 1);
    }
    return 1;
}

static int OscSine_new(lua_State *L) {
    // freq? | buf?
    if (lua_gettop(L) < 1)
        lua_pushnumber(L, 440); // Default frequency: A4 (440hz)

    // freq | buf?
    if (lua_gettop(L) < 2)
        Buffer_new(L, 0); // Blank buffer if not provided

    // freq | buf
    lua_createtable(L, 0, 3);
    // freq | buf | osc_table
    luaL_getmetatable(L, "OscSine"); // +metatable
    lua_setmetatable(L, -2); // -metatable

    OscSine *osc = lua_newuserdata(L, sizeof(OscSine)); // +osc_struct
    lua_setfield(L, -2, "_userdata"); // -osc_struct
    osc->phase = 0;

    // Set frequency
    lua_pushvalue(L, -3); // +freq
    lua_setfield(L, -2, "freq"); // -freq

    // Set buffer
    lua_pushvalue(L, -2); // +buf
    lua_setfield(L, -2, "buffer"); // -buf

    Actor_append(osc, &OscSine_process);
    Mixer_append(&mix_master, 1, osc->out);

    return 1;
}

static const struct luaL_Reg OscSineMetatable[] = {
    {"__index", OscSine_meta_index},
    {"__newindex", OscSine_meta_newindex},
    {NULL,NULL} // Sentinel value
};
