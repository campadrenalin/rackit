#pragma once
#define write_sample(sample) (*out)[i] = sample

#define GENERATE_CLASS(CLASSNAME) \
    typedef struct { FIELDS(STRUCT_FIELDS) } CLASSNAME; \
    FIELDS(NF_SETTERS) \
    static const NativeField CLASSNAME ## Fields[] = { \
        FIELDS(NF_FIELDS) \
        {NULL,NULL}, \
    }; \
    static int CLASSNAME ## _meta_index(lua_State *L) { \
        return native_index(L, CLASSNAME ## Fields); \
    } \
    static int CLASSNAME ## _meta_newindex(lua_State *L) { \
        return native_newindex(L, CLASSNAME ## Fields); \
    } \
    static const struct luaL_Reg CLASSNAME ## Metatable[] = { \
        {"__index", CLASSNAME ## _meta_index}, \
        {"__newindex", CLASSNAME ## _meta_newindex}, \
        {NULL,NULL} \
    }; \
    void CLASSNAME ## _register(lua_State *L) { \
        luaL_newmetatable(L, #CLASSNAME); \
        luaL_setfuncs(L, CLASSNAME ## Metatable, 0); \
        lua_pop(L, 1); \
    } \
    static int CLASSNAME ## _new(lua_State *L); \
    void CLASSNAME ## _process(void *, int);

#define STRUCT_FIELDS(type, name, _reader) \
    type name;

#define NF_FIELDS(_type, name, _reader) \
    {#name, "_" #name, OscSine_set_ ## name },

#define NF_SETTERS(type, name, reader) \
    void OscSine_set_ ## name (lua_State *L, void *userdata) { \
        OscSine *osc = userdata; \
        osc-> name = reader(L, -1); \
    }

#define CONSTRUCTOR_DEFAULTS() PARAMS(_CONSTRUCTOR_DEFAULTS)
#define _CONSTRUCTOR_DEFAULTS(n, _name, creator) \
    if (lua_gettop(L) < n) creator;

#define CONSTRUCTOR_APPLYS() PARAMS(_CONSTRUCTOR_APPLYS)
#define _CONSTRUCTOR_APPLYS(n, name, _creator) \
    lua_pushvalue(L, n); \
    lua_setfield(L, -2, #name);
