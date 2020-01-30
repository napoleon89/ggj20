#include <engine/reflection.h>
#include <string>
#include <cstring>
#include <cstdio>

using namespace Reflection;

void Reflection::Class::print() {
	printf("%s\n", name);
	for(u32 i = 0; i < field_count; i++) {
		const Field &field = fields[i];
		if(field.type->is_primitive) {
			printf("%s %s\n", field.type->name, field.name);
		} else {
			Class *class_type = (Class *)field.type;
			class_type->print();
		} 
	}
}

Field  *Reflection::Class::findField(const char *name) {
	for (u32 i = 0; i < field_count; i++) {
		Field &field = fields[i];
		if (std::strcmp(field.name, name) == 0) {
			return &field;
		}
	}
	return nullptr;
}

namespace std {
	string to_string(string input) {
		return input;
	}
};	

#define PrimitiveTypes \
	PrimType(s8, s8, "%hhi",  s8)  \
	PrimType(s16, s16, "%hi", s16) \
	PrimType(s32, s32, "%i", s32) \
	PrimType(s64, s64, "%lli", s64) \
	PrimType(u8, u8, "%hhu",  u8) \
	PrimType(u16, u16, "%hu", u16) \
	PrimType(u32, u32, "%u", u32) \
	PrimType(u64, u64, "%llu", u64) \
	PrimType(f32, f32, "%f", f32) \
	PrimType(f64, f64, "%f", f64) \
	PrimType(boolean, bool, "%d", s32) \

#define PrimType(enum_value, type, fmt, read_type) template<> const Type *Reflection::getType<type>() { static Type result = {#type, fmt, sizeof(type), true, PrimitiveType::enum_value}; return &result; }

PrimitiveTypes

PrimType(string, std::string, "%s", std::string)

const std::string Field::getStaticValueAsString() const {
#define PrimType(enum_value, type, fmt, read_type) case PrimitiveType::enum_value: return std::to_string(getStaticValue<type>()); break;
	switch (type->primitive_type) {
		PrimitiveTypes
		case PrimitiveType::string: return getStaticValue<std::string>(); break;
		default: Assert(1 == 0);  return "Field not a primitive type";
	}
}

void Reflection::Field::setValueFromStringInternal(void *instance, const char *value) {
#define PrimType(enum_value, type, fmt, read_type) case PrimitiveType::enum_value: { \
	read_type new_value; \
	std::sscanf(value, fmt, &new_value); \
	type *value_ptr = (type *)instance; \
	*value_ptr = new_value; \
	} break; 

	switch (type->primitive_type) {
		PrimitiveTypes
		case PrimitiveType::string: {
			std::string *str = (std::string *)instance;
			*str = value;
		} break;

		default: Assert(1 == 0);  /* Field is not of primitive type */;
	}
}

void Reflection::Field::setStaticValueFromString(const char *value) {
	setValueFromStringInternal(static_address, value);
}


void Reflection::Field::setValueFromString(void *instance, const char *value) {
	u8 *walker = (u8 *)instance;
	walker += offset;
	setValueFromStringInternal(walker, value);
}
