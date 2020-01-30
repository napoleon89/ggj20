
#define EnumValue(name, number) name = number,

enum class EnumName : EnumType {
	EnumValues
};

#undef EnumValue
#define EnumValue(name, number) case EnumName::name: return #name; break;

inline std::string getStrFromEnumValue(EnumName tag_value) { 
	switch(tag_value) {
		EnumValues
		default: Assert(1 == 0); return ""; break;
	}
}

#undef EnumValue
#undef EnumName
#undef EnumType
#undef EnumValues