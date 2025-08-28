#include "pipeline.hpp"


#include <cstdint>
#include <memory>

#include <SDL3/SDL.h>

#include "../luax.hpp"
#include "../hashmap.hpp"
#include "../program.hpp"
#include "shader.hpp"
#include "renderpass.hpp"


#define LUA_VERTEX_USERVALUE 1
#define LUA_FRAGMENT_USERVALUE 2


static int call_constructor(lua_State* lua);

static int call_finalizer(lua_State* lua);

static int call_bind(lua_State* lua);


int luaopen_pipeline(lua_State* lua)
{
	static const luaL_Reg metatable[]
	{
		{ "bind", call_bind },
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

	if (luaL_newmetatable(lua, "Pipeline"))
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


SDL_GPUGraphicsPipeline* lua_testpipeline(lua_State* lua, int arg)
{
	if (auto ptr = luaL_testudata(lua, arg, "Pipeline"))
	{ return *(SDL_GPUGraphicsPipeline**)ptr; }
	else
	{ return nullptr; }
}


SDL_GPUGraphicsPipeline*& lua_checkpipeline(lua_State* lua, int arg)
{
	return *lua_checkudata<SDL_GPUGraphicsPipeline*>(lua, arg, "Pipeline");
}


static int call_constructor(lua_State* lua)
{
	static const Game::HashMap<std::string, SDL_GPUVertexElementFormat> vertex_formats
	{
		{ "int",  SDL_GPU_VERTEXELEMENTFORMAT_INT },
		{ "int2", SDL_GPU_VERTEXELEMENTFORMAT_INT2 },
		{ "int3", SDL_GPU_VERTEXELEMENTFORMAT_INT3 },
		{ "int4", SDL_GPU_VERTEXELEMENTFORMAT_INT4 },

		{ "uint",  SDL_GPU_VERTEXELEMENTFORMAT_UINT },
		{ "uint2", SDL_GPU_VERTEXELEMENTFORMAT_UINT2 },
		{ "uint3", SDL_GPU_VERTEXELEMENTFORMAT_UINT3 },
		{ "uint4", SDL_GPU_VERTEXELEMENTFORMAT_UINT4 },

		{ "float",  SDL_GPU_VERTEXELEMENTFORMAT_FLOAT },
		{ "float2", SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2 },
		{ "float3", SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3 },
		{ "float4", SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4 },

		{ "byte2", SDL_GPU_VERTEXELEMENTFORMAT_BYTE2 },
		{ "byte4", SDL_GPU_VERTEXELEMENTFORMAT_BYTE4 },

		{ "ubyte2", SDL_GPU_VERTEXELEMENTFORMAT_UBYTE2 },
		{ "ubyte4", SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4 },

		{ "byte2norm", SDL_GPU_VERTEXELEMENTFORMAT_BYTE2_NORM },
		{ "byte4norm", SDL_GPU_VERTEXELEMENTFORMAT_BYTE4_NORM },

		{ "ubyte2norm", SDL_GPU_VERTEXELEMENTFORMAT_UBYTE2_NORM },
		{ "ubyte4norm", SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM },

		{ "short2", SDL_GPU_VERTEXELEMENTFORMAT_SHORT2 },
		{ "short4", SDL_GPU_VERTEXELEMENTFORMAT_SHORT4 },

		{ "ushort2", SDL_GPU_VERTEXELEMENTFORMAT_USHORT2 },
		{ "ushort4", SDL_GPU_VERTEXELEMENTFORMAT_USHORT4 },

		{ "short2norm", SDL_GPU_VERTEXELEMENTFORMAT_SHORT2_NORM },
		{ "short4norm", SDL_GPU_VERTEXELEMENTFORMAT_SHORT4_NORM },

		{ "ushort2norm", SDL_GPU_VERTEXELEMENTFORMAT_USHORT2_NORM },
		{ "ushort4norm", SDL_GPU_VERTEXELEMENTFORMAT_USHORT4_NORM },
	};

	static const Game::HashMap<std::string, SDL_GPUPrimitiveType> primitives
	{
		{ "trianglelist",  SDL_GPU_PRIMITIVETYPE_TRIANGLELIST },
		{ "trianglestrip", SDL_GPU_PRIMITIVETYPE_TRIANGLESTRIP },
		{ "linelist",      SDL_GPU_PRIMITIVETYPE_LINELIST },
		{ "linestrip",     SDL_GPU_PRIMITIVETYPE_LINESTRIP },
		{ "pointlist",     SDL_GPU_PRIMITIVETYPE_POINTLIST },
	};

	static const Game::HashMap<std::string, SDL_GPUVertexInputRate> rates
	{
		{ "vertex",   SDL_GPU_VERTEXINPUTRATE_VERTEX, },
		{ "instance", SDL_GPU_VERTEXINPUTRATE_INSTANCE, },
	};

	auto& program = *lua_getprogram(lua);

	// 1) Metatable, 2) info table
	lua_settop(lua, 2);

	// First arg field 'vertex' is our vertex shader.
	lua_getfield(lua, 2, "vertex");
	auto vertex = lua_checkshader(lua, 3);

	// First arg field 'fragment' is our fragment shader.
	lua_getfield(lua, 2, "fragment");
	auto fragment = lua_checkshader(lua, 4);

	// First arg field 'inputs' is our input layout.
	lua_getfield(lua, 2, "inputs");
	uint32_t inputs_size = lua_getlen(lua, 5);
	auto inputs = std::make_unique<SDL_GPUVertexAttribute[]>(inputs_size);
	lua_pushnil(lua);
	for (int i = 1; lua_next(lua, 5); ++i)
	{
		auto& input = inputs[i - 1];
		auto top = lua_gettop(lua);

		if (lua_getfield(lua, top, "location") != LUA_TNUMBER)
		{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'location' field of 'inputs' subtable #%d to be a number, was %s", i, luaL_typename(lua, top + 1))); }
		input.location = lua_tointeger(lua, top + 1);

		if (lua_getfield(lua, top, "buffer") != LUA_TNUMBER)
		{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'buffer' field of 'inputs' subtable #%d to be a number, was %s", i, luaL_typename(lua, top + 2))); }
		input.buffer_slot = lua_tointeger(lua, top + 2);

		if (lua_getfield(lua, top, "format") != LUA_TSTRING)
		{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'format' field of 'inputs' subtable #%d to be a string, was %s", i, luaL_typename(lua, top + 3))); }

		if (auto it = vertex_formats.find(lua_tostringview(lua, top + 3)); it == vertex_formats.end())
		{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'format' field of 'inputs' subtable #%d to be a valid vertex format, was %s", i, lua_tostringview(lua, top + 3))); }
		else
		{ input.format = it->second; }

		if (lua_getfield(lua, top, "offset") != LUA_TNUMBER)
		{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'offset' field of 'inputs' subtable #%d to be a number, was %s", i, luaL_typename(lua, top + 4))); }
		input.offset = lua_tointeger(lua, top + 4);

		lua_settop(lua, top);
		lua_pop(lua, 1);
	}

	// First arg field 'buffers' is our buffer descriptors.
	lua_getfield(lua, 2, "buffers");
	uint32_t buffers_size = lua_getlen(lua, 6);
	auto buffers = std::make_unique<SDL_GPUVertexBufferDescription[]>(buffers_size);
	lua_pushnil(lua);
	for (int i = 1; lua_next(lua, 6); ++i)
	{
		auto& buffer = buffers[i - 1];
		auto top = lua_gettop(lua);

		if (lua_getfield(lua, top, "slot") != LUA_TNUMBER)
		{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'slot' field of 'buffers' subtable #%d to be a number, was %s", i, luaL_typename(lua, top + 1))); }
		buffer.slot = lua_tointeger(lua, top + 1);

		if (lua_getfield(lua, top, "pitch") != LUA_TNUMBER)
		{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'pitch' field of 'buffers' subtable #%d to be a number, was %s", i, luaL_typename(lua, top + 1))); }
		buffer.pitch = lua_tointeger(lua, top + 1);

		if (lua_getfield(lua, top, "rate") != LUA_TSTRING)
		{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'rate' field of 'buffers' subtable #%d to be a string, was %s", i, luaL_typename(lua, top + 3))); }

		if (auto it = rates.find(lua_tostringview(lua, top + 3)); it == rates.end())
		{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'rate' field of 'buffers' subtable #%d to be either 'vertex' or 'instance', was %s", i, lua_tostringview(lua, top + 3))); }
		else
		{ buffer.input_rate = it->second; }

		lua_settop(lua, top);
		lua_pop(lua, 1);
	}

	// First arg field 'primitives' is our type of primitive to draw.
	lua_getfield(lua, 2, "primitive");
	if (lua_type(lua, 7) != LUA_TSTRING)
	{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'primitive' field to be a string, was %s", luaL_typename(lua, 7))); }

	SDL_GPUPrimitiveType primitive;
	if (auto it = primitives.find(lua_tostringview(lua, 7)); it == primitives.end())
	{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'primitive' field to be a valid primitive type, was %s", lua_tostringview(lua, 7))); }
	else
	{ primitive = it->second; }

	// First arg field 'targets' is our render targets.
	lua_getfield(lua, 2, "targets");
	uint32_t targets_size = lua_getlen(lua, 8);
	auto targets = std::make_unique<SDL_GPUColorTargetDescription[]>(targets_size);
	lua_pushnil(lua);
	for (int i = 1; lua_next(lua, 8); ++i)
	{
		auto& target = targets[i - 1];
		auto top = lua_gettop(lua);

		if (lua_getfield(lua, top, "format") != LUA_TSTRING)
		{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'format' field of 'targets' subtable #%d to be a string, was %s", i, luaL_typename(lua, top + 3))); }

		// render target format "display" means draw to the screen directly.
		if (lua_tostringview(lua, top + 1) == "display")
		{ target.format = SDL_GetGPUSwapchainTextureFormat(program, program); }
		else
		{ return luaL_argerror(lua, 1, lua_pushfstring(lua, "expected 'format' field of 'targets' subtable $%d to be a valid texture format, was %s", i, lua_tostring(lua, top + 1))); }

		lua_settop(lua, top);
		lua_pop(lua, 1);
	}

	// Fill out our pipeline creation information.
	SDL_GPUGraphicsPipelineCreateInfo info
	{
		.vertex_shader = vertex,
		.fragment_shader = fragment,
		.vertex_input_state = SDL_GPUVertexInputState
		{
			.vertex_buffer_descriptions = buffers.get(),
			.num_vertex_buffers = buffers_size,
			.vertex_attributes = inputs.get(),
			.num_vertex_attributes = inputs_size,
		}, 
		.primitive_type = primitive,
		.rasterizer_state = SDL_GPURasterizerState
		{
			.fill_mode = SDL_GPU_FILLMODE_FILL,
			.cull_mode = SDL_GPU_CULLMODE_BACK,
			.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
		},
		.multisample_state = SDL_GPUMultisampleState
		{
			.sample_count = SDL_GPU_SAMPLECOUNT_1,
		},
		.depth_stencil_state = SDL_GPUDepthStencilState
		{},
		.target_info = SDL_GPUGraphicsPipelineTargetInfo
		{
			.color_target_descriptions = targets.get(),
			.num_color_targets = targets_size,
		},
	};

	// Create our graphics pipeline.
	auto& pipeline = *lua_newudata<SDL_GPUGraphicsPipeline*>(lua, 2);
	pipeline = SDL_CreateGPUGraphicsPipeline(program, &info);
	luaL_setmetatable(lua, "Pipeline");

	if (pipeline == nullptr)
	{ return luaL_error(lua, "%s", SDL_GetError()); }

	// Prevent our shaders from getting garbage-collected.
	lua_pushvalue(lua, 3);
	lua_setiuservalue(lua, -2, LUA_VERTEX_USERVALUE);

	lua_pushvalue(lua, 4);
	lua_setiuservalue(lua, -2, LUA_FRAGMENT_USERVALUE);

	// Return our graphics pipeline.
	return 1;
}


static int call_finalizer(lua_State* lua)
{
	auto& program = *lua_getprogram(lua);

	// Destroy our graphics pipeline.
	auto pipeline = lua_checkpipeline(lua, 1);
	SDL_ReleaseGPUGraphicsPipeline(program, pipeline);

	// Allow our shaders to get garbage-collected.
	lua_pushnil(lua);
	lua_setiuservalue(lua, 1, LUA_VERTEX_USERVALUE);

	lua_pushnil(lua);
	lua_setiuservalue(lua, 1, LUA_FRAGMENT_USERVALUE);

	return 0;
}


static int call_bind(lua_State* lua)
{
	auto& pipeline = lua_checkpipeline(lua, 1);
	auto& pass = lua_checkrenderpass(lua, 2);

	SDL_BindGPUGraphicsPipeline(pass, pipeline);

	return 0;
}