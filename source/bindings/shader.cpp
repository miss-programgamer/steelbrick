#include "shader.hpp"


#include <SDL3/SDL.h>
#include <SDL3_shadercross/SDL_shadercross.h>

#include "../luax.hpp"
#include "../debug.hpp"
#include "../program.hpp"


static int call_constructor(lua_State* lua);

static int call_destructor(lua_State* lua);


int luaopen_shader(lua_State* lua)
{
	static const luaL_Reg metatable[]
	{
		{ "__metatable", nullptr },
		{ "__gc", call_destructor },
		{ nullptr, nullptr },
	};

	static const luaL_Reg callable[]
	{
		{ "__call", call_constructor },
		{ nullptr, nullptr },
	};

	if (luaL_newmetatable(lua, "Shader"))
	{
		luaL_setfuncs(lua, metatable, 0);

		luaL_newlib(lua, callable);
		lua_setmetatable(lua, -2);

		return 1;
	}

	return 0;
}


SDL_GPUShader* lua_checkshader(lua_State* lua, int index)
{
	return (SDL_GPUShader*)luaL_checkudata(lua, index, "Shader");
}


static int call_constructor(lua_State* lua)
{
	auto& program = *lua_getprogram(lua);

	auto filename = luaL_checkstring(lua, 2);

	if (!lua_istable(lua, 3))
	{ return luaL_typeerror(lua, 2, "table"); }

	if (lua_getfield(lua, 3, "entry") != LUA_TSTRING)
	{ return luaL_argerror(lua, 3, "expected 'entry' field to be a string"); }
	auto entrypoint = lua_tostring(lua, 4);

	if (lua_getfield(lua, 3, "stage") != LUA_TSTRING)
	{ return luaL_argerror(lua, 3, "expected 'stage' field to be a string"); }
	auto stage_str = lua_tostring(lua, 5);

	SDL_GPUShaderStage stage;
	if (strcmp(stage_str, "vertex") == 0)
	{ stage = SDL_GPU_SHADERSTAGE_VERTEX; }
	else if (strcmp(stage_str, "fragment") == 0)
	{ stage = SDL_GPU_SHADERSTAGE_FRAGMENT; }
	else
	{ return luaL_argerror(lua, 3, lua_pushfstring(lua, "invalid shader stage '%s'", stage_str)); }

	auto source_data = SDL_LoadFile(filename, nullptr);
	auto source = (char*)source_data;

	if (source_data == nullptr)
	{ return luaL_error(lua, "%s", SDL_GetError()); }

	SDL_ShaderCross_HLSL_Info source_info
	{
		.source = source,
		.entrypoint = entrypoint,
		.shader_stage = (SDL_ShaderCross_ShaderStage)stage,
		.enable_debug = debug,
		.name = filename,
	};

	size_t code_size;
	auto code_data = SDL_ShaderCross_CompileDXILFromHLSL(&source_info, &code_size);
	auto code = (Uint8*)code_data;

	if (code_data == nullptr)
	{
		SDL_free(source_data);
		return luaL_error(lua, "%s", SDL_GetError());
	}

	SDL_GPUShaderCreateInfo code_info
	{
		.code_size = code_size,
		.code = code,
		.entrypoint = entrypoint,
		.format = SDL_GPU_SHADERFORMAT_DXIL,
		.stage = stage,
	};
	auto shader = SDL_CreateGPUShader(program, &code_info);

	SDL_free(source_data);
	SDL_free(code_data);

	lua_pushlightuserdata(lua, shader);
	lua_rotate(lua, 1, -1);
	lua_setmetatable(lua, -2);

	return 1;
}


static int call_destructor(lua_State* lua)
{
	auto& program = *lua_getprogram(lua);
	auto shader = (SDL_GPUShader*)luaL_checkudata(lua, 1, "Shader");
	SDL_ReleaseGPUShader(program, shader);
	return 0;
}