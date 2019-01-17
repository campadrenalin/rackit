#include "math.h"
#include "classkit.h"

#define FIELDS(ACTION) \
    ACTION(double, phase, luaL_checknumber) \
    ACTION(Buffer*, freq, lua_touserdata) \
    ACTION(Buffer*, out,  lua_touserdata)

#define PARAMS(ACTION) \
    ACTION(1, freq,  lua_pushnumber(L, 440)) \
    ACTION(2, out,   Buffer_new(L, 0)) \
    ACTION(3, phase, lua_pushnumber(L, 0))

GENERATE_CLASS(OscSine)

static int OscSine_new(lua_State *L) {
    CONSTRUCTOR_DEFAULTS()
    make_buffer(L, 1);
    OscSine *osc = new_class_table(L, "OscSine", sizeof(OscSine));
    CONSTRUCTOR_APPLYS()
    Actor_append(osc, &OscSine_process);
    // Mixer_append(&mix_master, 1, osc->out);
    return 1;
}

void OscSine_process(void *raw, int length) OSCILLATE(OscSine, sin(phase*TAU))
