#include <exception>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3_shadercross/SDL_shadercross.h>

#include "program.hpp"
using namespace Game;


static SDL_AppResult ShowErrorBox(const char* title, const char* message, SDL_Window* window = nullptr)
{
	if (!SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, window))
	{ SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "%s", SDL_GetError()); }
	return SDL_APP_FAILURE;
}


SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv)
{
	auto& program = *(Program**)appstate;

	SDL_Init(SDL_INIT_VIDEO);
	SDL_ShaderCross_Init();

	try
	{
		program = new Program(argc, argv);
		return SDL_APP_CONTINUE;
	}
	catch (std::exception& ex)
	{
		return ShowErrorBox("Fatal Exception", ex.what());
	}
	catch (...)
	{
		const auto& what = "An unknown exception occured.";
		return ShowErrorBox("Unknown Exception", what);
	}
}


void SDL_AppQuit(void* appstate, SDL_AppResult result)
{
	if (auto program = (Program*)appstate)
	{ delete program; }

	SDL_ShaderCross_Quit();
	SDL_Quit();
}


SDL_AppResult SDL_AppIterate(void* appstate)
{
	auto program = (Program*)appstate;

	try
	{
		return program->Update();
	}
	catch (std::exception& ex)
	{
		return ShowErrorBox("Fatal Exception", ex.what(), *program);
	}
	catch (...)
	{
		const auto& what = "An unknown exception occured.";
		return ShowErrorBox("Unknown Exception", what, *program);
	}
}


SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event)
{
	auto program = (Program*)appstate;

	try
	{
		return program->Handle(*event);
	}
	catch (std::exception& ex)
	{
		return ShowErrorBox("Fatal Exception", ex.what(), *program);
	}
	catch (...)
	{
		const auto& what = "An unknown exception occured.";
		return ShowErrorBox("Unknown Exception", what, *program);
	}
}