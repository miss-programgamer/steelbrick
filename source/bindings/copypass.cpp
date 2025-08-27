#include "copypass.hpp"


#include "../luax.hpp"
#include "commandbuffer.hpp"


static int call_destructor(lua_State* lua);

static int call_finalizer(lua_State* lua);


int luaopen_copypass(lua_State* lua)
{
	static const luaL_Reg metatable[]
	{
		{ "__close", call_destructor },
		{ "__gc",    call_finalizer },
		{ "__metatable", nullptr },
		{ "__newindex", nullptr },
		{ "__index", nullptr },
		{ nullptr, nullptr },
	};

	if (luaL_newmetatable(lua, "CopyPass"))
	{
		luaL_setfuncs(lua, metatable, 0);
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -2, "__metatable");
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -2, "__index");
	}

	return 1;
}


SDL_GPUCopyPass*& lua_checkcopypass(lua_State* lua, int index)
{
	return *lua_checkudata<SDL_GPUCopyPass*>(lua, index, "CopyPass");
}


static int call_destructor(lua_State* lua)
{
	auto& pass = lua_checkcopypass(lua, 1);

	SDL_EndGPUCopyPass(pass);
	pass = nullptr;

	return 0;
}


static int call_finalizer(lua_State* lua)
{
	auto& pass = lua_checkcopypass(lua, 1);

	if (pass != nullptr)
	{
		SDL_EndGPUCopyPass(pass);
		pass = nullptr;
		return luaL_error(lua, "CopyPass %p was not closed properly (did you forget to add <close>)", lua_topointer(lua, 1));
	}

	return 0;
}