#include "math.h"
#include "classkit.h"

typedef struct {
    double phase;
    Buffer *freq, *out;
} OscSine;

NF_SETTER(OscSine, phase,  param_double, 0)
NF_SETTER(OscSine, freq, param_buffer, 440)
NF_SETTER(OscSine, out,  param_buffer, 0)

static const NativeField OscSineFields[] = {
    NF(OscSine, phase),
    NF(OscSine, freq),
    NF(OscSine, out),
    {NULL,NULL, NULL},
};
CK_BOILERPLATE(OscSine);

static int OscSine_new(lua_State *L) {
    OscSine *osc = new_class_table(L, "OscSine", sizeof(OscSine)); // +table
    Actor_append(osc, &OscSine_process);
    lua_rotate(L, 1, 1); // Put table on the bottom

    // table | ...params
    constructor_param(L, 0, "phase");
    constructor_param(L, 1, "freq");
    constructor_param(L, 2, "out");
    lua_settop(L, 1); // Clear any remaining junk
    return 1;
}

void OscSine_process(void *raw, int length) OSCILLATE(OscSine, sin(phase*TAU))
