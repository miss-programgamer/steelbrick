#ifndef GAME_LUAX_HEADER
#define GAME_LUAX_HEADER


#include <string_view>

#include <lua.hpp>


lua_Integer lua_getlen(lua_State* lua, int index);

// equivalent to `lua_tolstring`.
std::string_view lua_tostringview(lua_State* lua, int index);

// equivalent to `luaL_tolstring` (handles `__tostring` metamethod).
std::string_view lua_tostringviewex(lua_State* lua, int index);

int lua_pushtraceback(lua_State* lua);

template<typename T>
T* lua_checkudata(lua_State* lua, int index, const char* name)
{ return (T*)luaL_checkudata(lua, index, name); }


#endif // GAME_LUAX_HEADER