#include "program.hpp"
using namespace Game;


#include <exception>

#include "luax.hpp"
#include "debug.hpp"
#include "bindings.hpp"


Program::Program(int argc, char** argv)
{
	const auto& flags = SDL_WINDOW_HIDDEN|SDL_WINDOW_RESIZABLE;
	window = SDL_CreateWindow("SteelBrick", 800, 600, flags);

	if (window == nullptr)
	{ throw std::exception(SDL_GetError()); }

	device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_DXIL, debug, nullptr);

	if (device == nullptr)
	{
		auto what = SDL_GetError();
		SDL_DestroyWindow(window);
		throw std::exception(what);
	}

	if (!SDL_ClaimWindowForGPUDevice(device, window))
	{ SDL_LogError(0, "%s", SDL_GetError); }

	if (!SDL_ShowWindow(window))
	{ SDL_LogError(0, "%s", SDL_GetError); }

	lua = luaL_newstate();
	lua_getprogram(lua) = this;
	luaL_openlibs(lua);
	lua_openbindings(lua);

	auto traceback = lua_pushtraceback(lua);
	if (luaL_loadfile(lua, "assets/scripts/main.lua") == LUA_OK)
	{
		if (lua_pcall(lua, 0, 0, traceback) != LUA_OK)
		{ SDL_LogError(0, "Lua: %s", lua_tostring(lua, -1)); }
	}

	lua_settop(lua, 0);

	time = SDL_GetPerformanceCounter();
}


Program::~Program()
{
	if (lua)    { lua_close(lua); }
	if (device) { SDL_DestroyGPUDevice(device); }
	if (window) { SDL_DestroyWindow(window); }
}


SDL_AppResult Program::Update()
{
	auto prev = time;
	time = SDL_GetPerformanceCounter();
	double delta = double(time - prev) / double(SDL_GetPerformanceFrequency());

	lua_settop(lua, 0);

	auto traceback = lua_pushtraceback(lua);
	if (lua_getglobal(lua, "draw") == LUA_TFUNCTION)
	{
		lua_pushnumber(lua, delta);
		if (lua_pcall(lua, 1, 0, traceback) != LUA_OK)
		{ SDL_LogError(0, "Lua: %s", lua_tostring(lua, -1)); }
	}

	return SDL_APP_CONTINUE;
}


SDL_AppResult Program::Handle(const SDL_Event& event)
{
	switch (event.type)
	{
		case SDL_EVENT_QUIT:
			return Handle(event.quit);

		default:
			return SDL_APP_CONTINUE;
	}
}


SDL_AppResult Program::Handle(const SDL_QuitEvent& quit)
{
	if (!SDL_HideWindow(window))
	{ SDL_LogError(0, "%s", SDL_GetError); }

	lua_gc(lua, LUA_GCCOLLECT);

	return SDL_APP_SUCCESS;
}