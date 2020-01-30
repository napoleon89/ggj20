#pragma once

#include <core/collections/dynamic_array.h>
#include <core/collections/dictionary.h>

enum class JsonValueType {
	Unknown,
	Null,
	Object,
	Array,
	Number,
	String,
	Bool,
};

struct JsonValue;

typedef Dictionary<std::string, JsonValue> JsonObject;
typedef DynamicArray<JsonValue> JsonArray;

struct JsonDocument;

struct JsonValue {
	u32 object_index;
	JsonValueType type;
	JsonDocument* document;

	bool isNull() const;
	bool isObject() const;
	bool isArray() const;
	bool isNumber() const;
	bool isString() const;
	bool isBool() const;

	const JsonObject& getObject() const;
	const JsonArray& getArray() const;
	const f32 getNumber() const;
	const std::string& getString() const;
	const bool getBool() const;
};



struct JsonParser {
	const char* buffer;
	u32 index;
	u32 size;

	bool isIgnorableChar(const char c);
	bool canRead() const;
	void skipIgnorableChars();
	char readChar();
	char peekChar();
	
	std::string readString();
	bool readBool();
	f32 readNumber();
	void readNull();
};

struct JsonDocument {
	DynamicArray<JsonObject> objects;
	DynamicArray<JsonArray> arrays;
	DynamicArray<f32> numbers;
	DynamicArray<std::string> strings;
	DynamicArray<bool> bools;

	JsonValue root;

	void parseFromMemory(const char* data, const u32 size);
	void parseValue(JsonParser& parser, JsonValue& current_value);
	void parseObject(JsonParser& parser, JsonValue& current_value);
	void parseArray(JsonParser& parser, JsonValue& current_value);
};