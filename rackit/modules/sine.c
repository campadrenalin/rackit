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

void print_stack(lua_State *L) {
    int n = lua_gettop(L); //number of arguments
    SDL_Log("Printing stack (%d items)", n);
    for (int i=1; i <= n; i++) {
        int t = lua_type(L, i);
        SDL_Log("%i => <%s> %s", i, lua_typename(L, t), lua_tostring(L, i));
    }
    SDL_Log("---");
}

static int OscSine_set_freq(lua_State *L) {
    // ... | osc_table | freq
    double freq = luaL_checknumber(L, -1);
    lua_setfield(L, -2, "freq");
    // ... | osc_table

    lua_getfield(L, -1, "_userdata");
    // ... | osc_table | osc_struct
    void *userdata = lua_touserdata(L, -1);
    OscSine *osc = userdata;
    lua_pop(L, 1);
    // .. | osc_table

    osc->freq = freq; // Set in struct
    return 0;
}

static int OscSine_set_buffer(lua_State *L) {
    // ... | osc_table | buf
    Buffer *buf = lua_touserdata(L, -1);
    lua_setfield(L, -2, "buffer");
    // ... | osc_table

    lua_getfield(L, -1, "_userdata");
    // ... | osc_table | osc_struct
    OscSine *osc = lua_touserdata(L, -1);
    osc->out = buf; // Set in struct
    lua_pop(L, 1);
    // .. | osc_table
    return 0;
}

static int OscSine_new(lua_State *L) {
    // freq? | buf? |
    if (lua_gettop(L) < 1)
        lua_pushnumber(L, 440); // Default frequency: A4 (440hz)

    // freq | buf? |
    if (lua_gettop(L) < 2)
        Buffer_new(L, 0); // Blank buffer if not provided

    // freq | buf |
    lua_createtable(L, 0, 3);
    // freq | buf | osc_table
    void *userdata = lua_newuserdata(L, sizeof(OscSine));
    // freq | buf | osc_table | osc_struct
    lua_setfield(L, -2, "_userdata");
    // freq | buf | osc_table

    OscSine *osc = userdata;
    osc->phase = 0;
    osc->out = lua_touserdata(L, 2);

    // Set frequency
    lua_pushvalue(L, -3);
    // freq | buf | osc_table | freq
    OscSine_set_freq(L);
    // freq | buf | osc_table

    // Set buffer
    lua_pushvalue(L, -2);
    // freq | buf | osc_table | buf
    OscSine_set_buffer(L);
    // freq | buf | osc_table

    Actor_append(osc, &OscSine_process);
    Mixer_append(&mix_master, 1, osc->out);

    luaL_getmetatable(L, "OscSine");
    // freq | buf | osc_table | metatable
    lua_setmetatable(L, -2);
    return 1;
}

static const struct luaL_Reg OscSineMetatable[] = {
    // {"peek", buffer_peek},
    {NULL,NULL} // Sentinel value
};
