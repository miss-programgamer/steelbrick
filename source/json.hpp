#ifndef GAME_JSON_HEADER
#define GAME_JSON_HEADER


#include <cstdint>
#include <cstddef>

#include <SDL3/SDL_iostream.h>


/* ==================================================
	JSON CHUNK API
================================================== */

enum JSN_Type
{
	JSN_TYPE_INVALID = 0,

	JSN_TYPE_NULL,
	JSN_TYPE_BOOL,
	JSN_TYPE_INTEGER,
	JSN_TYPE_NUMBER,
	JSN_TYPE_STRING,
	JSN_TYPE_ARRAY,
	JSN_TYPE_OBJECT,
};


struct JSN_Value;
struct JSN_Element;
struct JSN_Property;

struct JSN_ArrayData;
struct JSN_ObjectData;


using JSN_Null = nullptr_t;

using JSN_Bool = bool;

using JSN_Integer = int64_t;

using JSN_Number = double;

using JSN_String = const char*;

using JSN_Array = JSN_ArrayData*;

using JSN_Object = JSN_ObjectData*;


struct JSN_Value
{
	JSN_Type type;
	union
	{
		JSN_Null    null_value;
		JSN_Bool    bool_value;
		JSN_Integer integer_value;
		JSN_Number  number_value;
		JSN_String  string_value;
		JSN_Array   array_value;
		JSN_Object  object_value;
	};
};


struct JSN_Chunk;


JSN_Chunk* JSN_CreateChunk();

void JSN_DestroyChunk(JSN_Chunk* chunk);


bool JSN_ChunkCopy(JSN_Chunk* chunk, JSN_Value* dest, const JSN_Value* src);


JSN_Value* JSN_GetChunkRoot(JSN_Chunk* chunk);


void JSN_ChunkSetNull(JSN_Chunk* chunk, JSN_Value* value);

void JSN_ChunkSetBool(JSN_Chunk* chunk, JSN_Value* value, bool bool_value);

void JSN_ChunkSetNumber(JSN_Chunk* chunk, JSN_Value* value, double number_value);

void JSN_ChunkSetString(JSN_Chunk* chunk, JSN_Value* value, const char* string_value, size_t length);

void JSN_ChunkSetArray(JSN_Chunk* chunk, JSN_Value* value);

void JSN_ChunkSetObject(JSN_Chunk* chunk, JSN_Value* value);


JSN_Value* JSN_ChunkAddElement(JSN_Chunk* chunk, JSN_Value* value, JSN_Element* whence);

JSN_Value* JSN_ChunkAddProperty(JSN_Chunk* chunk, JSN_Value* value, const char* key, size_t length, JSN_Property* whence);


/* ==================================================
	JSON READER INTERFACE API
================================================== */

struct JSN_ReaderInterface
{
	bool (*key)(void* userdata, const char* key, size_t length);

	bool (*value)(void* userdata, JSN_Value* value);

	bool (*open_array)(void* userdata);

	bool (*close_array)(void* userdata, size_t length);

	bool (*open_object)(void* userdata);

	bool (*close_object)(void* userdata, size_t length);
};


bool JSN_Read(SDL_IOStream* stream, const JSN_ReaderInterface* iface, void* userdata);


/* ==================================================
	JSON UTILITY API
================================================== */

JSN_Chunk* JSN_ReadFile(const char* file);

JSN_Chunk* JSN_ReadMem(const void* mem, size_t length);


#endif // GAME_JSON_HEADER