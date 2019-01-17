#include "math.h"
#include "classkit.h"

typedef struct {
    Buffer *in, *out;
    double center, scale;
} LFO;

NF_SETTER(LFO, in,  param_buffer, 0)
NF_SETTER(LFO, out, param_buffer, 0)
NF_SETTER(LFO, center, param_double, 0)
NF_SETTER(LFO, scale,  param_double, 1)

static const NativeField LFOFields[] = {
    NF(LFO, in),
    NF(LFO, out),
    NF(LFO, center),
    NF(LFO, scale),
    {NULL,NULL, NULL},
};
CK_BOILERPLATE(LFO);

static int LFO_new(lua_State *L) {
    LFO *lfo = new_class_table(L, "LFO", sizeof(LFO)); // +table
    Actor_append(lfo, &LFO_process);
    lua_rotate(L, 1, 1); // Put table on the bottom

    // table | ...params
    constructor_param(L, 1, "in");
    constructor_param(L, 2, "center");
    constructor_param(L, 3, "scale");
    constructor_param(L, 0, "out");
    lua_settop(L, 1); // Clear any remaining junk
    return 1;
}

void LFO_process(void *raw, int length) {
    LFO *lfo = raw;
    Buffer *in  = lfo->in;
    Buffer *out = lfo->out;
    for(int i=0; i<length; i++) {
        write_sample( read_sample(in) * lfo->scale + lfo->center );
    }
}
