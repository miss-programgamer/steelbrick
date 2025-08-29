#include "texture.hpp"


#include "../luax.hpp"
#include "../program.hpp"


#define LUA_WIDTH_USERVALUE 1
#define LUA_HEIGHT_USERVALUE 2


static int call_constructor(lua_State* lua);

static int call_finalizer(lua_State* lua);


int luaopen_texture(lua_State* lua)
{
	static const luaL_Reg metatable[]
	{
		{ "__gc", call_finalizer },
		{ "__metatable", nullptr },
		{ "__index", nullptr },
		{ nullptr, nullptr },
	};

	static const luaL_Reg callable[]
	{
		{ "__call", call_constructor },
		{ nullptr, nullptr },
	};

	if (luaL_newmetatable(lua, "Texture"))
	{
		// Fill metatable
		luaL_setfuncs(lua, metatable, 0);
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -2, "__index");
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -2, "__metatable");

		// Make metatable callable
		luaL_newlib(lua, callable);
		lua_setmetatable(lua, -2);
	}

	return 1;
}


SDL_GPUTexture* lua_testtexture(lua_State* lua, int arg)
{
	if (auto ptr = luaL_testudata(lua, arg, "Texture"))
	{ return *(SDL_GPUTexture**)ptr; }
	else
	{ return nullptr; }
}


SDL_GPUTexture*& lua_checktexture(lua_State* lua, int arg)
{
	return *lua_checkudata<SDL_GPUTexture*>(lua, arg, "Texture");
}


Uint32 lua_gettexturewidth(lua_State* lua, int index)
{
	lua_getiuservalue(lua, index, LUA_WIDTH_USERVALUE);
	auto width = lua_tointeger(lua, -1);
	lua_pop(lua, 1);
	return width;
}


Uint32 lua_gettextureheight(lua_State* lua, int index)
{
	lua_getiuservalue(lua, index, LUA_HEIGHT_USERVALUE);
	auto width = lua_tointeger(lua, -1);
	lua_pop(lua, 1);
	return width;
}


static int call_constructor(lua_State* lua)
{
	auto& program = *lua_getprogram(lua);

	auto width = (Uint32)luaL_checkinteger(lua, 2);
	auto height = (Uint32)luaL_checkinteger(lua, 3);

	lua_settop(lua, 3);

	auto& texture = *lua_newudata<SDL_GPUTexture*>(lua, 2);
	luaL_setmetatable(lua, "Texture");
	auto texture_index = lua_gettop(lua);

	SDL_GPUTextureCreateInfo info
	{
		.type = SDL_GPU_TEXTURETYPE_2D,
		.format = SDL_GPU_TEXTUREFORMAT_R32G32B32A32_FLOAT,
		.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
		.width = width,
		.height = height,
		.layer_count_or_depth = 1,
		.num_levels = 1,
	};
	texture = SDL_CreateGPUTexture(program, &info);
	if (texture == nullptr)
	{ return luaL_error(lua, "%s", SDL_GetError()); }

	lua_pushinteger(lua, width);
	lua_setiuservalue(lua, texture_index, LUA_WIDTH_USERVALUE);
	lua_pushinteger(lua, height);
	lua_setiuservalue(lua, texture_index, LUA_HEIGHT_USERVALUE);

	return 1;
}


static int call_finalizer(lua_State* lua)
{
	auto& program = *lua_getprogram(lua);

	auto& texture = lua_checktexture(lua, 1);
	SDL_ReleaseGPUTexture(program, texture);

	return 0;
}