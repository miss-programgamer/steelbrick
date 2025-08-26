#ifndef GAME_HASHMAP_HEADER
#define GAME_HASHMAP_HEADER


#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "thash.hpp"


namespace Game
{
	inline namespace Detail
	{
		template<typename K, typename V, typename A = std::allocator<std::pair<const K, V>>>
		struct HashMapHelper
		{ using HashMap = std::unordered_map<K, V, thash<K>, std::equal_to<void>, A>; };

		template<typename V, typename A>
		struct HashMapHelper<std::string, V, A>
		{ using HashMap = std::unordered_map<std::string, V, thash<std::string_view>, std::equal_to<void>, A>; };
	}

	template<typename K, typename V>
	using HashMap = Detail::HashMapHelper<K, V>::HashMap;
}


#endif // GAME_HASHMAP_HEADER