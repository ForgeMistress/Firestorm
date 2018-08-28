///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TypeTraits.h
//
//  Contains the traits for types within static templated structs.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Elflord 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef LIBIO_TYPETRAITS_H_
#define LIBIO_TYPETRAITS_H_
#pragma once

OPEN_NAMESPACE(Elf);

/*class IDocument;

struct TraitsErrors
{
	enum Errors : int
	{
		READ_ERROR,
		WRITE_ERROR,
		KEY_NOT_FOUND,
		KEY_ALREADY_EXISTS
	};
};

struct TraitsDocs
{
	static const char* SCHEMA_KEY;
	static const char* DATA_KEY;
	static const char* OBJECT_TYPE_KEY;
	static const char* OBJECT_DATA_KEY;
};

template <class T>
struct TypeTraits
{
	static const bool IsVoid = false;
	static const bool IsPrimitive = false;
	static const bool IsNumeric = false;
	static const bool IsString = false;
	static const bool IsWritable = false;
	static const bool IsReadable = false;
	static const bool IsObject = false;
	static const bool IsPointer = false;
	static const bool IsArray = false;

	static const char*         GetAbbreviation();
	static const char*         GetTypeString();
	static const Mirror::Type  GetMirrorType();
	static Result<void, Error> ReadFromDocument(IDocument* reader, const String& key, T& outValue);
	static Result<void, Error> WriteToDocument(IDocument* writer, const String& key, T value);
};

template <>
struct TypeTraits<void>
{
	static const bool IsVoid      = true;
	static const bool IsPrimitive = true;
	static const bool IsNumeric   = false;
	static const bool IsString    = false;
	static const bool IsObject    = false;
	static const bool IsWritable  = false;
	static const bool IsReadable  = false;
	static const bool IsPointer   = false;
	static const bool IsArray     = false;

	static const char*         GetAbbreviation();
	static const char*         GetTypeString();
	static const Mirror::Type  GetMirrorType();
	static Result<void, Error> ReadFromDocument(IDocument* reader, const String& key, void* outValue);
	static Result<void, Error> WriteToDocument(IDocument* writer, const String& key, void* value);
};

template <>
struct TypeTraits<int8_t>
{
	static const bool IsVoid      = false;
	static const bool IsPrimitive = true;
	static const bool IsNumeric   = true;
	static const bool IsString    = false;
	static const bool IsObject    = false;
	static const bool IsWritable  = true;
	static const bool IsReadable  = true;
	static const bool IsPointer   = false;
	static const bool IsArray     = false;

	static const char*         GetAbbreviation();
	static const char*         GetTypeString();
	static const Mirror::Type  GetMirrorType();

	static Result<void, Error> ReadFromDocument(IDocument* reader, const String& key, int8_t& outValue);
	static Result<void, Error> WriteToDocument(IDocument* writer, const String& key, int8_t value);
};

template <>
struct TypeTraits<uint8_t>
{
	static const bool IsVoid      = false;
	static const bool IsPrimitive = true;
	static const bool IsNumeric   = true;
	static const bool IsString    = false;
	static const bool IsObject    = false;
	static const bool IsWritable  = true;
	static const bool IsReadable  = true;
	static const bool IsPointer   = false;
	static const bool IsArray = false;

	static const char*         GetAbbreviation();
	static const char*         GetTypeString();
	static const Mirror::Type  GetMirrorType();
	static Result<void, Error> ReadFromDocument(IDocument* reader, const String& key, uint8_t& outValue);
	static Result<void, Error> WriteToDocument(IDocument* writer, const String& key, uint8_t value);
};

template <>
struct TypeTraits<int16_t>
{
	static const bool IsVoid = false;
	static const bool IsPrimitive = true;
	static const bool IsNumeric = true;
	static const bool IsString = false;
	static const bool IsObject = false;
	static const bool IsWritable = true;
	static const bool IsReadable = true;
	static const bool IsPointer = false;
	static const bool IsArray = false;

	static const char*         GetAbbreviation();
	static const char*         GetTypeString();
	static const Mirror::Type  GetMirrorType();
	static Result<void, Error> ReadFromDocument(IDocument* reader, const String& key, int16_t& outValue);
	static Result<void, Error> WriteToDocument(IDocument* writer, const String& key, int16_t value);
};

template <>
struct TypeTraits<Mirror::Object*>
{
	static const bool IsVoid = false;
	static const bool IsPrimitive = false;
	static const bool IsNumeric = false;
	static const bool IsString = false;
	static const bool IsObject = true;
	static const bool IsWritable = true;
	static const bool IsReadable = true;
	static const bool IsPointer = true;
	static const bool IsArray = false;

	static const char*         GetAbbreviation();
	static const char*         GetTypeString();
	static const Mirror::Type  GetMirrorType();
	static Result<void, Error> ReadFromDocument(IDocument* reader, const String& key, Mirror::Object* outValue);
	static Result<void, Error> WriteToDocument(IDocument* writer, const String& key, const Mirror::Object* value);

	static void RecursiveWrite(IDocument* writer, const String& key, const Mirror::Object* value);
};

#define DEF_GET_STR(TYPE, STR) \
	template<> \
	static const char* GetString(const TYPE& ) { \
		return TypeAbbreviations::GetTypeAbbreviation< TYPE >(); \
	} \
	template <> \
	static const char* GetTypeAbbreviation< TYPE >() { \
		static const char* const val = STR; \
		return val; \
	}

#define DEF_GET_TYPE(TYPE, STR) \
	if(typestr == String( STR )) { \
		return rttr::type::get< TYPE >(); \
	}
		

struct TypeAbbreviations {
	template<class T>
	static const char* GetString(const T& ) {
		return TypeAbbreviations::GetTypeAbbreviation<T>();
	}
	template <class T>
	static const char* GetTypeAbbreviation() {
		static const char * const val = "invalid";
		return val;
	}
	DEF_GET_STR(int8_t, "i8");
	DEF_GET_STR(uint8_t, "ui8");
	DEF_GET_STR(int16_t, "i16");
	DEF_GET_STR(uint16_t, "u16");
	DEF_GET_STR(int32_t, "i32");
	DEF_GET_STR(uint32_t, "u32");
	DEF_GET_STR(int64_t, "i32");
	DEF_GET_STR(uint64_t, "u32");
	DEF_GET_STR(float, "flt");
	DEF_GET_STR(double, "dbl");
	DEF_GET_STR(String, "str");

	static Mirror::Type getType(const char* typeCharArr) {
		String typestr(typeCharArr);
		DEF_GET_TYPE(int8_t, "i8");
		DEF_GET_TYPE(uint8_t, "u8");
		DEF_GET_TYPE(int16_t, "i16");
		DEF_GET_TYPE(uint16_t, "u16");
		DEF_GET_TYPE(int32_t, "i32");
		DEF_GET_TYPE(uint32_t, "ui32");
		DEF_GET_TYPE(int64_t, "i32");
		DEF_GET_TYPE(uint64_t, "u32");
		DEF_GET_TYPE(float, "flt");
		DEF_GET_TYPE(double, "dbl");
		DEF_GET_TYPE(String, "str");
		return rttr::detail::get_invalid_type();
	}
};*/

CLOSE_NAMESPACE(Elf);

#endif