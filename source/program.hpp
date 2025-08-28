#ifndef GAME_PROGRAM_HEADER
#define GAME_PROGRAM_HEADER


#include <cstdint>

#include <SDL3/SDL.h>
#include <lua.hpp>


namespace Game
{
	/**
	 * @brief Main "singleton" class of the project.
	 */
	class Program
	{
		SDL_Window* window = nullptr;

		SDL_GPUDevice* device = nullptr;

		lua_State* lua = nullptr;

		uint64_t time = 0;


	 public:
		/**
		 * @brief Disallow default-construction.
		 */
		Program() = delete;

		/**
		 * @brief Disallow copy-construction.
		 */
		Program(const Program&) = delete;

		/**
		 * @brief Disallow move-construction.
		 */
		Program(Program&&) = delete;

		/**
		 * @brief Construct a new Program instance.
		 * 
		 * @param argc Argument count.
		 * @param argv Argument vector.
		 */
		Program(int argc, char** argv);

		/**
		 * @brief Destruct this Program.
		 */
		~Program();


		/**
		 * @brief Program instance implicitly convertible to a window pointer.
		 * 
		 * @return The window created by this program during construction.
		 */
		inline operator SDL_Window*() const
		{ return window; }

		/**
		 * @brief Program instance implicitly convertible to a GPU device pointer.
		 * 
		 * @return The GPU device created by this program during construction.
		 */
		inline operator SDL_GPUDevice*() const
		{ return device; }


		/**
		 * @brief Update the program state, called roughly every frame.
		 * 
		 * @return Whether to continue running the program or to exit.
		 */
		SDL_AppResult Update();

		/**
		 * @brief Handle incoming events, dispatching them so sub-handlers as needed.
		 * 
		 * @param event A const reference to an incoming event of any type.
		 * @return Whether to continue running the program or to exit. 
		 */
		SDL_AppResult Handle(const SDL_Event& event);


	 private:
		SDL_AppResult Handle(const SDL_QuitEvent& quit);
	};
}


/**
 * @brief Get the extra space in the given Lua state as a reference to a pointer to a Program instance.
 * 
 * @param L Lua state.
 */
#define lua_getprogram(L) (*(Game::Program**)lua_getextraspace(L))


#endif // GAME_PROGRAM_HEADER