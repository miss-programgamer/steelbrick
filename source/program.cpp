#include "program.hpp"
using namespace Game;


#include <exception>

#include "luax.hpp"
#include "debug.hpp"
#include "bindings.hpp"


Program::Program(int argc, char** argv)
{
	// Create our main window with our desired flags.
	static const auto& flags = SDL_WINDOW_HIDDEN|SDL_WINDOW_RESIZABLE;
	window = SDL_CreateWindow("SteelBrick", 800, 600, flags);

	if (window == nullptr)
	{ throw std::exception(SDL_GetError()); }

	for (int i = 0; i < SDL_GetNumGPUDrivers(); ++i)
	{ SDL_LogInfo(0, "Available graphics driver: %s", SDL_GetGPUDriver(i)); }

	// Create a handle to our GPU device.
	static const auto& formats = SDL_GPU_SHADERFORMAT_DXIL|SDL_GPU_SHADERFORMAT_SPIRV;
	device = SDL_CreateGPUDevice(formats, debug, "vulkan");

	if (device == nullptr)
	{
		auto what = SDL_GetError();
		SDL_DestroyWindow(window);
		throw std::exception(what);
	}

	// Ensure our GPU device can draw to our window.
	if (!SDL_ClaimWindowForGPUDevice(device, window))
	{ SDL_LogError(0, "%s", SDL_GetError); }

	// Initialize our Lua state.
	lua = luaL_newstate();
	lua_getprogram(lua) = this;
	luaL_openlibs(lua);
	lua_openbindings(lua);

	// Load & run our main script.
	auto traceback = lua_pushtraceback(lua);
	if (luaL_loadfile(lua, "assets/scripts/main.lua") == LUA_OK)
	{
		if (lua_pcall(lua, 0, 0, traceback) != LUA_OK)
		{ SDL_LogError(0, "Lua: %s", lua_tostring(lua, -1)); }
	}
	lua_settop(lua, 0);

	// Show our window once we're done initializing.
	if (!SDL_ShowWindow(window))
	{ SDL_LogError(0, "%s", SDL_GetError); }

	// Start measuring time.
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
	// Calculate elapsed time since last update.
	auto prev = time;
	time = SDL_GetPerformanceCounter();
	double delta = double(time - prev) / double(SDL_GetPerformanceFrequency());

	// Invoke our main script's draw function.
	auto traceback = lua_pushtraceback(lua);
	if (lua_getglobal(lua, "draw") == LUA_TFUNCTION)
	{
		lua_pushnumber(lua, delta);
		if (lua_pcall(lua, 1, 0, traceback) != LUA_OK)
		{ SDL_LogError(0, "Lua: %s", lua_tostring(lua, -1)); }
	}
	lua_settop(lua, 0);

	// Continue running.
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
	// When exiting program, hide window immediately.
	if (!SDL_HideWindow(window))
	{ SDL_LogError(0, "%s", SDL_GetError); }

	// Pre-emptively collect garbage.
	lua_gc(lua, LUA_GCCOLLECT);

	// Exit app & signal success.
	return SDL_APP_SUCCESS;
}