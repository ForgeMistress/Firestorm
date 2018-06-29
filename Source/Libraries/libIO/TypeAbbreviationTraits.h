///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  JSONDocumentReader.h
//
//  IDocumentReader implementation that is designed to read JSON formatted documents.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_TYPEABBREVIATIONTRAITS_H_
#define LIBIO_TYPEABBREVIATIONTRAITS_H_
#pragma once

OPEN_NAMESPACE(Elf);

#define DEF_GET_STR(TYPE, STR) \
	template<> \
	static const char* getStr(const TYPE& t) { \
		static const char* const val = STR; \
		return val; \
	}

#define DEF_GET_TYPE(TYPE, STR) \
	if(typestr == String( STR )) { \
		return Mirror::Type::get< TYPE >(); \
	}
		

struct TypeAbbreviations {
	template<class T>
	static const char* getStr(const T& v) {
		static const char * const val = "invalid";
		return val;
	}
	DEF_GET_STR(int8_t, "i8");
	DEF_GET_STR(uint8_t, "ui8");
	DEF_GET_STR(int16_t, "i16");
	DEF_GET_STR(uint16_t, "ui16");
	DEF_GET_STR(int32_t, "i32");
	DEF_GET_STR(uint32_t, "ui32");
	DEF_GET_STR(float, "flt");
	DEF_GET_STR(double, "dbl");
	DEF_GET_STR(String, "str");

	static Mirror::Type getType(const char* typeCharArr) {
		String typestr(typeCharArr);
		DEF_GET_TYPE(int8_t, "i8");
		DEF_GET_TYPE(uint8_t, "ui8");
		DEF_GET_TYPE(int16_t, "i16");
		DEF_GET_TYPE(uint16_t, "ui16");
		DEF_GET_TYPE(int32_t, "i32");
		DEF_GET_TYPE(uint32_t, "ui32");
		DEF_GET_TYPE(float, "flt");
		DEF_GET_TYPE(double, "dbl");
		DEF_GET_TYPE(String, "str");
		return rttr::detail::get_invalid_type();
	}
};

CLOSE_NAMESPACE(Elf);

#endif