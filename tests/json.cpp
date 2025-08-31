#include <catch2/catch_test_macros.hpp>


#include <json.hpp>


TEST_CASE("JSON/Chunk/Create & Destroy", "[json]")
{
	JSN_Chunk* chunk = JSN_CreateChunk();
	JSN_Value* root = JSN_GetChunkRoot(chunk);
	REQUIRE(root->type == JSN_TYPE_EMPTY);
	JSN_DestroyChunk(chunk);
}


TEST_CASE("JSON/Reader/Read Memory", "[json]")
{
	SECTION("Read null value")
	{
		const char* json_string = "";
		size_t json_length = SDL_strlen(json_string);

		JSN_Chunk* chunk = JSN_ReadChunkFromMem(json_string, json_length);
		JSN_Value* root = JSN_GetChunkRoot(chunk);
		REQUIRE(root->type == JSN_TYPE_EMPTY);
		JSN_DestroyChunk(chunk);
	}

	SECTION("Read null value")
	{
		const char* json_string = "null";
		size_t json_length = SDL_strlen(json_string);

		JSN_Chunk* chunk = JSN_ReadChunkFromMem(json_string, json_length);
		JSN_Value* root = JSN_GetChunkRoot(chunk);
		REQUIRE(root->type == JSN_TYPE_NULL);
		JSN_DestroyChunk(chunk);
	}

	SECTION("Read integer value")
	{
		const char* json_string = "0";
		size_t json_length = SDL_strlen(json_string);

		JSN_Chunk* chunk = JSN_ReadChunkFromMem(json_string, json_length);
		JSN_Value* root = JSN_GetChunkRoot(chunk);
		REQUIRE(root->type == JSN_TYPE_INTEGER);
		JSN_DestroyChunk(chunk);
	}

	SECTION("Read number value")
	{
		const char* json_string = "0.0";
		size_t json_length = SDL_strlen(json_string);

		JSN_Chunk* chunk = JSN_ReadChunkFromMem(json_string, json_length);
		JSN_Value* root = JSN_GetChunkRoot(chunk);
		REQUIRE(root->type == JSN_TYPE_NUMBER);
		JSN_DestroyChunk(chunk);
	}

	SECTION("Read infinity value")
	{
		const char* json_string = "Infinity";
		size_t json_length = SDL_strlen(json_string);

		JSN_Chunk* chunk = JSN_ReadChunkFromMem(json_string, json_length);
		JSN_Value* root = JSN_GetChunkRoot(chunk);
		REQUIRE(root->type == JSN_TYPE_NUMBER);
		JSN_DestroyChunk(chunk);
	}

	SECTION("Read empty array")
	{
		const char* json_string = "[]";
		size_t json_length = SDL_strlen(json_string);

		JSN_Chunk* chunk = JSN_ReadChunkFromMem(json_string, json_length);
		JSN_Value* root = JSN_GetChunkRoot(chunk);
		REQUIRE(root->type == JSN_TYPE_ARRAY);
		JSN_DestroyChunk(chunk);
	}

	SECTION("Read empty object")
	{
		const char* json_string = "{}";
		size_t json_length = SDL_strlen(json_string);

		JSN_Chunk* chunk = JSN_ReadChunkFromMem(json_string, json_length);
		JSN_Value* root = JSN_GetChunkRoot(chunk);
		REQUIRE(root->type == JSN_TYPE_OBJECT);
		JSN_DestroyChunk(chunk);
	}
}