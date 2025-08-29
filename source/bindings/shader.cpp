#include "shader.hpp"


#include <SDL3/SDL.h>
#include <SDL3_shadercross/SDL_shadercross.h>

#include "../luax.hpp"
#include "../debug.hpp"
#include "../hashmap.hpp"
#include "../program.hpp"


static int call_constructor(lua_State* lua);

static int call_finalizer(lua_State* lua);


int luaopen_shader(lua_State* lua)
{
	static const luaL_Reg metatable[]
	{
		{ "__gc", call_finalizer },
		{ "__metatable", nullptr },
		{ "__newindex", nullptr },
		{ "__index", nullptr },
		{ nullptr, nullptr },
	};

	static const luaL_Reg callable[]
	{
		{ "__call", call_constructor },
		{ "__metatable", nullptr },
		{ nullptr, nullptr },
	};

	if (luaL_newmetatable(lua, "Shader"))
	{
		// Fill metatable
		luaL_setfuncs(lua, metatable, 0);
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -2, "__index");
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -2, "__metatable");

		// Make metatable invokable
		luaL_newlib(lua, callable);
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -2, "__metatable");
		lua_setmetatable(lua, -2);
	}

	return 1;
}


SDL_GPUShader* lua_testshader(lua_State* lua, int arg)
{
	if (auto ptr = luaL_testudata(lua, arg, "Shader"))
	{ return *(SDL_GPUShader**)ptr; }
	else
	{ return nullptr; }
}


SDL_GPUShader*& lua_checkshader(lua_State* lua, int arg)
{
	return *lua_checkudata<SDL_GPUShader*>(lua, arg, "Shader");
}


static int call_constructor(lua_State* lua)
{
	static const Game::HashMap<std::string, SDL_GPUShaderStage> stages
	{
		{ "vertex",   SDL_GPU_SHADERSTAGE_VERTEX },
		{ "fragment", SDL_GPU_SHADERSTAGE_FRAGMENT },
	};

	auto& program = *lua_getprogram(lua);

	// 1) Metatable, 2) filename, 3) info table
	lua_settop(lua, 3);

	// First argument is a filename string.
	auto filename = luaL_checkstring(lua, 2);

	// Second arg field 'entry' is an entry point name.
	if (lua_getfield(lua, 3, "entry") != LUA_TSTRING)
	{ return luaL_argerror(lua, 3, "expected 'entry' field to be a string"); }
	auto entrypoint = lua_tostring(lua, 4);

	// Second arg field 'stage' is a shader stage name.
	if (lua_getfield(lua, 3, "stage") != LUA_TSTRING)
	{ return luaL_argerror(lua, 3, "expected 'stage' field to be a string"); }

	SDL_GPUShaderStage stage;
	if (auto it = stages.find(lua_tostringview(lua, 5)); it == stages.end())
	{ return luaL_argerror(lua, 3, lua_pushfstring(lua, "invalid shader stage '%s'", lua_tostringview(lua, 5))); }
	else
	{ stage = it->second; }

	// Load shader source file.
	auto source_data = SDL_LoadFile(filename, nullptr);
	auto source = (char*)source_data;

	if (source_data == nullptr)
	{ return luaL_error(lua, "%s", SDL_GetError()); }

	// Fill out information about source code
	SDL_ShaderCross_HLSL_Info source_info
	{
		.source = source,
		.entrypoint = entrypoint,
		.shader_stage = (SDL_ShaderCross_ShaderStage)stage,
		.enable_debug = debug,
		.name = filename,
	};

	// Compile shader into bytecode.
	size_t code_size;
	auto code_data = SDL_ShaderCross_CompileSPIRVFromHLSL(&source_info, &code_size);
	auto code = (Uint8*)code_data;
	SDL_free(source_data);

	if (code_data == nullptr)
	{ return luaL_error(lua, "%s", SDL_GetError()); }

	// Create a pointer to our shader.
	auto& shader = *lua_newudata<SDL_GPUShader*>(lua);
	luaL_setmetatable(lua, "Shader");

	// Fill out information about bytecode.
	auto props = SDL_CreateProperties();
	auto debug_name = lua_pushfstring(lua, "%s (%s)", filename, lua_tostring(lua, 5));
	SDL_SetStringProperty(props, SDL_PROP_GPU_SHADER_CREATE_NAME_STRING, debug_name);
	SDL_GPUShaderCreateInfo code_info
	{
		.code_size = code_size,
		.code = code,
		.entrypoint = entrypoint,
		.format = SDL_GPU_SHADERFORMAT_SPIRV,
		.stage = stage,
		.props = props,
	};

	// Create our shader.
	shader = SDL_CreateGPUShader(program, &code_info);
	SDL_DestroyProperties(props);
	SDL_free(code_data);
	lua_pop(lua, 1);

	// Return our shader.
	return 1;
}


static int call_finalizer(lua_State* lua)
{
	auto& program = *lua_getprogram(lua);

	auto shader = *lua_checkudata<SDL_GPUShader*>(lua, 1, "Shader");
	SDL_ReleaseGPUShader(program, shader);

	return 0;
}