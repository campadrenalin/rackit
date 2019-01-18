#include "math.h"
#include "classkit.h"

typedef struct {
    double phase;
    Buffer *freq, *pw, *out;
} OscSquare;

NF_SETTER(OscSquare, phase,  param_double, 0)
NF_SETTER(OscSquare, freq, param_buffer, 440)
NF_SETTER(OscSquare, pw,   param_buffer, 0.5)
NF_SETTER(OscSquare, out,  param_buffer, 0)

static const NativeField OscSquareFields[] = {
    NF(OscSquare, phase),
    NF(OscSquare, freq),
    NF(OscSquare, pw),
    NF(OscSquare, out),
    {NULL,NULL, NULL},
};
CK_BOILERPLATE(OscSquare);

static int OscSquare_new(lua_State *L) {
    OscSquare *osc = new_class_table(L, "OscSquare", sizeof(OscSquare)); // +table
    Actor_append(osc, &OscSquare_process);
    lua_rotate(L, 1, 1); // Put table on the bottom

    // table | ...params
    constructor_param(L, 0, "phase");
    constructor_param(L, 1, "freq");
    constructor_param(L, 2, "out");
    constructor_param(L, 3, "pw");
    lua_settop(L, 1); // Clear any remaining junk
    return 1;
}

void OscSquare_process(void *raw, int length) OSCILLATE(OscSquare, (fmod(phase, 1)>PW)*2-1)
