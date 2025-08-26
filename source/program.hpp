#ifndef GAME_PROGRAM_HEADER
#define GAME_PROGRAM_HEADER


#include <cstdint>

#include <SDL3/SDL.h>
#include <lua.hpp>


namespace Game
{
	class Program
	{
		SDL_Window* window = nullptr;

		SDL_GPUDevice* device = nullptr;

		lua_State* lua = nullptr;

		uint64_t time = 0;


	 public:
		Program(int argc, char** argv);

		~Program();


		inline operator SDL_Window*() const
		{ return window; }

		inline operator SDL_GPUDevice*() const
		{ return device; }


		SDL_AppResult Update();

		SDL_AppResult Handle(const SDL_Event& event);


	 private:
		SDL_AppResult Handle(const SDL_QuitEvent& quit);
	};
}


#define lua_getprogram(L) (*(Game::Program**)lua_getextraspace(L))


#endif // GAME_PROGRAM_HEADER