#ifndef GAME_DEBUG_HEADER
#define GAME_DEBUG_HEADER


#ifdef _DEBUG
/**
 * @brief Is `true` if `_DEBUG` is defined, `false` otherwise.
 */
constexpr bool debug = true;
#else
/**
 * @brief Is `true` if `_DEBUG` is defined, `false` otherwise.
 */
constexpr bool debug = false;
#endif


#endif // GAME_DEBUG_HEADER