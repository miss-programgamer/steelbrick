#include "json.hpp"


#include <SDL3/SDL_assert.h>


/* ==================================================
	JSON CHUNK API
================================================== */

struct JSN_Chunk
{
	JSN_Value root;
};


static void FinalizeValue(JSN_Chunk* chunk, JSN_Value* value)
{
	switch (value->type)
	{
		case JSN_TYPE_STRING:
			SDL_free((void*)value->string_value);
			break;

		case JSN_TYPE_ARRAY:
		{
			JSN_Array* array = value->array_value;
			for (JSN_Element* el = array->first; el; el = el->next)
			{ FinalizeValue(chunk, &el->value); }
			SDL_free(array);
			break;
		}

		case JSN_TYPE_OBJECT:
		{
			JSN_Object* object = value->object_value;
			for (JSN_Property* prop = object->first; prop; prop = prop->next)
			{
				SDL_free((void*)prop->key);
				FinalizeValue(chunk, &prop->value);
			}
			SDL_free(object);
			break;
		}
	}

	SDL_zerop(value);
}


JSN_Chunk* JSN_CreateChunk()
{
	JSN_Chunk* chunk = (JSN_Chunk*)SDL_malloc(sizeof(JSN_Chunk));
	SDL_zerop(chunk);
	return chunk;
}


void JSN_DestroyChunk(JSN_Chunk* chunk)
{
	FinalizeValue(chunk, &chunk->root);
	SDL_free(chunk);
}


bool JSN_ChunkCopy(JSN_Chunk* chunk, JSN_Value* dest, const JSN_Value* src)
{
	FinalizeValue(chunk, dest);
	dest->type = src->type;

	switch (src->type)
	{
		case JSN_TYPE_BOOL:
			dest->bool_value = src->bool_value;
			break;

		case JSN_TYPE_INTEGER:
			dest->integer_value = src->integer_value;
			break;

		case JSN_TYPE_NUMBER:
			dest->number_value = src->number_value;
			break;

		case JSN_TYPE_STRING:
		{
			size_t length = SDL_strlen(src->string_value);
			char* copy = (char*)SDL_calloc(length + 1, sizeof(char));
			SDL_strlcpy(copy, src->string_value, length);
			dest->string_value = copy;
			break;
		}

		case JSN_TYPE_ARRAY:
			for (JSN_Element* el = src->array_value->first; el; el = el->next)
			{
				JSN_Value* value = JSN_ChunkAddElement(chunk, dest, NULL, NULL);
				JSN_ChunkCopy(chunk, value, &el->value);
			}
			break;

		case JSN_TYPE_OBJECT:
			for (JSN_Property* prop = src->object_value->first; prop; prop = prop->next)
			{
				JSN_Value* value = JSN_ChunkAddProperty(chunk, dest, NULL, NULL, prop->key, SDL_strlen(prop->key));
				JSN_ChunkCopy(chunk, value, &prop->value);
			}
			break;
	}

	return true;
}


JSN_Value* JSN_GetChunkRoot(JSN_Chunk* chunk)
{
	return &chunk->root;
}


void JSN_ChunkSetNull(JSN_Chunk* chunk, JSN_Value* value)
{
	FinalizeValue(chunk, value);
	value->type = JSN_TYPE_NULL;
}


void JSN_ChunkSetBool(JSN_Chunk* chunk, JSN_Value* value, bool bool_value)
{
	FinalizeValue(chunk, value);
	value->type = JSN_TYPE_BOOL;
	value->bool_value = bool_value;
}


void JSN_ChunkSetInteger(JSN_Chunk* chunk, JSN_Value* value, int64_t integer_value)
{
	FinalizeValue(chunk, value);
	value->type = JSN_TYPE_INTEGER;
	value->integer_value = integer_value;
}


void JSN_ChunkSetNumber(JSN_Chunk* chunk, JSN_Value* value, double number_value)
{
	FinalizeValue(chunk, value);
	value->type = JSN_TYPE_NUMBER;
	value->number_value = number_value;
}


void JSN_ChunkSetString(JSN_Chunk* chunk, JSN_Value* value, const char* string_value, size_t length)
{
	FinalizeValue(chunk, value);
	value->type = JSN_TYPE_STRING;
	char* copy = (char*)SDL_calloc(length + 1, sizeof(char));
	SDL_strlcpy(copy, string_value, length);
	value->string_value = copy;
}


void JSN_ChunkSetArray(JSN_Chunk* chunk, JSN_Value* value)
{
	FinalizeValue(chunk, value);
	value->type = JSN_TYPE_ARRAY;
	value->array_value = (JSN_Array*)SDL_malloc(sizeof(JSN_Array));
	SDL_zerop(value->array_value);
}


void JSN_ChunkSetObject(JSN_Chunk* chunk, JSN_Value* value)
{
	FinalizeValue(chunk, value);
	value->type = JSN_TYPE_OBJECT;
	value->object_value = (JSN_Object*)SDL_malloc(sizeof(JSN_Object));
	SDL_zerop(value->object_value);
}


JSN_Value* JSN_ChunkAddElement(JSN_Chunk* chunk, JSN_Value* array_value, JSN_Element* whence, size_t* index)
{
	SDL_assert(array_value && array_value->type == JSN_TYPE_ARRAY);
	SDL_assert(!whence || whence->parent == array_value->array_value);

	JSN_Array* array = array_value->array_value;

	JSN_Element* el = (JSN_Element*)SDL_malloc(sizeof(JSN_Element));
	SDL_zerop(el);
	el->parent = array;
	el->index = array->count++;

	if (whence == array->first)
	{ array->first = el; }

	if (whence == NULL)
	{ array->last = el; }

	el->next = whence;

	if (index)
	{ *index = el->index; }

	return &el->value;
}


JSN_Value* JSN_ChunkAddProperty(JSN_Chunk* chunk, JSN_Value* object_value, JSN_Property* whence, size_t* index, const char* key, size_t length)
{
	SDL_assert(object_value && object_value->type == JSN_TYPE_OBJECT);
	SDL_assert(!whence || whence->parent == object_value->object_value);

	JSN_Object* object = object_value->object_value;

	JSN_Property* prop = (JSN_Property*)SDL_malloc(sizeof(JSN_Property));
	SDL_zerop(prop);
	prop->parent = object;
	prop->index = object->count++;

	char* copy = (char*)SDL_calloc(length + 1, sizeof(char));
	SDL_strlcpy(copy, key, length);
	prop->key = copy;

	if (whence == object->first)
	{ object->first = prop; }

	if (whence == NULL)
	{ object->last = prop; }

	prop->next = whence;

	if (index)
	{ *index = prop->index; }

	return &prop->value;
}


/* ==================================================
	JSON TOKENIZER IMPLEMENTATION
================================================== */

enum JSN_TokenType
{
	JSN_TOKEN_NONE = 0,

	JSN_TOKEN_NULL,
	JSN_TOKEN_BOOL,
	JSN_TOKEN_INTEGER,
	JSN_TOKEN_NUMBER,
	JSN_TOKEN_STRING,

	JSN_TOKEN_COMMA,
	JSN_TOKEN_COLON,

	JSN_TOKEN_ARRAY_OPEN,
	JSN_TOKEN_ARRAY_CLOSE,

	JSN_TOKEN_OBJECT_OPEN,
	JSN_TOKEN_OBJECT_CLOSE,
};


struct JSN_Tokenizer
{
	char* token_data;
	size_t token_size;
	size_t token_cap;
};


static void JSN_TokenizerInit(JSN_Tokenizer* tokenizer)
{
	constexpr size_t init_length = 16;

	tokenizer->token_data = (char*)SDL_calloc(init_length, sizeof(char));
	tokenizer->token_cap = init_length;

	tokenizer->token_size = 0;
	tokenizer->token_data[0] = '\0';
}


static void JSN_TokenizerQuit(JSN_Tokenizer* tokenizer)
{
	SDL_free(tokenizer->token_data);
}


static void JSN_TokenizerReset(JSN_Tokenizer* tokenizer)
{
	constexpr size_t init_length = 16;

	tokenizer->token_data = (char*)SDL_realloc(tokenizer->token_data, init_length * sizeof(char));
	tokenizer->token_cap = init_length;

	tokenizer->token_size = 0;
	tokenizer->token_data[0] = '\0';
}


static void JSN_TokenizerAppend(JSN_Tokenizer* tokenizer, char c)
{
	if (tokenizer->token_size == tokenizer->token_cap - 1)
	{
		tokenizer->token_cap *= 1.5;
		tokenizer->token_data = (char*)SDL_realloc(tokenizer->token_data, tokenizer->token_cap * sizeof(char));
	}

	tokenizer->token_data[tokenizer->token_size] = c;
	tokenizer->token_data[++tokenizer->token_size] = '\0';
}


static void JSN_TokenizerReturn(JSN_Tokenizer* tokenizer, const char** start, size_t* length)
{
	*start = tokenizer->token_data;
	*length = tokenizer->token_size;
}


static JSN_TokenType NextToken(JSN_Tokenizer* tokenizer, SDL_IOStream* stream, const char** start, size_t* length)
{
	uint8_t c;

	JSN_TokenizerReset(tokenizer);

 start:
	if (SDL_ReadU8(stream, &c))
	{
		if (c == ' ' || c == '\t' || c == '\n')
		{
			goto start;
		}
		else if (c == ',')
		{
			JSN_TokenizerAppend(tokenizer, c);
			JSN_TokenizerReturn(tokenizer, start, length);
			return JSN_TOKEN_COMMA;
		}
		else if (c == ':')
		{
			JSN_TokenizerAppend(tokenizer, c);
			JSN_TokenizerReturn(tokenizer, start, length);
			return JSN_TOKEN_COLON;
		}
		else if (c == '[')
		{
			JSN_TokenizerAppend(tokenizer, c);
			JSN_TokenizerReturn(tokenizer, start, length);
			return JSN_TOKEN_ARRAY_OPEN;
		}
		else if (c == ']')
		{
			JSN_TokenizerAppend(tokenizer, c);
			JSN_TokenizerReturn(tokenizer, start, length);
			return JSN_TOKEN_ARRAY_CLOSE;
		}
		else if (c == '{')
		{
			JSN_TokenizerAppend(tokenizer, c);
			JSN_TokenizerReturn(tokenizer, start, length);
			return JSN_TOKEN_OBJECT_OPEN;
		}
		else if (c == '}')
		{
			JSN_TokenizerAppend(tokenizer, c);
			JSN_TokenizerReturn(tokenizer, start, length);
			return JSN_TOKEN_OBJECT_CLOSE;
		}
		else if (SDL_isdigit(c) || c == '-' || c == '+')
		{
			JSN_TokenType type = JSN_TOKEN_INTEGER;

			do
			{
				if (c == '.' || c == 'e')
				{ type = JSN_TOKEN_NUMBER; }

				JSN_TokenizerAppend(tokenizer, c);
				SDL_ReadU8(stream, &c);
			}
			while (SDL_isdigit(c) || c == '.' || c == 'e' || c == 'x');

			if (SDL_GetIOStatus(stream) != SDL_IO_STATUS_EOF)
			{ SDL_SeekIO(stream, -1, SDL_IO_SEEK_CUR); }

			JSN_TokenizerReturn(tokenizer, start, length);
			return type;
		}
		else if (SDL_isalpha(c) || c == '_')
		{
			do
			{
				JSN_TokenizerAppend(tokenizer, c);
				SDL_ReadU8(stream, &c);
			}
			while (SDL_isalnum(c) || c == '_');

			if (SDL_GetIOStatus(stream) != SDL_IO_STATUS_EOF)
			{ SDL_SeekIO(stream, -1, SDL_IO_SEEK_CUR); }

			if (SDL_strcmp(tokenizer->token_data, "null") == 0)
			{
				JSN_TokenizerReturn(tokenizer, start, length);
				return JSN_TOKEN_NULL;
			}
			else if (SDL_strcmp(tokenizer->token_data, "true") == 0)
			{
				JSN_TokenizerReturn(tokenizer, start, length);
				return JSN_TOKEN_BOOL;
			}
			else if (SDL_strcmp(tokenizer->token_data, "false") == 0)
			{
				JSN_TokenizerReturn(tokenizer, start, length);
				return JSN_TOKEN_BOOL;
			}
			else if (SDL_strcmp(tokenizer->token_data, "Infinity") == 0)
			{
				JSN_TokenizerReturn(tokenizer, start, length);
				return JSN_TOKEN_NUMBER;
			}
			else if (SDL_strcmp(tokenizer->token_data, "NaN") == 0)
			{
				JSN_TokenizerReturn(tokenizer, start, length);
				return JSN_TOKEN_NUMBER;
			}
			else
			{
				SDL_SetError("unknown token encountered while reading JSON stream: %s", tokenizer->token_data);
				return JSN_TOKEN_NONE;
			}
		}
		else if (c == '"')
		{
			JSN_TokenizerReturn(tokenizer, start, length);
			return JSN_TOKEN_STRING;
		}
		else if (c >= 128)
		{
			// TODO: actually handle UTF8 character sequences
			SDL_SetError("UTF8 character encountered while reading JSON stream");
			return JSN_TOKEN_NONE;
		}
	}

	return JSN_TOKEN_NONE;
}


/* ==================================================
	JSON READER INTERFACE API
================================================== */

bool JSN_Read(SDL_IOStream* stream, const JSN_ReaderInterface* iface, void* userdata, bool closeio)
{
	JSN_Tokenizer tokenizer;
	JSN_TokenizerInit(&tokenizer);

	const char* token_data;
	size_t token_size;

	JSN_Value value;
	SDL_zero(value);

	for (bool running = true; running;)
	{
		switch (NextToken(&tokenizer, stream, &token_data, &token_size))
		{
			case JSN_TOKEN_NONE:
				running = false;
				break;

			case JSN_TOKEN_NULL:
				value.type = JSN_TYPE_NULL;
				if (!iface->value(userdata, &value))
				{ return false; }
				break;

			case JSN_TOKEN_BOOL:
				value.type = JSN_TYPE_BOOL;
				value.bool_value = token_data[0] == 't';
				if (!iface->value(userdata, &value))
				{ return false; }
				break;

			case JSN_TOKEN_INTEGER:
				value.type = JSN_TYPE_INTEGER;
				value.integer_value = SDL_strtol(token_data, NULL, 10);
				if (!iface->value(userdata, &value))
				{ return false; }
				break;

			case JSN_TOKEN_NUMBER:
				value.type = JSN_TYPE_NUMBER;
				value.number_value = SDL_strtod(token_data, NULL);
				if (!iface->value(userdata, &value))
				{ return false; }
				break;

			case JSN_TOKEN_ARRAY_OPEN:
				if (!iface->open_array(userdata))
				{ return false; }
				break;

			case JSN_TOKEN_ARRAY_CLOSE:
				if (!iface->close_array(userdata, 0))
				{ return false; }
				break;

			case JSN_TOKEN_OBJECT_OPEN:
				if (!iface->open_object(userdata))
				{ return false; }
				break;

			case JSN_TOKEN_OBJECT_CLOSE:
				if (!iface->close_object(userdata, 0))
				{ return false; }
				break;

			default:
				running = false;
				break;
		}
	}

	JSN_TokenizerQuit(&tokenizer);

	bool success = SDL_GetIOStatus(stream) == SDL_IO_STATUS_EOF;

	if (closeio)
	{ SDL_CloseIO(stream); }

	return success;
}


/* ==================================================
	JSON UTILITY API
================================================== */

struct JSN_ReaderFrame
{
	JSN_Value* value;
	JSN_ReaderFrame* parent;
};


struct JSN_ChunkReader
{
	JSN_Chunk* result;
	JSN_ReaderFrame* top;
	const char* key;
	size_t key_length;
};


static JSN_ReaderFrame* JSN_ChunkReaderPush(JSN_ChunkReader* reader)
{
	JSN_ReaderFrame* frame = (JSN_ReaderFrame*)SDL_malloc(sizeof(JSN_ReaderFrame));
	SDL_zerop(frame);

	if (reader->top)
	{
		switch (reader->top->value->type)
		{
			case JSN_TYPE_ARRAY:
				frame->value = JSN_ChunkAddElement(reader->result, reader->top->value, NULL, NULL);
				break;

			case JSN_TYPE_OBJECT:
				frame->value = JSN_ChunkAddProperty(reader->result, reader->top->value, NULL, NULL, reader->key, reader->key_length);
				break;
		}

		frame->parent = reader->top;
		reader->top = frame;
	}
	else
	{
		frame->value = &reader->result->root;
		reader->top = frame;
	}

	return frame;
}


static void JSN_ChunkReaderPop(JSN_ChunkReader* reader)
{
	JSN_ReaderFrame* top = reader->top;
	reader->top = top->parent;
	SDL_free(top);
}


static bool JSN_ChunkReaderKey(void* userdata, const char* key, size_t length)
{
	JSN_ChunkReader* reader = (JSN_ChunkReader*)userdata;

	char* copy = (char*)SDL_calloc(length + 1, sizeof(char));
	SDL_strlcpy(copy, key, length);
	reader->key = copy;
	reader->key_length = length;

	return true;
}


static bool JSN_ChunkReaderValue(void* userdata, JSN_Value* value)
{
	JSN_ChunkReader* reader = (JSN_ChunkReader*)userdata;

	if (reader->top)
	{
		switch (reader->top->value->type)
		{
			case JSN_TYPE_ARRAY:
				return JSN_ChunkCopy(reader->result, JSN_ChunkAddElement(reader->result, reader->top->value, NULL, NULL), value);

			case JSN_TYPE_OBJECT:
				return JSN_ChunkCopy(reader->result, JSN_ChunkAddProperty(reader->result, reader->top->value, NULL, NULL, reader->key, reader->key_length), value);

			default:
				return false;
		}
	}
	else
	{
		return JSN_ChunkCopy(reader->result, &reader->result->root, value);
	}
}


static bool JSN_ChunkReaderOpenArray(void* userdata)
{
	JSN_ChunkReader* reader = (JSN_ChunkReader*)userdata;

	JSN_ReaderFrame* top = JSN_ChunkReaderPush(reader);
	JSN_ChunkSetArray(reader->result, top->value);

	return true;
}


static bool JSN_ChunkReaderCloseArray(void* userdata, size_t)
{
	JSN_ChunkReader* reader = (JSN_ChunkReader*)userdata;
	JSN_ChunkReaderPop(reader);
	return true;
}


static bool JSN_ChunkReaderOpenObject(void* userdata)
{
	JSN_ChunkReader* reader = (JSN_ChunkReader*)userdata;

	JSN_ReaderFrame* top = JSN_ChunkReaderPush(reader);
	JSN_ChunkSetObject(reader->result, top->value);

	return true;
}


static bool JSN_ChunkReaderCloseObject(void* userdata, size_t)
{
	JSN_ChunkReader* reader = (JSN_ChunkReader*)userdata;
	JSN_ChunkReaderPop(reader);
	return true;
}


JSN_Chunk* JSN_ReadChunkFromIO(SDL_IOStream* stream, bool closeio)
{
	static const JSN_ReaderInterface iface
	{
		.version = sizeof(JSN_ReaderInterface),
		.key = JSN_ChunkReaderKey,
		.value = JSN_ChunkReaderValue,
		.open_array = JSN_ChunkReaderOpenArray,
		.close_array = JSN_ChunkReaderCloseArray,
		.open_object = JSN_ChunkReaderOpenObject,
		.close_object = JSN_ChunkReaderCloseObject,
	};

	JSN_ChunkReader reader;
	SDL_zero(reader);
	reader.result = JSN_CreateChunk();

	if (JSN_Read(stream, &iface, &reader, closeio))
	{ return reader.result; }

	return NULL;
}


JSN_Chunk* JSN_ReadChunkFromFile(const char* file)
{
	return JSN_ReadChunkFromIO(SDL_IOFromFile(file, "rt"), true);
}


JSN_Chunk* JSN_ReadChunkFromMem(const void* mem, size_t length)
{
	return JSN_ReadChunkFromIO(SDL_IOFromConstMem(mem, length), true);
}