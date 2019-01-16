#pragma once
#define write_sample(sample) (*out)[i] = sample
#define read_sample(src) (*src)[i]

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

#define NF(classname, name) \
    {#name, "_" #name, classname ## _set_ ## name }

#define NF_SETTERS(type, name, reader) \
    void OscSine_set_ ## name (lua_State *L, void *userdata) { \
        (( OscSine *)userdata)-> name = reader(L, -1); \
    }

#define NF_SETTER(classname, name, reader, ...) \
    void classname ## _set_ ## name (lua_State *L, void *userdata) { \
        (( classname *)userdata)-> name = reader(L, ##__VA_ARGS__); \
    }

#define CONSTRUCTOR_DEFAULTS() PARAMS(_CONSTRUCTOR_DEFAULTS)
#define _CONSTRUCTOR_DEFAULTS(n, _name, creator) \
    if (lua_gettop(L) < n) creator;

#define CONSTRUCTOR_APPLYS() PARAMS(_CONSTRUCTOR_APPLYS)
#define _CONSTRUCTOR_APPLYS(n, name, _creator) \
    lua_pushvalue(L, n); \
    lua_setfield(L, -2, #name);

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

Buffer* param_buffer(lua_State *L) {
    // Get correct value in place
    // -in +buf
    int type = lua_type(L, -1);
    switch (type) {
        case LUA_TUSERDATA: break;
        case LUA_TNIL:
            lua_pop(L, 1); Buffer_new(L, 0); break;
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
