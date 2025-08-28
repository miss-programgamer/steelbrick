#include "luax.hpp"


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


std::string_view lua_checkstringview(lua_State* lua, int arg)
{
	size_t length;
	const char* str = luaL_checklstring(lua, arg, &length);
	return std::string_view(str, length);
}


lua_Integer lua_getlen(lua_State* lua, int index)
{
	lua_len(lua, index);
	auto len = lua_tointeger(lua, -1);
	lua_pop(lua, 1);
	return len;
}


int lua_pushtraceback(lua_State* lua)
{
	if (lua_getfield(lua, LUA_REGISTRYINDEX, LUA_LOADED_TABLE) != LUA_TTABLE)
	{ lua_pop(lua, 1); return 0; }

	if (lua_getfield(lua, -1, "debug") != LUA_TTABLE)
	{ lua_pop(lua, 2); return 0; }

	if (lua_getfield(lua, -1, "traceback") != LUA_TFUNCTION)
	{ lua_pop(lua, 3); return 0; }

	lua_copy(lua, -1, -3);
	lua_pop(lua, 2);

	return lua_gettop(lua);
}