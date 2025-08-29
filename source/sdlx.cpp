#include "sdlx.hpp"


SDL_Surface* IMG_LoadFormat(const char* filename, SDL_PixelFormat format)
{
	if (auto temp = IMG_Load(filename))
	{
		auto surface = SDL_ConvertSurface(temp, format);
		SDL_DestroySurface(temp);
		return surface;
	}

	return nullptr;
}