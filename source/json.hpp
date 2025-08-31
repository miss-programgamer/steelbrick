#ifndef GAME_JSON_HEADER
#define GAME_JSON_HEADER


#include <cstdint>
#include <cstddef>

#include <SDL3/SDL_iostream.h>


/* ==================================================
	JSON CHUNK API
================================================== */

/**
 * @brief Standard JSON value types, except with 'integer' as its own type.
 */
enum JSN_Type
{
	JSN_TYPE_EMPTY = 0,		/**< Initial type of new values. Only valid for root value (empty file). */

	JSN_TYPE_NULL,			/**< Value is of type 'null'. */
	JSN_TYPE_BOOL,			/**< Value is of type 'boolean' */
	JSN_TYPE_INTEGER,		/**< Value is of type 'number', but representable as an integer. */
	JSN_TYPE_NUMBER,		/**< Value is of type 'number', but representable as an double. */
	JSN_TYPE_STRING,		/**< Value is of type 'string'. */
	JSN_TYPE_ARRAY,			/**< Value is of type 'array'. */
	JSN_TYPE_OBJECT,		/**< Value is of type 'object'. */
};


struct JSN_Array;
struct JSN_Object;


struct JSN_Value
{
	JSN_Type type;
	union
	{
		bool			bool_value;
		int64_t			integer_value;
		double			number_value;
		const char*		string_value;
		JSN_Array*		array_value;
		JSN_Object*		object_value;
	};
};


struct JSN_Element
{
	JSN_Value value;
	size_t index;
	JSN_Array* parent;
	JSN_Element* next;
};


struct JSN_Property
{
	JSN_Value value;
	const char* key;
	size_t index;
	JSN_Object* parent;
	JSN_Property* next;
};


struct JSN_Array
{
	JSN_Element* first;
	JSN_Element* last;
	size_t count;
};


struct JSN_Object
{
	JSN_Property* first;
	JSN_Property* last;
	size_t count;
};


/**
 * @brief Opaque handle to a JSON chunk.
 */
typedef struct JSN_Chunk JSN_Chunk;


/**
 * @brief Create a JSON chunk.
 * 
 * @returns a handle to a newly created JSON chunk.
 */
JSN_Chunk* JSN_CreateChunk();

/**
 * @brief Destroy a JSON chunk.
 * 
 * @param chunk The JSON chunk to destroy.
 */
void JSN_DestroyChunk(JSN_Chunk* chunk);


/**
 * @brief Copy a given JSON value into another one.
 * 
 * @param chunk The JSON chunk from which the destination value originates.
 * @param dest The JSON value to be overwritten by another.
 * @param src The JSON value to overwrite the destination with.
 * @returns true on success or false on failure; call SDL_GetError() for more information.
 */
bool JSN_ChunkCopy(JSN_Chunk* chunk, JSN_Value* dest, const JSN_Value* src);


/**
 * @brief Get the root JSON value of the given JSON chunk.
 * 
 * @param chunk The JSON chunk
 * @return (JSN_Value*) 
 */
JSN_Value* JSN_GetChunkRoot(JSN_Chunk* chunk);


/**
 * @brief Set the given JSON value to null.
 * 
 * @param chunk The JSON chunk from which the value originates.
 * @param value The JSON value to overwrite with null.
 */
void JSN_ChunkSetNull(JSN_Chunk* chunk, JSN_Value* value);

/**
 * @brief Set the given JSON value to a bool value.
 * 
 * @param chunk The JSON chunk from which the value originates.
 * @param value The JSON value to overwrite with a bool value.
 * @param bool_value The boolean value to assign to our value.
 */
void JSN_ChunkSetBool(JSN_Chunk* chunk, JSN_Value* value, bool bool_value);

/**
 * @brief Set the given JSON value to an integer value.
 * 
 * @param chunk The JSON chunk from which the value originates.
 * @param value The JSON value to overwrite with an integer value.
 * @param integer_value The integer value to assign to our value.
 */
void JSN_ChunkSetInteger(JSN_Chunk* chunk, JSN_Value* value, int64_t integer_value);

/**
 * @brief Set the given JSON value to a numeric value.
 * 
 * @param chunk The JSON chunk from which the value originates.
 * @param value The JSON value to overwrite with a numeric value.
 * @param number_value The numeric value to assign to our value.
 */
void JSN_ChunkSetNumber(JSN_Chunk* chunk, JSN_Value* value, double number_value);

/**
 * @brief Set the given JSON value to a string value.
 * 
 * @param chunk The JSON chunk from which the value originates.
 * @param value The JSON value to overwrite with a string value.
 * @param string_value The string value to assign to our value.
 * @param length The length of the given string value.
 */
void JSN_ChunkSetString(JSN_Chunk* chunk, JSN_Value* value, const char* string_value, size_t length);

/**
 * @brief Set the given JSON value to an array.
 * 
 * @param chunk The JSON chunk from which the value originates.
 * @param value The JSON value to overwrite with an array.
 */
void JSN_ChunkSetArray(JSN_Chunk* chunk, JSN_Value* value);

/**
 * @brief Set the given JSON value to an object.
 * 
 * @param chunk The JSON chunk from which the value originates.
 * @param value The JSON value to overwrite with an object.
 */
void JSN_ChunkSetObject(JSN_Chunk* chunk, JSN_Value* value);


/**
 * @brief Add an element to the given array value.
 * 
 * @param chunk The JSON chunk from which the array value originates.
 * @param array_value The array value to which an element will be added.
 * @param whence Existing element before which the element will be inserted. May be NULL, in which case the element will be inserted at the end.
 * @param index A pointer filled with the index of the newly created element, may be NULL.
 * @returns a pointer to the newly created element's JSON value.
 */
JSN_Value* JSN_ChunkAddElement(JSN_Chunk* chunk, JSN_Value* array_value, JSN_Element* whence, size_t* index);

/**
 * @brief Add a property to the given object value.
 * 
 * @param chunk The JSON chunk from which the object value originates.
 * @param object_value The object value to which a property will be added.
 * @param whence Existing property before which the property will be inserted. May be NULL, in which case the property will be inserted at the end.
 * @param index A pointer filled with the index of the newly created property, may be NULL.
 * @param key The newly created property's key string. A copy of this string is made & stored by this function.
 * @param length The length of the newly created property's key.
 * @returns a pointer to the newly created property's JSON value.
 */
JSN_Value* JSN_ChunkAddProperty(JSN_Chunk* chunk, JSN_Value* object_value, JSN_Property* whence, size_t* index, const char* key, size_t length);


/* ==================================================
	JSON READER INTERFACE API
================================================== */

struct JSN_ReaderInterface
{
    /** @brief The version of this interface. */
    uint32_t version;

	bool (*key)(void* userdata, const char* key, size_t length);

	bool (*value)(void* userdata, JSN_Value* value);

	bool (*open_array)(void* userdata);

	bool (*close_array)(void* userdata, size_t length);

	bool (*open_object)(void* userdata);

	bool (*close_object)(void* userdata, size_t length);
};


#define JSN_INIT_READER_INTERFACE(iface)		\
	do {										\
		SDL_zerop(iface);						\
		(iface)->version = sizeof(*(iface));	\
	} while (0)


/**
 * @brief Read JSON data from a stream using the given reader interface.
 * 
 * @param stream An SDL_IOStream from which JSON data will be read from.
 * @param iface Pointer to an implementation of the JSN_ReaderInterface interface.
 * @param userdata Opaque pointer passed to interface functions for state management.
 * @param closeio true to close/free the SDL_IOStream before returning, false to leave it open.
 * @returns true on success or false on failure; call SDL_GetError() for more information.
 */
bool JSN_Read(SDL_IOStream* stream, const JSN_ReaderInterface* iface, void* userdata, bool closeio);


/* ==================================================
	JSON UTILITY API
================================================== */

JSN_Chunk* JSN_ReadChunkFromIO(SDL_IOStream* stream, bool closeio);

JSN_Chunk* JSN_ReadChunkFromFile(const char* file);

JSN_Chunk* JSN_ReadChunkFromMem(const void* mem, size_t length);


#endif // GAME_JSON_HEADER