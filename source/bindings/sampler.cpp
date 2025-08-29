#include "sampler.hpp"


#include "../luax.hpp"
#include "../hashmap.hpp"
#include "../program.hpp"


static int call_constructor(lua_State* lua);

static int call_finalizer(lua_State* lua);


int luaopen_sampler(lua_State* lua)
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

	if (luaL_newmetatable(lua, "Sampler"))
	{
		// Fill metatable.
		luaL_setfuncs(lua, metatable, 0);
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -1, "__index");
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -1, "__metatable");

		// Make metatable callable.
		luaL_newlib(lua, callable);
		lua_setmetatable(lua, -2);
	}

	return 1;
}


SDL_GPUSampler* lua_testsampler(lua_State* lua, int arg)
{
	if (auto ptr = luaL_testudata(lua, arg, "Sampler"))
	{ return *(SDL_GPUSampler**)ptr; }
	else
	{ return nullptr; }
}


SDL_GPUSampler*& lua_checksampler(lua_State* lua, int arg)
{
	return *lua_checkudata<SDL_GPUSampler*>(lua, arg, "Sampler");
}


static int call_constructor(lua_State* lua)
{
	static const Game::HashMap<std::string, SDL_GPUFilter> filters
	{
		{ "nearest", SDL_GPU_FILTER_NEAREST },
		{ "linear",  SDL_GPU_FILTER_LINEAR },
	};

	static const Game::HashMap<std::string, SDL_GPUSamplerMipmapMode> mipmap_modes
	{
		{ "nearest", SDL_GPU_SAMPLERMIPMAPMODE_NEAREST },
		{ "linear",  SDL_GPU_SAMPLERMIPMAPMODE_LINEAR },
	};

	static const Game::HashMap<std::string, SDL_GPUSamplerAddressMode> address_modes
	{
		{ "repeat", SDL_GPU_SAMPLERADDRESSMODE_REPEAT },
		{ "mirror", SDL_GPU_SAMPLERADDRESSMODE_MIRRORED_REPEAT },
		{ "clamp",  SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE },
	};

	enum stack_indices : int
	{
		meta_index   = 1,
		info_index   = 2,

		min_index    = 3,
		mag_index    = 4,
		mipmap_index = 5,
		umode_index  = 6,
		vmode_index  = 7,
		wmode_index  = 8,
		aniso_index  = 9,
	};

	auto& program = *lua_getprogram(lua);

	// 1) Metatable, 2) info table
	lua_settop(lua, 2);

	// Initialize sampler info.
	SDL_GPUSamplerCreateInfo info{};

	// First arg field 'min' is our minification function.
	if (lua_getfield(lua, info_index, "min") != LUA_TSTRING)
	{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'min' field to be a string, was %s", luaL_typename(lua, min_index))); }

	if (auto it = filters.find(lua_tostringview(lua, min_index)); it == filters.end())
	{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'min' field to be a valid filter function, was %s", lua_tostring(lua, min_index))); }
	else
	{ info.min_filter = it->second; }

	// First arg field 'mag' is our magnification function.
	if (lua_getfield(lua, info_index, "mag") != LUA_TSTRING)
	{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'mag' field to be a string, was %s", luaL_typename(lua, mag_index))); }

	if (auto it = filters.find(lua_tostringview(lua, mag_index)); it == filters.end())
	{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'mag' field to be a valid filter function, was %s", lua_tostring(lua, mag_index))); }
	else
	{ info.mag_filter = it->second; }

	// First arg field 'mipmap' is our mipmap mode function.
	if (lua_getfield(lua, info_index, "mipmap") != LUA_TSTRING)
	{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'mipmap' field to be a string, was %s", luaL_typename(lua, mipmap_index))); }

	if (auto it = mipmap_modes.find(lua_tostringview(lua, mipmap_index)); it == mipmap_modes.end())
	{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'mipmap' field to be a valid filter function, was %s", lua_tostring(lua, mipmap_index))); }
	else
	{ info.mipmap_mode = it->second; }

	// First arg field 'umode' is our magnification function.
	if (lua_getfield(lua, info_index, "umode") != LUA_TSTRING)
	{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'umode' field to be a string, was %s", luaL_typename(lua, umode_index))); }

	if (auto it = address_modes.find(lua_tostringview(lua, umode_index)); it == address_modes.end())
	{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'umode' field to be a valid address mode, was %s", lua_tostring(lua, umode_index))); }
	else
	{ info.address_mode_u = it->second; }

	// First arg field 'vmode' is our magnification function.
	if (lua_getfield(lua, info_index, "vmode") != LUA_TSTRING)
	{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'vmode' field to be a string, was %s", luaL_typename(lua, vmode_index))); }

	if (auto it = address_modes.find(lua_tostringview(lua, vmode_index)); it == address_modes.end())
	{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'vmode' field to be a valid address mode, was %s", lua_tostring(lua, vmode_index))); }
	else
	{ info.address_mode_v = it->second; }

	// First arg field 'wmode' is our magnification function.
	if (lua_getfield(lua, info_index, "wmode") != LUA_TSTRING)
	{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'wmode' field to be a string, was %s", luaL_typename(lua, wmode_index))); }

	if (auto it = address_modes.find(lua_tostringview(lua, wmode_index)); it == address_modes.end())
	{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'wmode' field to be a valid address mode, was %s", lua_tostring(lua, wmode_index))); }
	else
	{ info.address_mode_w = it->second; }

	// First arg field 'anisotropy' is our flag to enable anisotropic filtering.
	if (auto type = lua_getfield(lua, info_index, "anisotropy"); type != LUA_TBOOLEAN && type != LUA_TNIL)
	{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'anisotropy' field to be a boolean or nil, was %s", luaL_typename(lua, aniso_index))); }
	info.enable_anisotropy = luaL_opt(lua, lua_toboolean, aniso_index, false);
	info.max_anisotropy = info.enable_anisotropy ? 16 : 0;

	// Create our sampler.
	auto& sampler = *lua_newudata<SDL_GPUSampler*>(lua, 0);
	sampler = SDL_CreateGPUSampler(program, &info);
	luaL_setmetatable(lua, "Sampler");

	if (sampler == nullptr)
	{ return luaL_error(lua, "%s", SDL_GetError()); }

	// Return our sampler.
	return 1;
}


static int call_finalizer(lua_State* lua)
{
	auto& program = *lua_getprogram(lua);

	auto sampler = lua_checksampler(lua, 1);
	SDL_ReleaseGPUSampler(program, sampler);

	return 0;
}