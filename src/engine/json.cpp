#include <engine/json.h>

static bool isNumeric(const char c) {
	return c >= '0' && c <= '9';
}

bool JsonParser::isIgnorableChar(const char c) {
	return !isalnum(c) && 
		c != '{' && 
		c != '}' && 
		c != '[' &&
		c != ']' &&
		c != ':' &&
		c != '"' &&
		c != '.' &&
		c != '-';
}

bool JsonParser::canRead() const {
	return index < size;
}

void JsonParser::skipIgnorableChars() {
	while(isIgnorableChar(buffer[index])) {
		index++;
		Assert(index < size);
	}
}

char JsonParser::readChar() {
	skipIgnorableChars();
	char result = buffer[index];
	index++;
	return result;
}

char JsonParser::peekChar() {
	skipIgnorableChars();
	Assert(index < size);
	return buffer[index];
}

std::string JsonParser::readString() {
	char previous = readChar(); // "
	char c = buffer[index];
	int start = index;
	int length = 0;
	while(!(c == '"' && previous != '\\')) {
		length++;
		previous = c;
		c = buffer[start + length];
	}

	index = start + length+1;

	return std::string(buffer + start, buffer + start + length);
}

bool JsonParser::readBool() {
	const char c = readChar();
	if(c == 't') {
		for(u32 i = 0; i < 3; i++) readChar();
		return true;
	} else {
		for(u32 i = 0; i < 4; i++) readChar();
		
		return false;
	}
	
}

f32 JsonParser::readNumber() {
	char* end;
	const char* start = buffer + index;
	f32 result = strtof(start, &end);
	u32 diff = (u32)(end - start);
	index += diff;
	return result;
}

void JsonParser::readNull() {
	for(u32 i = 0; i < 4; i++) index++;
}

void JsonDocument::parseFromMemory(const char* data, const u32 size) {
	JsonParser parser = {data, 0, size};
	parseValue(parser, root);
}

void JsonDocument::parseValue(JsonParser& parser, JsonValue& current_value) {
	current_value.document = this;
	char start = parser.peekChar();
	if(start == '{') {
		current_value.type = JsonValueType::Object;
		parseObject(parser, current_value);
	} else if(start == '[') {
		current_value.type = JsonValueType::Array;
		parseArray(parser, current_value);
	} else if(isNumeric(start) || start == '-') {
		current_value.type = JsonValueType::Number;
		f32 value = parser.readNumber();
		numbers.add(value);
		current_value.object_index = numbers.count-1;
	} else if(start == 'f' || start == 't') {
		current_value.type = JsonValueType::Bool;
		const bool b = parser.readBool();
		bools.add(b);
		current_value.object_index = bools.count-1;
		
	} else if(start == 'n') {
		current_value.type = JsonValueType::Null;
		parser.readNull();
		
	} else if(start == '"') {
		current_value.type = JsonValueType::String;
		std::string str = parser.readString();
		strings.add(str);
		current_value.object_index = strings.count-1;
	} else {
		current_value.type = JsonValueType::Unknown;
		Assert(1 == 0);
	}

	if(parser.canRead()) {
		char next = parser.peekChar();
		if(next == ',') parser.readChar();
	}
}

void JsonDocument::parseObject(JsonParser& parser, JsonValue& current_value) {
	JsonObject object = {};
	parser.readChar(); // {
	char next = parser.peekChar();
	while(next != '}') {
		std::string member_name = parser.readString();
		parser.readChar(); // :
		
		JsonValue value = {};
		parseValue(parser, value);
		
		object.insertItem(member_name, value);
		next = parser.peekChar();
	}

	parser.readChar(); // }
	
	objects.add(object);
	current_value.object_index = objects.count-1;
}

void JsonDocument::parseArray(JsonParser& parser, JsonValue& current_value) {
	parser.readChar(); // [
	JsonArray array = {};

	char next = parser.peekChar();
	while(next != ']') {

		JsonValue value = {};
		parseValue(parser, value);
		array.add(value);
		
		next = parser.peekChar();
	}
	
	parser.readChar(); // ]
	arrays.add(array);
	current_value.object_index = arrays.count-1;
}

bool JsonValue::isNull() const {
	return type == JsonValueType::Null;
}

bool JsonValue::isObject() const {
	return type == JsonValueType::Object;
}

bool JsonValue::isArray() const {
	return type == JsonValueType::Array;
}

bool JsonValue::isNumber() const {
	return type == JsonValueType::Number;
}

bool JsonValue::isString() const {
	return type == JsonValueType::String;
}

bool JsonValue::isBool() const {
	return type == JsonValueType::Bool;
}

const JsonObject& JsonValue::getObject() const {
	Assert(type == JsonValueType::Object);
	return document->objects.getRefConst(object_index);
}

const JsonArray& JsonValue::getArray() const {
	Assert(type == JsonValueType::Array);
	return document->arrays.getRefConst(object_index);
}

const f32 JsonValue::getNumber() const {
	Assert(type == JsonValueType::Number);
	return document->numbers.getCopy(object_index);
}

const std::string& JsonValue::getString() const {
	Assert(type == JsonValueType::String);
	return document->strings.getRefConst(object_index);
}

const bool JsonValue::getBool() const {
	Assert(type == JsonValueType::Bool);
	return document->bools.getRefConst(object_index);
}
