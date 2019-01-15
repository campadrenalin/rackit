#include "math.h"
#include "classkit.h"

#define FIELDS(ACTION) \
    ACTION(double, phase, luaL_checknumber) \
    ACTION(double, freq,  luaL_checknumber) \
    ACTION(Buffer*, out,  lua_touserdata)

GENERATE_CLASS(OscSine)

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
    lua_setfield(L, -2, "out"); // -buf

    Actor_append(osc, &OscSine_process);
    Mixer_append(&mix_master, 1, osc->out);

    return 1;
}

void OscSine_process(void *raw_osc, int length) {
    OscSine *osc = raw_osc;
    Buffer *out = osc->out;
    double p = 0;
    double f = osc->freq*TAU;
    for(int i=0; i<length; i++) {
        double t = (double)i/sr;
        p = t*f;
        write_sample(sin(osc->phase + p));
    }
    osc->phase += p;
    // SDL_Log("Phase: %f", osc->phase);
}
