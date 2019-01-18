#pragma once
#define write_sample(sample) (*out)[i] = sample
#define read_sample(src) (*src)[i]
#define FREQ read_sample(osc->freq)
#define PW   read_sample(osc->pw)

#define NF(classname, name) \
    {#name, "_" #name, classname ## _set_ ## name }

#define NF_SETTER(classname, name, reader, ...) \
    void classname ## _set_ ## name (lua_State *L, void *userdata) { \
        (( classname *)userdata)-> name = reader(L, ##__VA_ARGS__); \
    }

// The golden rule of repeated sums (accumulation) in floating point?
// Don't mix small numbers with big numbers.
// Small with small, big with big.
// This design philosophy is how we get decent accuracy here.
//
// For sample_out, expect phase to be in units of _wavelengths_, such
// that phase=1 is exactly one cycle. Value may be higher than 1.
#define OSCILLATE(classname, sample_out) { \
    classname *osc = raw; \
    Buffer *out  = osc->out; \
    Buffer *f_buf = osc->freq; \
    double p=0, phase=0; \
    for(int i=0; i<length; i++) { \
        p += FREQ/sr; /* Accumulate in small variable for error control */ \
        phase = osc->phase + p; /* Make running total available */ \
        write_sample(sample_out); \
    } \
    osc->phase = fmod(phase, 1); /* Wrap around regularly, but not every sample */ \
}

#define CK_BOILERPLATE(ClassName) \
    static int ClassName ## _meta_index(lua_State *L) { \
        return native_index(L, ClassName ## Fields); \
    } \
    static int ClassName ## _meta_newindex(lua_State *L) { \
        return native_newindex(L, ClassName ## Fields); \
    } \
    static const struct luaL_Reg ClassName ## Metatable[] = { \
        {"__index", ClassName ## _meta_index}, \
        {"__newindex", ClassName ## _meta_newindex}, \
        {NULL,NULL} \
    }; \
    void ClassName ## _register(lua_State *L) { \
        luaL_newmetatable(L, #ClassName); \
        luaL_setfuncs(L, ClassName ## Metatable, 0); \
        lua_pop(L, 1); \
    } \
    static int ClassName ## _new(lua_State *L); \
    void ClassName ## _process(void *, int);
    
void constructor_param(lua_State *L, int n, const char* name) {
    n++; // Account for table at the bottom
    if (n == 1 || lua_gettop(L) < n) {
        lua_pushnil(L); // Always use nil as default value, rely on setter intelligence
    } else {
        lua_pushvalue(L, n);
    }
    // table | ... | value
    lua_setfield(L, 1, name); // -value
}

Buffer* param_buffer(lua_State *L, double def_fill) {
    // Get correct value in place
    // -in +buf
    int type = lua_type(L, -1);
    switch (type) {
        case LUA_TUSERDATA: break;
        case LUA_TNIL:
            lua_pop(L, 1); Buffer_new(L, def_fill); break;
        case LUA_TNUMBER:
        case LUA_TSTRING:
            Buffer_new(L, luaL_checknumber(L, -1)); lua_remove(L, -2); break;
        default:
            luaL_error(L, "Incorrect argument type for param_double (got %s)", lua_typename(L, type));
    }
    return lua_touserdata(L, -1);
}

double param_double(lua_State *L, double def) {
    // Get correct value in place
    // -in +buf
    int type = lua_type(L, -1);
    switch (type) {
        case LUA_TNUMBER: break;
        case LUA_TSTRING: break;
        case LUA_TNIL:
            lua_pop(L, 1); lua_pushnumber(L, def); break;
        default:
            luaL_error(L, "Incorrect argument type for param_double (got %s)", lua_typename(L, type));
    }
    return luaL_checknumber(L, -1);
}
