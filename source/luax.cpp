#include "luax.hpp"


lua_Integer lua_getlen(lua_State* lua, int index)
{
	lua_len(lua, index);
	auto len = lua_tointeger(lua, -1);
	lua_pop(lua, 1);
	return len;
}


std::string_view lua_tostringview(lua_State* lua, int index)
{
	size_t length;
	const char* str = lua_tolstring(lua, index, &length);
	return std::string_view(str, length);
}


std::string_view lua_tostringviewex(lua_State* lua, int index)
{
	size_t length;
	const char* str = luaL_tolstring(lua, index, &length);
	return std::string_view(str, length);
}