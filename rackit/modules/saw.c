#include "math.h"
#include "classkit.h"

typedef struct {
    double phase;
    Buffer *freq, *out;
} OscSaw;

NF_SETTER(OscSaw, phase,  param_double, 0)
NF_SETTER(OscSaw, freq, param_buffer, 440)
NF_SETTER(OscSaw, out,  param_buffer, 0)

static const NativeField OscSawFields[] = {
    NF(OscSaw, phase),
    NF(OscSaw, freq),
    NF(OscSaw, out),
    {NULL,NULL, NULL},
};
CK_BOILERPLATE(OscSaw);

static int OscSaw_new(lua_State *L) {
    OscSaw *osc = new_class_table(L, "OscSaw", sizeof(OscSaw)); // +table
    Actor_append(osc, &OscSaw_process);
    lua_rotate(L, 1, 1); // Put table on the bottom

    // table | ...params
    constructor_param(L, 0, "phase");
    constructor_param(L, 1, "freq");
    constructor_param(L, 2, "out");
    lua_settop(L, 1); // Clear any remaining junk
    return 1;
}

void OscSaw_process(void *raw, int length) OSCILLATE(OscSaw, fmod(phase, 1)*2 - 1)
