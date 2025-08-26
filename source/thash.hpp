#ifndef GAME_THASH_HEADER
#define GAME_THASH_HEADER


#include <utility>


namespace Game
{
	template<typename T>
	class thash : public std::hash<T>
	{ using is_transparent = void; };
}


#endif // GAME_THASH_HEADER