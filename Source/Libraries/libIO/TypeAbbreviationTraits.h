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

#define DEF_ABBREVIATION(TYPE, STR) \
		template<> \
		static const char* get(const TYPE&) { \
			static const char* const val = STR; \
			return val; \
		}

struct TypeAbbreviations {
	template<class T>
	static const char* get(const T& v) { 
		static const char * const val = "invalid";
		return val;
	}

	DEF_ABBREVIATION(int8_t, "i8");
	DEF_ABBREVIATION(char, "i8");
	DEF_ABBREVIATION(uint8_t, "ui8");
	DEF_ABBREVIATION(unsigned char, "ui8");
	DEF_ABBREVIATION(int16_t, "i16");
	DEF_ABBREVIATION(uint16_t, "ui16");
	DEF_ABBREVIATION(int32_t, "i32");
	DEF_ABBREVIATION(uint32_t, "ui32");
	DEF_ABBREVIATION(float, "flt");
	DEF_ABBREVIATION(double, "dbl");
	DEF_ABBREVIATION(String, "str");
};

CLOSE_NAMESPACE(Elf);

#endif