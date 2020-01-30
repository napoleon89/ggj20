#pragma once

#include <core/std.h>
#include <core/math.h>

namespace Reflection {

	enum class PrimitiveType {
		None,
		s8,
		s16,
		s32,
		s64,
		u8,
		u16,
		u32,
		u64,
		f32,
		f64,
		boolean,
		string,
	};

	struct Type {
		const char *name;
		const char *format;
		size_t size;
		bool is_primitive;
		PrimitiveType primitive_type;
	};

	struct Field {
		const Type *type;
		const char *name;
		size_t offset;
		void *static_address;

		inline bool isStatic() const {
			return static_address != nullptr;
		}

		template<typename T>
		void setValue(void *instance, T value) {
			u8 *walker = (u8 *)instance;
			walker += offset;
			memcpy(walker, &value, type->size);
		}

		template<typename T>
		T getValue(void *instance) const {
			u8 *walker = (u8 *)instance;
			walker += offset;
			T result = *(T *)walker;
			return result;
		}

		template<typename T>
		T getStaticValue() const {
			const T value = *(T *)static_address;
			return value;
		}

		void setStaticValueFromString(const char *value);
		void setValueFromString(void *instance, const char *value);

		const std::string getStaticValueAsString() const;

	private:
		void setValueFromStringInternal(void *instance, const char *value);
	};

	struct Class : public Type {
		Field *fields;
		u32 field_count;

		void print();
		Field *findField(const char *name);
	};

	template<typename T>
	Class *getStruct();

	template<typename T>
	Class *getStruct(const T *obj) {
		return getStruct<T>();
	}

	template<typename T>
	void printStruct(const T *obj) {
		Class *class_type = getStruct<T>();
		class_type->print();
	}

	template<typename T>
	const Type *getType();
}