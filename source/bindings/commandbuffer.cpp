#include "commandbuffer.hpp"


#include "../luax.hpp"
#include "../program.hpp"
#include "color.hpp"
#include "pipeline.hpp"


#define LUA_TEXTURE_USERVALUE 1


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
		// Fill metatable.
		luaL_setfuncs(lua, metatable, 0);
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -2, "__index");
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -2, "__metatable");

		// Make metatable callable.
		luaL_newlib(lua, callable);
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -2, "__metatable");
		lua_setmetatable(lua, -2);
	}

	return 1;
}


SDL_GPUCommandBuffer* lua_testcommandbuffer(lua_State* lua, int arg)
{
	if (auto ptr = luaL_testudata(lua, arg, "CommandBuffer"))
	{ return *(SDL_GPUCommandBuffer**)ptr; }
	else
	{ return nullptr; }
}


SDL_GPUCommandBuffer*& lua_checkcommandbuffer(lua_State* lua, int arg)
{
	return *lua_checkudata<SDL_GPUCommandBuffer*>(lua, arg, "CommandBuffer");
}


static int call_constructor(lua_State* lua)
{
	auto& program = *lua_getprogram(lua);

	// Create our command buffer.
	auto& commands = *lua_newudata<SDL_GPUCommandBuffer*>(lua, 1);
	commands = SDL_AcquireGPUCommandBuffer(program);
	luaL_setmetatable(lua, "CommandBuffer");
	auto commands_index = lua_gettop(lua);

	if (commands == nullptr)
	{ return luaL_error(lua, SDL_GetError()); }

	// Store our swapchain target texture for later.
	if (lua_isstring(lua, 2) && lua_tostringview(lua, 2) == "display")
	{
		SDL_GPUTexture* texture;
		if (!SDL_WaitAndAcquireGPUSwapchainTexture(commands, program, &texture, nullptr, nullptr))
		{ return luaL_error(lua, SDL_GetError()); }

		lua_pushinteger(lua, (uintptr_t)texture);
		lua_setiuservalue(lua, commands_index, LUA_TEXTURE_USERVALUE);
	}

	return 1;
}


static int call_destructor(lua_State* lua)
{
	auto& commands = lua_checkcommandbuffer(lua, 1);

	if (!SDL_SubmitGPUCommandBuffer(commands))
	{ return luaL_error(lua, SDL_GetError()); }
	commands = nullptr;

	return 0;
}


static int call_finalizer(lua_State* lua)
{
	auto& commands = lua_checkcommandbuffer(lua, 1);

	if (commands != nullptr)
	{
		lua_getiuservalue(lua, 1, LUA_TEXTURE_USERVALUE);
		auto texture = (SDL_GPUTexture*)lua_tointeger(lua, -1);
		lua_pop(lua, 1);

		// Attempt to either cancel or submit our dangling command buffer.
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

	lua_getiuservalue(lua, 1, LUA_TEXTURE_USERVALUE);
	auto texture = (SDL_GPUTexture*)lua_tointeger(lua, -1);
	lua_pop(lua, 1);

	SDL_GPUColorTargetInfo target_info{};
	if (auto color = lua_testcolor(lua, 2))
	{
		target_info = SDL_GPUColorTargetInfo
		{
			.texture = texture,
			.clear_color = *color,
			.load_op = SDL_GPU_LOADOP_CLEAR,
		};
	}
	else if (lua_isboolean(lua, 2))
	{
		if (lua_toboolean(lua, 2))
		{
			target_info = SDL_GPUColorTargetInfo
			{
				.texture = texture,
				.clear_color = SDL_FColor{},
				.load_op = SDL_GPU_LOADOP_CLEAR,
			};
		}
		else
		{
			target_info = SDL_GPUColorTargetInfo
			{
				.texture = texture,
				.load_op = SDL_GPU_LOADOP_LOAD,
			};
		}
	}
	else
	{
		target_info = SDL_GPUColorTargetInfo
		{
			.texture = texture,
			.load_op = SDL_GPU_LOADOP_DONT_CARE,
		};
	}

	auto& pass = *lua_newudata<SDL_GPURenderPass*>(lua);
	pass = SDL_BeginGPURenderPass(commands, &target_info, 1, nullptr);
	luaL_setmetatable(lua, "RenderPass");

	return 1;
}