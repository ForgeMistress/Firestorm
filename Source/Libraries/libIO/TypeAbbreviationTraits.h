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

#define DEF_ABBREVIATION(TYPE, STR, VAL) \
		template<> \
		static const char* getStr(const TYPE& t) { \
			static const char* const val = STR; \
			return val; \
		} \
		template<> \
		static uint8_t getVal<TYPE>(const TYPE& t) {\
			return VAL; \
		} \
		template<> \
		static Mirror::Type getType<VAL>() { \
			return Mirror::Type::get<TYPE>(); \
		}
		

struct TypeAbbreviations {
	static const uint8_t TYPE_ENUM = 0;
	template<class T>
	static const char* getStr(const T& v) {
		static const char * const val = "invalid";
		return val;
	}
	template<class T>
	static constexpr uint8_t getVal(const T& v) {
		return 0;
	}
	template<uint8_t T>
	static constexpr Mirror::Type getType() {
		return Mirror::Type::get<void>();
	}
	/*template<>
	static Mirror::Type getType<1>() {
		return Mirror::Type::get<char>();
	}*/


	DEF_ABBREVIATION(int8_t, "i8", 1);
	DEF_ABBREVIATION(char, "i8", 2);
	DEF_ABBREVIATION(uint8_t, "ui8", 3);
	// DEF_ABBREVIATION(unsigned char, "ui8", 4);
	DEF_ABBREVIATION(int16_t, "i16", 5);
	DEF_ABBREVIATION(uint16_t, "ui16", 6);
	DEF_ABBREVIATION(int32_t, "i32", 7);
	DEF_ABBREVIATION(uint32_t, "ui32", 8);
	DEF_ABBREVIATION(float, "flt", 9);
	DEF_ABBREVIATION(double, "dbl", 10);
	DEF_ABBREVIATION(String, "str", 11);
};

CLOSE_NAMESPACE(Elf);

#endif