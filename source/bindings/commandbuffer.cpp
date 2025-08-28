#include "commandbuffer.hpp"


#include "../luax.hpp"
#include "../program.hpp"
#include "pipeline.hpp"


static int call_constructor(lua_State* lua);

static int call_destructor(lua_State* lua);

static int call_finalizer(lua_State* lua);

static int call_copypass(lua_State* lua);

static int call_renderpass(lua_State* lua);


int luaopen_commandbuffer(lua_State* lua)
{
	static const luaL_Reg metatable[]
	{
		{ "copypass",   call_copypass },
		{ "renderpass", call_renderpass },
		{ "__close",    call_destructor },
		{ "__gc",       call_finalizer },
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

	if (luaL_newmetatable(lua, "CommandBuffer"))
	{
		luaL_setfuncs(lua, metatable, 0);
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -2, "__index");
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -2, "__metatable");

		luaL_newlib(lua, callable);
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -2, "__metatable");
		lua_setmetatable(lua, -2);
	}

	return 1;
}


SDL_GPUCommandBuffer*& lua_checkcommandbuffer(lua_State* lua, int index)
{
	return *lua_checkudata<SDL_GPUCommandBuffer*>(lua, index, "CommandBuffer");
}


static int call_constructor(lua_State* lua)
{
	auto& program = *lua_getprogram(lua);

	auto& commands = *lua_newudata<SDL_GPUCommandBuffer*>(lua, 1);
	commands = SDL_AcquireGPUCommandBuffer(program);

	if (commands == nullptr)
	{ return luaL_error(lua, SDL_GetError()); }

	SDL_GPUTexture* texture;
	if (!SDL_WaitAndAcquireGPUSwapchainTexture(commands, program, &texture, nullptr, nullptr))
	{ return luaL_error(lua, SDL_GetError()); }

	lua_pushvalue(lua, 1);
	lua_setmetatable(lua, 2);

	lua_pushinteger(lua, (uintptr_t)texture);
	lua_setiuservalue(lua, 2, 1);

	return 1;
}


static int call_destructor(lua_State* lua)
{
	auto& commands = lua_checkcommandbuffer(lua, 1);

	if (!SDL_SubmitGPUCommandBuffer(commands))
	{ return luaL_error(lua, SDL_GetError()); }
	commands = nullptr;

	lua_pushnil(lua);
	lua_setiuservalue(lua, 1, 1);

	return 0;
}


static int call_finalizer(lua_State* lua)
{
	auto& commands = lua_checkcommandbuffer(lua, 1);

	if (commands != nullptr)
	{
		lua_getiuservalue(lua, 1, 1);
		auto texture = (SDL_GPUTexture*)lua_tointeger(lua, -1);
		lua_pop(lua, 1);

		if (texture != nullptr)
		{
			if (!SDL_SubmitGPUCommandBuffer(commands))
			{ return luaL_error(lua, SDL_GetError()); }
		}
		else
		{
			if (!SDL_CancelGPUCommandBuffer(commands))
			{ return luaL_error(lua, SDL_GetError()); }
		}

		commands = nullptr;

		lua_pushnil(lua);
		lua_setiuservalue(lua, 1, 1);

		return luaL_error(lua, "CommandBuffer %p was not closed properly (did you forget to add <close>)", lua_topointer(lua, 1));
	}

	return 0;
}


static int call_copypass(lua_State* lua)
{
	auto& commands = lua_checkcommandbuffer(lua, 1);

	auto& pass = *lua_newudata<SDL_GPUCopyPass*>(lua);
	pass = SDL_BeginGPUCopyPass(commands);
	luaL_setmetatable(lua, "CopyPass");

	return 1;
}


static int call_renderpass(lua_State* lua)
{
	auto& commands = lua_checkcommandbuffer(lua, 1);
	auto& pipeline = lua_checkpipeline(lua, 2);

	lua_getiuservalue(lua, 1, 1);
	auto texture = (SDL_GPUTexture*)lua_tointeger(lua, -1);
	lua_pop(lua, 1);

	SDL_GPUColorTargetInfo target_info { .texture = texture };
	auto& pass = *lua_newudata<SDL_GPURenderPass*>(lua);
	pass = SDL_BeginGPURenderPass(commands, &target_info, 1, nullptr);
	luaL_setmetatable(lua, "RenderPass");

	SDL_BindGPUGraphicsPipeline(pass, pipeline);

	return 1;
}