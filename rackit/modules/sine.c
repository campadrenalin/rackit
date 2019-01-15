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

void OscSine_set_freq(lua_State *L, void *userdata) {
    OscSine *osc = userdata;
    osc->freq = luaL_checknumber(L, -1);
}
void OscSine_set_buffer(lua_State *L, void *userdata) {
    OscSine *osc = userdata;
    osc->out = lua_touserdata(L, -1);
}

static const NativeField OscSineFields[] = {
    {"freq",   "_freq",   OscSine_set_freq},
    {"buffer", "_buffer", OscSine_set_buffer},
    {NULL,NULL}, // Sentinel value
};
static int OscSine_meta_index(lua_State *L) {
    return native_index(L, OscSineFields);
}
static int OscSine_meta_newindex(lua_State *L) {
    return native_newindex(L, OscSineFields);
}

static int OscSine_new(lua_State *L) {
    // freq? | buf?
    if (lua_gettop(L) < 1)
        lua_pushnumber(L, 440); // Default frequency: A4 (440hz)

    // freq | buf?
    if (lua_gettop(L) < 2)
        Buffer_new(L, 0); // Blank buffer if not provided

    // freq | buf
    lua_createtable(L, 0, 3); // +osc_table
    luaL_getmetatable(L, "OscSine"); // +metatable
    lua_setmetatable(L, -2); // -metatable
    // freq | buf | osc_table

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

void OscSine_register(lua_State *L) {
    luaL_newmetatable(L, "OscSine");   // +mt
    luaL_setfuncs(L, OscSineMetatable, 0);
    lua_pop(L, 1); // -mt
}
