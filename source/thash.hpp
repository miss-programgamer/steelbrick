#ifndef GAME_THASH_HEADER
#define GAME_THASH_HEADER


#include <utility>


namespace Game
{
	/**
	 * @brief Transparent version of `std::hash`.
	 * 
	 * @note This is used to create an alias of `std::unordered_map` that doesn't allocate a string for each lookup.
	 * 
	 * @tparam T The type to hash.
	 */
	template<typename T>
	class thash : public std::hash<T>
	{ using is_transparent = void; };
}


#endif // GAME_THASH_HEADER