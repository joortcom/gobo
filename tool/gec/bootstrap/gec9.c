#include "gec.h"

#ifdef __cplusplus
extern "C" {
#endif

int GE_main(int argc, EIF_NATIVE_CHAR** argv)
{
	T0* t1;
	GE_context tc = GE_default_context;
	GE_context* ac = &tc;
	GE_type_info_count = 2263;
	GE_argc = argc;
	GE_argv = argv;
	GE_main_context = ac;
	GE_system_name = "gec";
	GE_root_class_name = "GEC";
	GE_init_gc();
	GE_new_exception_manager = &GE_new21;
	GE_init_exception_manager = &T21f14;
	GE_last_exception = &T21f1;
	GE_once_raise = &T21f15;
	GE_set_exception_data = &T21f16;
	GE_init_onces(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	GE_once_per_object_data_mutex = GE_mutex_create();
	GE_thread_onces_set_counts(4, 1, 1, 0, 0, 10, 0, 0, 0, 0, 4, 0, 0, 0, 953, 0);
	GE_init_thread(ac);
	GE_init_exception(ac);
	GE_init_console();
	GE_init_identified();
	GE_init_const();
	t1 = T25c61(ac);
	return 0;
}


#ifdef __cplusplus
}
#endif
/*
	description:

		"C functions used to manipulate strings"

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2016-2019, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef GE_STRING_C
#define GE_STRING_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef GE_STRING_H
#include "ge_string.h"
#endif
#ifndef GE_NATIVE_STRING_H
#include "ge_native_string.h"
#endif
#ifndef GE_GC_H
#include "ge_gc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Number of EIF_CHARACTER_32 characters which can be read from
 * the first `n' native characters in native string `s'.
 * Invalid native characters are escaped.
 */
static int GE_nstr_str32len(EIF_NATIVE_CHAR* s, EIF_INTEGER n)
{
	int i, j;
#ifdef EIF_WINDOWS
	uint16_t c1, c2;
#else
	uint8_t c1, c2, c3, c4;
#endif

	j = 0;
	for (i = 0; i < n; i++) {
#ifdef EIF_WINDOWS
		c1 = (uint16_t)s[i];
		if (c1 == 0) {
			return j;
		} else if (c1 < 0xD800 || c1 >= 0xE000) {
				/* Codepoint from Basic Multilingual Plane: one 16-bit code unit. */
			j++;
		} else if (c1 <= 0xDBFF && i + 1 < n) {
				/* Check if a lead surrogate (value between 0xD800 and 0xDBFF) is followed by a trail surrogate. */
			c2 = (uint16_t)s[i + 1];
			if (c2 >= 0xDC00 && c2 <= 0xDFFF) {
				/* Supplementary Planes: surrogate pair with lead and trail surrogates. */
				i++;
				j++;
			} else {
				j += 6;
			}
		} else {
			j += 6;
		}
#else
		c1 = (uint8_t)s[i];
		if (c1 == 0) {
			return j;
		} else if (c1 <= 0x7F) {
			j++;
		} else if ((c1 & 0xE0) == 0xC0) {
			if (i + 1 < n) {
				c2 = (uint8_t)s[i + 1];
				if ((c2 & 0xC0) == 0x80) {
					j++;
					i++;
				} else {
					j += 3;
				}
			} else {
				j += 3;
			}
		} else if ((c1 & 0xF0) == 0xE0) {
			if (i + 2 < n) {
				c2 = (uint8_t)s[i + 1];
				c3 = (uint8_t)s[i + 2];
				if ((c2 & 0xC0) == 0x80 && (c3 & 0xC0) == 0x80) {
					j++;
					i +=2;
				} else {
					j += 3;
				}
			} else {
				j += 3;
			}
		} else if ((c1 & 0xF8) == 0xF0) {
			if (i + 3 < n) {
				c2 = (uint8_t)s[i + 1];
				c3 = (uint8_t)s[i + 2];
				c4 = (uint8_t)s[i + 3];
				if ((c2 & 0xC0) == 0x80 && (c3 & 0xC0) == 0x80 && (c4 & 0xC0) == 0x80) {
					j++;
					i +=3;
				} else {
					j += 3;
				}
			} else {
				j += 3;
			}
		} else {
			j += 3;
		}
#endif
	}
	return j;
}

/*
 * Hexadecimal representation of `a_code'.
 * `a_code' is expected to be between 0 and 15.
 */
static char GE_to_hex(uint16_t a_code)
{
	switch (a_code) {
	case 0:
		return '0';
	case 1:
		return '1';
	case 2:
		return '2';
	case 3:
		return '3';
	case 4:
		return '4';
	case 5:
		return '5';
	case 6:
		return '6';
	case 7:
		return '7';
	case 8:
		return '8';
	case 9:
		return '9';
	case 10:
		return 'A';
	case 11:
		return 'B';
	case 12:
		return 'C';
	case 13:
		return 'D';
	case 14:
		return 'E';
	default:
		return 'F';
	}
}

/*
 * Copy escaped version of `c' to `a_buffer'.
 * `a_buffer' is expected to have enough space for 6 characters.
 */
static void GE_uint16_to_escaped_str32(uint16_t c, EIF_CHARACTER_32* a_buffer)
{
	int i = 0;

	a_buffer[i] = (EIF_CHARACTER_32)0x0000FFFD;
	i++;
	a_buffer[i] = (EIF_CHARACTER_32)'u';
	i++;
	a_buffer[i] = (EIF_CHARACTER_32)GE_to_hex((c & 0xF000) >> 12);
	i++;
	a_buffer[i] = (EIF_CHARACTER_32)GE_to_hex((c & 0x0F00) >> 8);
	i++;
	a_buffer[i] = (EIF_CHARACTER_32)GE_to_hex((c & 0x00F0) >> 4);
	i++;
	a_buffer[i] = (EIF_CHARACTER_32)GE_to_hex(c & 0x000F);
}

#ifndef __LCC__
/* lcc-win32 complains about this used static function. */

/*
 * Copy escaped version of `c' to `a_buffer'.
 * `a_buffer' is expected to have enough space for 3 characters.
 */
static void GE_uint8_to_escaped_str32(uint8_t c, EIF_CHARACTER_32* a_buffer)
{
	int i = 0;

	a_buffer[i] = (EIF_CHARACTER_32)0x0000FFFD;
	i++;
	a_buffer[i] = (EIF_CHARACTER_32)GE_to_hex((uint16_t)((c & 0xF0) >> 4));
	i++;
	a_buffer[i] = (EIF_CHARACTER_32)GE_to_hex((uint16_t)(c & 0x0F));
}
#endif

/*
 * Copy to `a_buffer' the EIF_CHARACTER_32 characters corresponding to the
 * first `n' native characters in the native string `s'.
 * `a_buffer' is expected to have enough space.
 * Invalid native characters are escaped.
 */
static void GE_nstr_to_str32(EIF_NATIVE_CHAR* s, EIF_CHARACTER_32* a_buffer, EIF_INTEGER n)
{
	int i, j;
#ifdef EIF_WINDOWS
	uint16_t c1, c2;
#else
	uint8_t c1, c2, c3, c4;
#endif

	j = 0;
	for (i = 0; i < n; i++) {
#ifdef EIF_WINDOWS
		c1 = (uint16_t)s[i];
		if (c1 < 0xD800 || c1 >= 0xE000) {
				/* Codepoint from Basic Multilingual Plane: one 16-bit code unit. */
			a_buffer[j] = (EIF_CHARACTER_32)(c1);
			j++;
		} else if (c1 <= 0xDBFF && i + 1 < n) {
				/* Check if a lead surrogate (value between 0xD800 and 0xDBFF) is followed by a trail surrogate. */
			c2 = (uint16_t)s[i + 1];
			if (c2 >= 0xDC00 && c2 <= 0xDFFF) {
					/* Supplementary Planes: surrogate pair with lead and trail surrogates. */
				a_buffer[j] = (EIF_CHARACTER_32)(((uint32_t)c1 << 10) + (uint32_t)c2 - 0x035FDC00);
				j++;
				i++;
			} else {
				GE_uint16_to_escaped_str32(c1, a_buffer + j);
				j += 6;
			}
		} else {
			GE_uint16_to_escaped_str32(c1, a_buffer + j);
			j += 6;
		}
#else
		c1 = (uint8_t)s[i];
		if (c1 <= 0x7F) {
			a_buffer[j] = (EIF_CHARACTER_32)(c1);
			j++;
		} else if ((c1 & 0xE0) == 0xC0) {
			if (i + 1 < n) {
				c2 = (uint8_t)s[i + 1];
				if ((c2 & 0xC0) == 0x80) {
					a_buffer[j] = (EIF_CHARACTER_32)((((uint32_t)c1 & 0x0000001F) << 6) | ((uint32_t)c2 & 0x0000003F));
					j++;
					i++;
				} else {
					GE_uint8_to_escaped_str32(c1, a_buffer + j);
					j += 3;
				}
			} else {
				GE_uint8_to_escaped_str32(c1, a_buffer + j);
				j += 3;
			}
		} else if ((c1 & 0xF0) == 0xE0) {
			if (i + 2 < n) {
				c2 = (uint8_t)s[i + 1];
				c3 = (uint8_t)s[i + 2];
				if ((c2 & 0xC0) == 0x80 && (c3 & 0xC0) == 0x80) {
					a_buffer[j] = (EIF_CHARACTER_32)((((uint32_t)c1 & 0x0000000F) << 12) | (((uint32_t)c2 & 0x0000003F) << 6) | ((uint32_t)c3 & 0x0000003F));
					j++;
					i +=2;
				} else {
					GE_uint8_to_escaped_str32(c1, a_buffer + j);
					j += 3;
				}
			} else {
				GE_uint8_to_escaped_str32(c1, a_buffer + j);
				j += 3;
			}
		} else if ((c1 & 0xF8) == 0xF0) {
			if (i + 3 < n) {
				c2 = (uint8_t)s[i + 1];
				c3 = (uint8_t)s[i + 2];
				c4 = (uint8_t)s[i + 3];
				if ((c2 & 0xC0) == 0x80 && (c3 & 0xC0) == 0x80 && (c4 & 0xC0) == 0x80) {
					a_buffer[j] = (EIF_CHARACTER_32)((((uint32_t)c1 & 0x00000007) << 18) | (((uint32_t)c2 & 0x0000003F) << 12) | (((uint32_t)c3 & 0x0000003F) << 6) | ((uint32_t)c4 & 0x0000003F));
					j++;
					i +=3;
				} else {
					GE_uint8_to_escaped_str32(c1, a_buffer + j);
					j += 3;
				}
			} else {
				GE_uint8_to_escaped_str32(c1, a_buffer + j);
				j += 3;
			}
		} else {
			GE_uint8_to_escaped_str32(c1, a_buffer + j);
			j += 3;
		}
#endif
	}
}

/*
 * Copy to `a_buffer' the EIF_CHARACTER_32 characters corresponding to the
 * first `n' characters in the ISO 8859-1 string `s'.
 * `a_buffer' is expected to have enough space for `n' characters.
 */
static void GE_str8_to_str32(const char* s, EIF_CHARACTER_32* a_buffer, EIF_INTEGER n)
{
	int i;
	for (i = 0; i < n; i++) {
		a_buffer[i] = (EIF_CHARACTER_32)s[i];
	}
}

/*
 * New Eiffel string of type "STRING_8" containing the
 * first `c' characters found in ISO 8859-1 string `s'.
 */
EIF_REFERENCE GE_ms8(const char* s, EIF_INTEGER c)
{
	EIF_STRING* l_string;
	EIF_SPECIAL* l_area;
	EIF_CHARACTER_8* l_area_base_address;

	l_string = (EIF_STRING*)GE_new_str8(c);
	l_area = (EIF_SPECIAL*)(l_string->area);
	l_area_base_address = (EIF_CHARACTER_8*)((char*)l_area + l_area->offset);
	memcpy((char*)l_area_base_address, s, c);
#ifndef GE_alloc_atomic_cleared
	*(l_area_base_address + c) = (EIF_CHARACTER_8)'\0';
#endif
	l_area->count = (c + 1);
	l_string->count = c;
	return (EIF_REFERENCE)l_string;
}

/*
 * New Eiffel string of type "STRING_8" containing all
 * characters found in the null-terminated ISO 8859-1 string `s'.
 */
EIF_REFERENCE GE_str8(const char* s)
{
	return GE_ms8(s, strlen(s));
}

/*
 * New Eiffel string of type "IMMUTABLE_STRING_8" containing the
 * first `c' characters found in ISO 8859-1 string `s'.
 */
EIF_REFERENCE GE_ims8(const char* s, EIF_INTEGER c)
{
	EIF_STRING* l_string;
	EIF_SPECIAL* l_area;
	EIF_CHARACTER_8* l_area_base_address;

	l_string = (EIF_STRING*)GE_new_istr8(c);
	l_area = (EIF_SPECIAL*)(l_string->area);
	l_area_base_address = (EIF_CHARACTER_8*)((char*)l_area + l_area->offset);
	memcpy((char*)l_area_base_address, s, c);
#ifndef GE_alloc_atomic_cleared
	*(l_area_base_address + c) = (EIF_CHARACTER_8)'\0';
#endif
	l_area->count = (c + 1);
	l_string->count = c;
	return (EIF_REFERENCE)l_string;
}

/*
 * New Eiffel string of type "STRING_32" containing the
 * first `c' characters found in ISO 8859-1 string `s'.
 */
EIF_REFERENCE GE_ms32(const char* s, EIF_INTEGER c)
{
	EIF_STRING* l_string;
	EIF_SPECIAL* l_area;
	EIF_CHARACTER_32* l_area_base_address;

	l_string = (EIF_STRING*)GE_new_str32(c);
	l_area = (EIF_SPECIAL*)(l_string->area);
	l_area_base_address = (EIF_CHARACTER_32*)((char*)l_area + l_area->offset);
	GE_str8_to_str32(s, l_area_base_address, c);
#ifndef GE_alloc_atomic_cleared
	*(l_area_base_address + c) = (EIF_CHARACTER_32)'\0';
#endif
	l_area->count = (c + 1);
	l_string->count = c;
	return (EIF_REFERENCE)l_string;
}

/*
 * New Eiffel string of type "STRING_32" containing the
 * first `c' 32-bit characters built from `s' by reading
 * groups of four bytes with little-endian byte order.
 */
EIF_REFERENCE GE_ms32_from_utf32le(const char* s, EIF_INTEGER c)
{
	EIF_STRING* l_string;
	EIF_SPECIAL* l_area;
	EIF_CHARACTER_32* l_area_base_address;

	l_string = (EIF_STRING*)GE_new_str32(c);
	l_area = (EIF_SPECIAL*)(l_string->area);
	l_area_base_address = (EIF_CHARACTER_32*)((char*)l_area + l_area->offset);
#if BYTEORDER == 0x1234
	memcpy((EIF_CHARACTER_32*)l_area_base_address, s, c * 4);
#else
	{
		int i;
		EIF_CHARACTER_32 l_little, l_big;
		for (i = 0; i < c ; i++) {
			memcpy(&l_little, s + (i * 4), 4);
				/* Convert our little endian to big endian. */
			l_big = ((l_little >> 24) & 0xFF) |
				((l_little >> 8) & 0xFF00) |
			   	((l_little << 8) & 0xFF0000) |
			   	((l_little << 24) & 0xFF000000);
			l_area_base_address[i] = l_big;
		}
	}
#endif
#ifndef GE_alloc_atomic_cleared
	*(l_area_base_address + c) = (EIF_CHARACTER_32)'\0';
#endif
	l_area->count = (c + 1);
	l_string->count = c;
	return (EIF_REFERENCE)l_string;
}

/*
 * New Eiffel string of type "STRING_32" containing all
 * characters found in the null-terminated ISO 8859-1 string `s'.
 */
EIF_REFERENCE GE_str32(const char* s)
{
	return GE_ms32(s, strlen(s));
}

/*
 * New Eiffel string of type "IMMUTABLE_STRING_32" containing
 * the first `c' characters found in ISO 8859-1 string `s'.
 */
EIF_REFERENCE GE_ims32(const char* s, EIF_INTEGER c)
{
	EIF_STRING* l_string;
	EIF_SPECIAL* l_area;
	EIF_CHARACTER_32* l_area_base_address;

	l_string = (EIF_STRING*)GE_new_istr32(c);
	l_area = (EIF_SPECIAL*)(l_string->area);
	l_area_base_address = (EIF_CHARACTER_32*)((char*)l_area + l_area->offset);
	GE_str8_to_str32(s, l_area_base_address, c);
#ifndef GE_alloc_atomic_cleared
	*(l_area_base_address + c) = (EIF_CHARACTER_32)'\0';
#endif
	l_area->count = (c + 1);
	l_string->count = c;
	return (EIF_REFERENCE)l_string;
}

/*
 * New Eiffel string of type "IMMUTABLE_STRING_32" containing the
 * first `c' 32-bit characters built from `s' by reading
 * groups of four bytes with little-endian byte order.
 */
EIF_REFERENCE GE_ims32_from_utf32le(const char* s, EIF_INTEGER c)
{
	EIF_STRING* l_string;
	EIF_SPECIAL* l_area;
	EIF_CHARACTER_32* l_area_base_address;

	l_string = (EIF_STRING*)GE_new_istr32(c);
	l_area = (EIF_SPECIAL*)(l_string->area);
	l_area_base_address = (EIF_CHARACTER_32*)((char*)l_area + l_area->offset);
#if BYTEORDER == 0x1234
	memcpy((EIF_CHARACTER_32*)l_area_base_address, s, c * 4);
#else
	{
		int i;
		EIF_CHARACTER_32 l_little, l_big;
		for (i = 0; i < c ; i++) {
			memcpy(&l_little, s + (i * 4), 4);
				/* Convert our little endian to big endian. */
			l_big = ((l_little >> 24) & 0xFF) |
				((l_little >> 8) & 0xFF00) |
			   	((l_little << 8) & 0xFF0000) |
			   	((l_little << 24) & 0xFF000000);
			l_area_base_address[i] = l_big;
		}
	}
#endif
#ifndef GE_alloc_atomic_cleared
	*(l_area_base_address + c) = (EIF_CHARACTER_32)'\0';
#endif
	l_area->count = (c + 1);
	l_string->count = c;
	return (EIF_REFERENCE)l_string;
}

/*
 * New Eiffel string of type "IMMUTABLE_STRING_32" containing all
 * characters found in the null-terminated ISO 8859-1 string `s'.
 */
EIF_REFERENCE GE_istr32(const char* s)
{
	return GE_ims32(s, strlen(s));
}

/*
 * New Eiffel string of type "IMMUTABLE_STRING_32" containing the
 * first `n' native characters found in native string `s'.
 * Invalid native characters are escaped.
 */
EIF_REFERENCE GE_ims32_from_nstr(EIF_NATIVE_CHAR* s, EIF_INTEGER n)
{
	EIF_STRING* l_string;
	EIF_SPECIAL* l_area;
	EIF_CHARACTER_32* l_area_base_address;
	EIF_INTEGER c;

	c = GE_nstr_str32len(s, n);
	l_string = (EIF_STRING*)GE_new_istr32(c);
	l_area = (EIF_SPECIAL*)(l_string->area);
	l_area_base_address = (EIF_CHARACTER_32*)((char*)l_area + l_area->offset);
	GE_nstr_to_str32(s, l_area_base_address, n);
#ifndef GE_alloc_atomic_cleared
	*(l_area_base_address + c) = (EIF_CHARACTER_32)'\0';
#endif
	l_area->count = (c + 1);
	l_string->count = c;
	return (EIF_REFERENCE)l_string;
}

/*
 * New Eiffel string of type "IMMUTABLE_STRING_32" containing all
 * characters found in the null-terminated native string `s'.
 * Invalid native characters are escaped.
 */
EIF_REFERENCE GE_istr32_from_nstr(EIF_NATIVE_CHAR* s)
{
	return GE_ims32_from_nstr(s, GE_nstrlen(s));
}

/*
 * New Eiffel string of type "STRING" containing all
 * characters found in the null-terminated ISO 8859-1 string `s'
 */
EIF_REFERENCE GE_str(const char* s)
{
	return GE_ms(s, strlen(s));
}

#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"C functions used to implement type information"

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2016-2023, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef GE_TYPES_C
#define GE_TYPES_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef GE_TYPES_H
#include "ge_types.h"
#endif
#ifndef GE_STRING_H
#include "ge_string.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Number of type infos in `GE_type_infos'.
 * Do not take into account the fake item at index 0.
 */
int GE_type_info_count;

/*
 * Encode a EIF_TYPE into a EIF_ENCODED_TYPE.
 * The lower part of EIF_ENCODED_TYPE contains the .id field,
 * and the upper part the .annotations.
 */
EIF_ENCODED_TYPE GE_encoded_type(EIF_TYPE a_type)
{
	EIF_ENCODED_TYPE l_result;

#if defined(_MSC_VER)
		/* This code below is just optimized as one move by cl on x86 platforms.
		 * The else-part below generates non-optimal code with cl.
		 */
	memcpy(&l_result, &a_type, sizeof(EIF_ENCODED_TYPE));
#else
		/* This code below is just optimized as one move by gcc/clang on x86 platforms. */
	l_result = a_type.annotations;
	l_result = (l_result << 16) | a_type.id;
#endif
	return l_result;
}

/*
 * Decode a EIF_ENCODED_TYPE into a EIF_TYPE.
 * The lower part of EIF_ENCODED_TYPE contains the .id field,
 * and the upper part the .annotations.
 */
EIF_TYPE GE_decoded_type(EIF_ENCODED_TYPE a_type)
{
	EIF_TYPE l_result;

#if defined(_MSC_VER)
		/* This code below is just optimized as one move by cl on x86 platforms.
		 * The else-part below generates non-optimal code with cl.
		 */
	memcpy(&l_result, &a_type, sizeof(EIF_TYPE));
#else
		/* This code below is just optimized as one move by gcc/clang on x86 platforms. */
	l_result.id = a_type & 0x0000FFFF;
	l_result.annotations = a_type >> 16;
#endif
	return l_result;
}

/*
 * Type with `a_id' and `a_annotations'.
 */
EIF_TYPE GE_new_type(EIF_TYPE_INDEX a_id, EIF_TYPE_INDEX a_annotations)
{
	EIF_TYPE l_result;
	l_result.id = a_id;
	l_result.annotations = a_annotations;
	return l_result;
}

/*
 * Associated detachable type of `a_type' if any,
 * otherwise `a_type'.
 */
EIF_TYPE GE_non_attached_type(EIF_TYPE a_type)
{
		/* Since types are by default detachable, we simply remove
		 * all attachment marks. */
	a_type.annotations &= ~(ATTACHED_FLAG | DETACHABLE_FLAG);
	return a_type;
}

/*
 * Associated attached type of `a_type' if any,
 * otherwise `a_type'.
 */
EIF_TYPE GE_attached_type(EIF_TYPE a_type)
{
	if (!GE_is_expanded_type_index(a_type.id)) {
		a_type.annotations &= ~DETACHABLE_FLAG;
		a_type.annotations |= ATTACHED_FLAG;
	}
	return a_type;
}

/*
 * Is `a_type' a SPECIAL type of user-defined expanded type?
 */
EIF_BOOLEAN GE_is_special_of_expanded_type_index(EIF_TYPE_INDEX a_type)
{
	EIF_BOOLEAN l_result = EIF_FALSE;
#ifdef GE_USE_TYPE_GENERIC_PARAMETERS
	EIF_TYPE_INDEX l_generic_parameter;
	uint32_t l_flags;

	if ((GE_type_infos[a_type].flags & GE_TYPE_FLAG_SPECIAL)) {
		l_generic_parameter = GE_decoded_type(GE_type_infos[a_type].generic_parameters[0]).id;
		l_flags = GE_type_infos[l_generic_parameter].flags;
		l_result = EIF_TEST((l_flags & GE_TYPE_FLAG_EXPANDED) && !(l_flags & GE_TYPE_FLAG_BASIC_MASK));
	}
#endif
	return l_result;
}

/*
 * Is `a_type' a SPECIAL type of reference type?
 */
EIF_BOOLEAN GE_is_special_of_reference_type_index(EIF_TYPE_INDEX a_type)
{
	EIF_BOOLEAN l_result = EIF_FALSE;
#ifdef GE_USE_TYPE_GENERIC_PARAMETERS
	EIF_TYPE_INDEX l_generic_parameter;
	uint32_t l_flags;

	if ((GE_type_infos[a_type].flags & GE_TYPE_FLAG_SPECIAL)) {
		l_generic_parameter = GE_decoded_type(GE_type_infos[a_type].generic_parameters[0]).id;
		l_flags = GE_type_infos[l_generic_parameter].flags;
		l_result = EIF_TEST(!(l_flags & GE_TYPE_FLAG_EXPANDED));
	}
#endif
	return l_result;
}

/*
 * Is `a_type' a SPECIAL type of reference type or basic expanded type?
 * (Note that user-defined expanded types are excluded.)
 */
EIF_BOOLEAN GE_is_special_of_reference_or_basic_expanded_type_index(EIF_TYPE_INDEX a_type)
{
	EIF_BOOLEAN l_result = EIF_FALSE;
#ifdef GE_USE_TYPE_GENERIC_PARAMETERS
	EIF_TYPE_INDEX l_generic_parameter;
	uint32_t l_flags;

	if ((GE_type_infos[a_type].flags & GE_TYPE_FLAG_SPECIAL)) {
		l_generic_parameter = GE_decoded_type(GE_type_infos[a_type].generic_parameters[0]).id;
		l_flags = GE_type_infos[l_generic_parameter].flags;
		l_result = EIF_TEST(!(l_flags & GE_TYPE_FLAG_EXPANDED) || (l_flags & GE_TYPE_FLAG_BASIC_MASK));
	}
#endif
	return l_result;
}

/*
 * Does `i'-th field of `a_object + a_physical_offset' (which is expected to be reference)
 * denote a reference with copy semantics?
 */
EIF_BOOLEAN GE_is_copy_semantics_field(EIF_INTEGER i, EIF_POINTER a_object, EIF_INTEGER a_physical_offset)
{
	EIF_REFERENCE l_object;

	l_object = GE_reference_field(i, a_object, a_physical_offset);
	if (l_object) {
		return GE_is_expanded_object(l_object);
	} else {
		return EIF_FALSE;
	}
}

/*
 * Does `i'-th item of special `a_object' (which is expected to be reference)
 * denote a reference with copy semantics?
 */
EIF_BOOLEAN GE_is_special_copy_semantics_item(EIF_INTEGER i, EIF_POINTER a_object)
{
#if defined(GE_USE_ATTRIBUTES) && defined(GE_USE_ATTRIBUTE_OFFSET)
	EIF_REFERENCE l_object;
	GE_type_info l_type_info;

	l_type_info = GE_type_infos[((EIF_REFERENCE)a_object)->id];
		/* The last pseudo attribute is the item at index 0 in the special object */
	l_object = *(EIF_REFERENCE*)((char*)a_object + l_type_info.attributes[l_type_info.attribute_count - 1]->offset + i * sizeof(EIF_REFERENCE));
	if (l_object) {
		return GE_is_expanded_object(l_object);
	} else {
		return EIF_FALSE;
	}
#else
	return EIF_FALSE;
#endif
}

/*
 * Generator class name of `a_type'.
 */
EIF_REFERENCE GE_generator_of_type_index(EIF_TYPE_INDEX a_type)
{
	const char* l_generator;
#ifdef GE_USE_TYPE_GENERATOR
/* TODO: check that `a_type' is valid. */
	l_generator = GE_type_infos[a_type].generator;
#else
	l_generator = "";
#endif
	return GE_str(l_generator);
}

EIF_REFERENCE GE_generator_8_of_type_index(EIF_TYPE_INDEX a_type)
{
	const char* l_generator;
#ifdef GE_USE_TYPE_GENERATOR
/* TODO: check that `a_type' is valid. */
	l_generator = GE_type_infos[a_type].generator;
#else
	l_generator = "";
#endif
	return GE_str8(l_generator);
}

/*
 * Full name of `a_type'.
 */
EIF_REFERENCE GE_generating_type_of_encoded_type(EIF_ENCODED_TYPE a_type)
{
	const char* l_name;
#ifdef GE_USE_TYPE_NAME
/* TODO: check that `a_type' is valid. */
	EIF_TYPE l_decoded_type;

	l_decoded_type = GE_decoded_type(a_type);
	l_name = GE_type_infos[l_decoded_type.id].name;
	if (!l_decoded_type.annotations) {
		l_name++;
	}
#else
	l_name = "";
#endif
	return GE_str(l_name);
}

EIF_REFERENCE GE_generating_type_8_of_encoded_type(EIF_ENCODED_TYPE a_type)
{
	const char* l_name;
#ifdef GE_USE_TYPE_NAME
/* TODO: check that `a_type' is valid. */
	EIF_TYPE l_decoded_type;

	l_decoded_type = GE_decoded_type(a_type);
	l_name = GE_type_infos[l_decoded_type.id].name;
	if (!l_decoded_type.annotations) {
		l_name++;
	}
#else
	l_name = "";
#endif
	return GE_str8(l_name);
}

/*
 * Encoded type whose name is `a_name'.
 * -1 if no such type.
 */
EIF_ENCODED_TYPE GE_encoded_type_from_name(EIF_POINTER a_name)
{
#ifdef GE_USE_TYPE_NAME
/* TODO: check that `a_type' is valid. */
	int i;
	const char* l_name;

	for (i = 1; i <= GE_type_info_count; i++) {
		l_name = GE_type_infos[i].name;
		if (strcmp((char*)a_name, l_name + 1) == 0) {
			return (EIF_INTEGER)GE_encoded_type(GE_new_type(i, 0x0));
		} else if (strcmp((char*)a_name, l_name) == 0) {
			return (EIF_INTEGER)GE_encoded_type(GE_new_type(i, ATTACHED_FLAG));
		}
	}
#endif
	return EIF_NO_TYPE;
}

/*
 * Does `a_type_1' conform to `a_type_2'?
 */
EIF_BOOLEAN GE_encoded_type_conforms_to(EIF_ENCODED_TYPE a_type_1, EIF_ENCODED_TYPE a_type_2)
{
#ifdef GE_USE_ANCESTORS
	GE_type_info l_type_info_1, l_type_info_2;
	GE_ancestor** l_ancestors;
	uint32_t l_ancestor_count, i;
	EIF_TYPE l_decoded_type_1, l_decoded_type_2;
	EIF_TYPE_INDEX l_type_index_1, l_type_index_2, l_ancestor_type_index;
	uint32_t l_flags_1, l_flags_2;

	l_decoded_type_1 = GE_decoded_type(a_type_1);
	l_decoded_type_2 = GE_decoded_type(a_type_2);
	l_type_index_1 = l_decoded_type_1.id;
	l_type_index_2 = l_decoded_type_2.id;
	l_type_info_1 = GE_type_infos[l_type_index_1];
	l_type_info_2 = GE_type_infos[l_type_index_2];
	l_flags_1 = l_type_info_1.flags;
	l_flags_2 = l_type_info_2.flags;
	if (!(l_flags_1 & GE_TYPE_FLAG_EXPANDED || l_decoded_type_1.annotations & ATTACHED_FLAG) && (l_flags_2 & GE_TYPE_FLAG_EXPANDED || l_decoded_type_2.annotations & ATTACHED_FLAG)) {
		return EIF_FALSE;
	} else if (l_flags_1 & GE_TYPE_FLAG_NONE) {
		return EIF_TEST(!(l_flags_2 & GE_TYPE_FLAG_EXPANDED));
	} else if (l_type_index_1 == l_type_index_2) {
		return EIF_TRUE;
	} else if (l_type_index_1 < l_type_index_2) {
		return EIF_FALSE;
	} else {
		l_ancestors = l_type_info_1.ancestors;
		l_ancestor_count = l_type_info_1.ancestor_count;
		for (i = 0; i < l_ancestor_count; i++) {
			l_ancestor_type_index = l_ancestors[i]->type_id;
			if (l_ancestor_type_index == l_type_index_2) {
				return EIF_TRUE;
			} else if (l_ancestor_type_index > l_type_index_2) {
				return EIF_FALSE;
			}
		}
	}
#endif
	return EIF_FALSE;
}

/*
 * Number of generic parameters.
 */
EIF_INTEGER GE_generic_parameter_count_of_type_index(EIF_TYPE_INDEX a_type)
{
#ifdef GE_USE_TYPE_GENERIC_PARAMETERS
	return (EIF_INTEGER)GE_type_infos[a_type].generic_parameter_count;
#else
	return (EIF_INTEGER)0;
#endif
}

/*
 * Type of `i'-th generic parameter of `a_type'.
 */
EIF_INTEGER GE_generic_parameter_of_type_index(EIF_TYPE_INDEX a_type, EIF_INTEGER i)
{
#ifdef GE_USE_TYPE_GENERIC_PARAMETERS
/* TODO: check that `a_type' and `i' are valid. */
	return (EIF_INTEGER)GE_type_infos[a_type].generic_parameters[i - 1];
#else
	return (EIF_INTEGER)0;
#endif
}

/*
 * Number of fields of an object of dynamic type `a_type'.
 */
EIF_INTEGER GE_field_count_of_type_index(EIF_TYPE_INDEX a_type)
{
#if defined(GE_USE_ATTRIBUTES) && defined(GE_USE_ATTRIBUTE_OFFSET)
/* TODO: check that `a_type' is valid. */
	return (EIF_INTEGER)GE_type_infos[a_type].attribute_count;
#else
	return (EIF_INTEGER)0;
#endif
}

/*
 * Physical offset of the `i'-th field for an object of dynamic type `a_type'.
 */
EIF_INTEGER GE_field_offset_of_type_index(EIF_INTEGER i, EIF_TYPE_INDEX a_type)
{
#if defined(GE_USE_ATTRIBUTES) && defined(GE_USE_ATTRIBUTE_OFFSET)
/* TODO: check that `a_type' and `i' are valid. */
	return (EIF_INTEGER)GE_type_infos[a_type].attributes[i - 1]->offset;
#else
	return (EIF_INTEGER)0;
#endif
}

/*
 * Name of the `i'-th field for an object of dynamic type `a_type'.
 */
EIF_POINTER GE_field_name_of_type_index(EIF_INTEGER i, EIF_TYPE_INDEX a_type)
{
#if defined(GE_USE_ATTRIBUTES) && defined(GE_USE_ATTRIBUTE_NAME)
/* TODO: check that `a_type' and `i' are valid. */
	return (EIF_POINTER)GE_type_infos[a_type].attributes[i - 1]->name;
#else
	return (EIF_POINTER)0;
#endif
}

/*
 * Static type of the `i'-th field for an object of dynamic type `a_type'.
 */
EIF_INTEGER GE_field_static_type_of_type_index(EIF_INTEGER i, EIF_TYPE_INDEX a_type)
{
#if defined(GE_USE_ATTRIBUTES) && defined(GE_USE_ATTRIBUTE_TYPE_ID)
/* TODO: check that `a_type' and `i' are valid. */
	return (EIF_INTEGER)GE_type_infos[a_type].attributes[i - 1]->type_id;
#else
	return (EIF_INTEGER)0;
#endif
}

/*
 * Kind of type of the `i'-th field for an object of dynamic type `a_type'.
 */
EIF_INTEGER GE_field_type_kind_of_type_index(EIF_INTEGER i, EIF_TYPE_INDEX a_type)
{
#if defined(GE_USE_ATTRIBUTES) && defined(GE_USE_ATTRIBUTE_TYPE_ID)
	uint32_t l_flags = GE_type_infos[GE_decoded_type(GE_type_infos[a_type].attributes[i - 1]->type_id).id].flags;
	if (l_flags & GE_TYPE_FLAG_BASIC_MASK) {
		switch (l_flags & GE_TYPE_FLAG_BASIC_MASK) {
		case GE_TYPE_FLAG_BOOLEAN:
			return (EIF_INTEGER)GE_TYPE_KIND_BOOLEAN;
		case GE_TYPE_FLAG_CHARACTER_8:
			return (EIF_INTEGER)GE_TYPE_KIND_CHARACTER_8;
		case GE_TYPE_FLAG_CHARACTER_32:
			return (EIF_INTEGER)GE_TYPE_KIND_CHARACTER_32;
		case GE_TYPE_FLAG_INTEGER_8:
			return (EIF_INTEGER)GE_TYPE_KIND_INTEGER_8;
		case GE_TYPE_FLAG_INTEGER_16:
			return (EIF_INTEGER)GE_TYPE_KIND_INTEGER_16;
		case GE_TYPE_FLAG_INTEGER_32:
			return (EIF_INTEGER)GE_TYPE_KIND_INTEGER_32;
		case GE_TYPE_FLAG_INTEGER_64:
			return (EIF_INTEGER)GE_TYPE_KIND_INTEGER_64;
		case GE_TYPE_FLAG_NATURAL_8:
			return (EIF_INTEGER)GE_TYPE_KIND_NATURAL_8;
		case GE_TYPE_FLAG_NATURAL_16:
			return (EIF_INTEGER)GE_TYPE_KIND_NATURAL_16;
		case GE_TYPE_FLAG_NATURAL_32:
			return (EIF_INTEGER)GE_TYPE_KIND_NATURAL_32;
		case GE_TYPE_FLAG_NATURAL_64:
			return (EIF_INTEGER)GE_TYPE_KIND_NATURAL_64;
		case GE_TYPE_FLAG_POINTER:
			return (EIF_INTEGER)GE_TYPE_KIND_POINTER;
		case GE_TYPE_FLAG_REAL_32:
			return (EIF_INTEGER)GE_TYPE_KIND_REAL_32;
		case GE_TYPE_FLAG_REAL_64:
			return (EIF_INTEGER)GE_TYPE_KIND_REAL_64;
		default:
			return (EIF_INTEGER)GE_TYPE_KIND_INVALID;
		}
	} else if (l_flags & GE_TYPE_FLAG_EXPANDED) {
		return (EIF_INTEGER)GE_TYPE_KIND_EXPANDED;
	} else {
		return (EIF_INTEGER)GE_TYPE_KIND_REFERENCE;
	}
#else
	return (EIF_INTEGER)GE_TYPE_KIND_INVALID;
#endif
}

/*
 * Physical size of `a_object'.
 */
EIF_NATURAL_64 GE_object_size(EIF_POINTER a_object)
{
#ifdef GE_USE_TYPE_OBJECT_SIZE
	EIF_TYPE_INDEX l_type_index = ((EIF_REFERENCE)(a_object))->id;
	uint64_t l_size = GE_type_infos[l_type_index].object_size;
#ifdef GE_USE_TYPE_GENERIC_PARAMETERS
	if (GE_is_special_type_index(l_type_index)) {
		EIF_TYPE_INDEX l_generic_parameter = GE_decoded_type(GE_type_infos[l_type_index].generic_parameters[0]).id;
		uint32_t l_flags = GE_type_infos[l_generic_parameter].flags;
		EIF_INTEGER l_capacity = ((EIF_SPECIAL*)a_object)->capacity;
		uint64_t l_item_size;

		if (l_flags & GE_TYPE_FLAG_BASIC_MASK) {
			switch (l_flags & GE_TYPE_FLAG_BASIC_MASK) {
			case GE_TYPE_FLAG_BOOLEAN:
				l_item_size = sizeof(EIF_BOOLEAN);
				break;
			case GE_TYPE_FLAG_CHARACTER_8:
				l_item_size = sizeof(EIF_CHARACTER_8);
				break;
			case GE_TYPE_FLAG_CHARACTER_32:
				l_item_size = sizeof(EIF_CHARACTER_32);
				break;
			case GE_TYPE_FLAG_INTEGER_8:
				l_item_size = sizeof(EIF_INTEGER_8);
				break;
			case GE_TYPE_FLAG_INTEGER_16:
				l_item_size = sizeof(EIF_INTEGER_16);
				break;
			case GE_TYPE_FLAG_INTEGER_32:
				l_item_size = sizeof(EIF_INTEGER_32);
				break;
			case GE_TYPE_FLAG_INTEGER_64:
				l_item_size = sizeof(EIF_INTEGER_64);
				break;
			case GE_TYPE_FLAG_NATURAL_8:
				l_item_size = sizeof(EIF_NATURAL_8);
				break;
			case GE_TYPE_FLAG_NATURAL_16:
				l_item_size = sizeof(EIF_NATURAL_16);
				break;
			case GE_TYPE_FLAG_NATURAL_32:
				l_item_size = sizeof(EIF_NATURAL_32);
				break;
			case GE_TYPE_FLAG_NATURAL_64:
				l_item_size = sizeof(EIF_NATURAL_64);
				break;
			case GE_TYPE_FLAG_POINTER:
				l_item_size = sizeof(EIF_POINTER);
				break;
			case GE_TYPE_FLAG_REAL_32:
				l_item_size = sizeof(EIF_REAL_32);
				break;
			case GE_TYPE_FLAG_REAL_64:
				l_item_size = sizeof(EIF_REAL_64);
				break;
			default:
				l_item_size = 0;
			}
		} else if (l_flags & GE_TYPE_FLAG_EXPANDED) {
			l_item_size = GE_type_infos[l_generic_parameter].object_size;
		} else {
			l_item_size = sizeof(EIF_REFERENCE);
		}
		l_size += l_capacity * l_item_size;
	}
#endif
	return (EIF_NATURAL_64)l_size;
#else
	return (EIF_NATURAL_64)0;
#endif
}

/*
 * Is `i'-th field of objects of type `a_type' a user-defined expanded attribute?
 */
EIF_BOOLEAN GE_is_field_expanded_of_type_index(EIF_INTEGER i, EIF_TYPE_INDEX a_type)
{
#if defined(GE_USE_ATTRIBUTES) && defined(GE_USE_ATTRIBUTE_TYPE_ID)
	uint32_t l_flags = GE_type_infos[GE_decoded_type(GE_type_infos[a_type].attributes[i - 1]->type_id).id].flags;
	return EIF_TEST((l_flags & GE_TYPE_FLAG_EXPANDED) && !(l_flags & GE_TYPE_FLAG_BASIC_MASK));
#else
	return EIF_FALSE;
#endif
}

/*
 * Get a lock on `GE_mark_object' and `GE_unmark_object' routines so that
 * 2 threads cannot `GE_mark_object' and `GE_unmark_object' at the same time.
 */
void GE_lock_marking(void)
{
#ifdef GE_USE_THREADS
/* TODO */
#endif
}

/*
 * Release a lock on `GE_mark_object' and `GE_unmark_object', so that another
 * thread can use `GE_mark_object' and `GE_unmark_object'.
 */
void GE_unlock_marking(void)
{
#ifdef GE_USE_THREADS
/* TODO */
#endif
}

/*
 * Is `obj' marked?
 */
EIF_BOOLEAN GE_is_object_marked(EIF_POINTER obj)
{
	return EIF_TEST(((EIF_REFERENCE)obj)->flags & GE_OBJECT_FLAG_MARKED);
}

/*
 * Mark `obj'.
 */
void GE_mark_object(EIF_POINTER obj)
{
	((EIF_REFERENCE)obj)->flags |= GE_OBJECT_FLAG_MARKED;
}

/*
 * Unmark `obj'.
 */
void GE_unmark_object(EIF_POINTER obj)
{
	((EIF_REFERENCE)obj)->flags &= ~GE_OBJECT_FLAG_MARKED;
}

/*
 * New instance of dynamic `a_type'.
 * Note: returned object is not initialized and may
 * hence violate its invariant.
 * `a_type' cannot represent a SPECIAL type, use
 * `GE_new_special_of_reference_instance_of_type_index' instead.
 */
EIF_REFERENCE GE_new_instance_of_type_index(GE_context* a_context, EIF_TYPE_INDEX a_type)
{
	EIF_REFERENCE (*l_new)(GE_context*, EIF_BOOLEAN);

	l_new = (EIF_REFERENCE (*)(GE_context*, EIF_BOOLEAN))GE_type_infos[a_type].new_instance;
	if (l_new) {
		return l_new(a_context, EIF_TRUE);
	} else {
		return EIF_VOID;
	}
}

/*
 * New instance of dynamic `a_type' that represents
 * a SPECIAL with can contain `a_capacity' elements of reference type.
 * To create a SPECIAL of basic type, use class SPECIAL directly.
 */
EIF_REFERENCE GE_new_special_of_reference_instance_of_type_index(GE_context* a_context, EIF_TYPE_INDEX a_type, EIF_INTEGER a_capacity)
{
	EIF_REFERENCE (*l_new)(GE_context*, EIF_INTEGER, EIF_BOOLEAN);

	l_new = (EIF_REFERENCE (*)(GE_context*, EIF_INTEGER, EIF_BOOLEAN))GE_type_infos[a_type].new_instance;
	if (l_new) {
		return l_new(a_context, a_capacity, EIF_TRUE);
	} else {
		return EIF_VOID;
	}
}

/*
 * New instance of TYPE for object of type `a_type'.
 */
EIF_REFERENCE GE_new_type_instance_of_encoded_type(GE_context* a_context, EIF_ENCODED_TYPE a_type)
{
	EIF_TYPE l_decoded_type;
	EIF_TYPE_INDEX l_type_index;
	EIF_TYPE_INDEX l_annotations;
	EIF_REFERENCE l_result;

	l_decoded_type = GE_decoded_type(a_type);
	l_type_index = l_decoded_type.id;
	l_annotations = l_decoded_type.annotations;
	l_result = (EIF_REFERENCE)&(GE_types[l_type_index][l_annotations]);
	if (l_result->id == 0) {
		l_result = EIF_VOID;
		GE_raise(GE_EX_PROG);
	}
	return l_result;
}

#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"C functions used to access garbage collector facilities"

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2007-2018, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef GE_GC_C
#define GE_GC_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef GE_GC_H
#include "ge_gc.h"
#endif
#ifndef GE_TYPES_H
#include "ge_types.h"
#endif

/*
 * Allocate memory that does not contain pointers to collectable objects.
 * The allocated memory is zeroed.
 * The allocated object is itself not collectable.
 * Do not raise an exception when no-more-memory.
 */
#ifdef GE_USE_BOEHM_GC
void* GE_unprotected_calloc_atomic_uncollectable(size_t nelem, size_t elsize)
{
	void* new_p;

	new_p = GC_malloc_atomic_uncollectable(nelem * elsize);
	if (new_p) {
		memset(new_p, 0, nelem * elsize);
	}
	return new_p;
}
#endif

/*
 * Allocate more memory for the given pointer.
 * The reallocated pointer keeps the same properties (e.g. atomic or not, collectable or not).
 * The extra allocated memory is zeroed.
 * Raise an exception when no-more-memory.
 */
void* GE_recalloc(void* p, size_t old_nelem, size_t new_nelem, size_t elsize)
{
	void* new_p;
#ifdef GE_USE_BOEHM_GC
	new_p = GE_null(GC_REALLOC(p, new_nelem * elsize));
#else /* No GC */
	new_p = GE_null(realloc(p, new_nelem * elsize));
#endif
	memset(((char*)new_p) + (old_nelem * elsize), 0, (new_nelem - old_nelem) * elsize);
	return new_p;
}

/*
 * Allocate more memory for the given pointer.
 * The reallocated pointer keeps the same properties (e.g. atomic or not, collectable or not).
 * The extra allocated memory is zeroed.
 * Do not raise an exception when no-more-memory.
 */
void* GE_unprotected_recalloc(void* p, size_t old_nelem, size_t new_nelem, size_t elsize)
{
	void* new_p;
#ifdef GE_USE_BOEHM_GC
	new_p = GC_REALLOC(p, new_nelem * elsize);
#else /* No GC */
	new_p = realloc(p, new_nelem * elsize);
#endif
	if (new_p) {
		memset(((char*)new_p) + (old_nelem * elsize), 0, (new_nelem - old_nelem) * elsize);
	}
	return new_p;
}

#ifdef GE_USE_BOEHM_GC
/*
 * Call dispose routine on object `C'.
 */
void GE_boehm_dispose(void* C, void* disp) {
	((GE_types[((EIF_REFERENCE)C)->id][0]).dispose)(GE_current_context(), (EIF_REFERENCE) C);
}

/*
 * Call dispose routine `disp' on once-per-object data `data'.
 */
void GE_boehm_dispose_once_per_object_data(void* data, void* disp) {
	((void (*) (void*)) disp)(data);
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"C functions used to implement class THREAD and related threading facilities"

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2016-2024, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef GE_THREAD_C
#define GE_THREAD_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifdef GE_USE_THREADS

#ifndef GE_THREAD_H
#include "ge_thread.h"
#endif
#ifndef GE_GC_H
#include "ge_gc.h"
#endif
#ifndef GE_ONCE_H
#include "ge_once.h"
#endif
#ifndef GE_TIME_H
#include "ge_time.h"
#endif

#ifdef GE_USE_SCOOP
#ifndef GE_SCOOP_H
#include "ge_scoop.h"
#endif
#endif

#ifdef GE_USE_POSIX_THREADS
#include <limits.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Numbers of once-per-thread features.
 */
static unsigned int GE_thread_onces_boolean_count;
static unsigned int GE_thread_onces_character_8_count;
static unsigned int GE_thread_onces_character_32_count;
static unsigned int GE_thread_onces_integer_8_count;
static unsigned int GE_thread_onces_integer_16_count;
static unsigned int GE_thread_onces_integer_32_count;
static unsigned int GE_thread_onces_integer_64_count;
static unsigned int GE_thread_onces_natural_8_count;
static unsigned int GE_thread_onces_natural_16_count;
static unsigned int GE_thread_onces_natural_32_count;
static unsigned int GE_thread_onces_natural_64_count;
static unsigned int GE_thread_onces_pointer_count;
static unsigned int GE_thread_onces_real_32_count;
static unsigned int GE_thread_onces_real_64_count;
static unsigned int GE_thread_onces_reference_count;
static unsigned int GE_thread_onces_procedure_count;

/*
 * Mutexes used to protect the calls to once-per-process features.
 */
GE_once_mutexes* GE_process_once_mutexes;

/*
 * Create a new 'GE_once_mutexes' struct which can deal with the
 * numbers of once features passed as argument.
 */
GE_once_mutexes* GE_new_once_mutexes(
	unsigned int a_boolean_count,
	unsigned int a_character_8_count,
	unsigned int a_character_32_count,
	unsigned int a_integer_8_count,
	unsigned int a_integer_16_count,
	unsigned int a_integer_32_count,
	unsigned int a_integer_64_count,
	unsigned int a_natural_8_count,
	unsigned int a_natural_16_count,
	unsigned int a_natural_32_count,
	unsigned int a_natural_64_count,
	unsigned int a_pointer_count,
	unsigned int a_real_32_count,
	unsigned int a_real_64_count,
	unsigned int a_reference_count,
	unsigned int a_procedure_count)
{
	GE_once_mutexes* l_once_mutexes;
	unsigned int i;

	l_once_mutexes = (GE_once_mutexes*)GE_calloc_atomic_uncollectable(1, sizeof(GE_once_mutexes));
	if (a_boolean_count > 0) {
		l_once_mutexes->boolean_mutex = (EIF_POINTER*)GE_malloc_atomic_uncollectable(a_boolean_count * sizeof(EIF_POINTER));
		for (i = 0; i < a_boolean_count; i++) {
			l_once_mutexes->boolean_mutex[i] = GE_mutex_create();
		}
	}
	if (a_character_8_count > 0) {
		l_once_mutexes->character_8_mutex = (EIF_POINTER*)GE_malloc_atomic_uncollectable(a_character_8_count * sizeof(EIF_POINTER));
		for (i = 0; i < a_character_8_count; i++) {
			l_once_mutexes->character_8_mutex[i] = GE_mutex_create();
		}
	}
	if (a_character_32_count > 0) {
		l_once_mutexes->character_32_mutex = (EIF_POINTER*)GE_malloc_atomic_uncollectable(a_character_32_count * sizeof(EIF_POINTER));
		for (i = 0; i < a_character_32_count; i++) {
			l_once_mutexes->character_32_mutex[i] = GE_mutex_create();
		}
	}
	if (a_integer_8_count > 0) {
		l_once_mutexes->integer_8_mutex = (EIF_POINTER*)GE_malloc_atomic_uncollectable(a_integer_8_count * sizeof(EIF_POINTER));
		for (i = 0; i < a_integer_8_count; i++) {
			l_once_mutexes->integer_8_mutex[i] = GE_mutex_create();
		}
	}
	if (a_integer_16_count > 0) {
		l_once_mutexes->integer_16_mutex = (EIF_POINTER*)GE_malloc_atomic_uncollectable(a_integer_16_count * sizeof(EIF_POINTER));
		for (i = 0; i < a_integer_16_count; i++) {
			l_once_mutexes->integer_16_mutex[i] = GE_mutex_create();
		}
	}
	if (a_integer_32_count > 0) {
		l_once_mutexes->integer_32_mutex = (EIF_POINTER*)GE_malloc_atomic_uncollectable(a_integer_32_count * sizeof(EIF_POINTER));
		for (i = 0; i < a_integer_32_count; i++) {
			l_once_mutexes->integer_32_mutex[i] = GE_mutex_create();
		}
	}
	if (a_integer_64_count > 0) {
		l_once_mutexes->integer_64_mutex = (EIF_POINTER*)GE_malloc_atomic_uncollectable(a_integer_64_count * sizeof(EIF_POINTER));
		for (i = 0; i < a_integer_64_count; i++) {
			l_once_mutexes->integer_64_mutex[i] = GE_mutex_create();
		}
	}
	if (a_natural_8_count > 0) {
		l_once_mutexes->natural_8_mutex = (EIF_POINTER*)GE_malloc_atomic_uncollectable(a_natural_8_count * sizeof(EIF_POINTER));
		for (i = 0; i < a_natural_8_count; i++) {
			l_once_mutexes->natural_8_mutex[i] = GE_mutex_create();
		}
	}
	if (a_natural_16_count > 0) {
		l_once_mutexes->natural_16_mutex = (EIF_POINTER*)GE_malloc_atomic_uncollectable(a_natural_16_count * sizeof(EIF_POINTER));
		for (i = 0; i < a_natural_16_count; i++) {
			l_once_mutexes->natural_16_mutex[i] = GE_mutex_create();
		}
	}
	if (a_natural_32_count > 0) {
		l_once_mutexes->natural_32_mutex = (EIF_POINTER*)GE_malloc_atomic_uncollectable(a_natural_32_count * sizeof(EIF_POINTER));
		for (i = 0; i < a_natural_32_count; i++) {
			l_once_mutexes->natural_32_mutex[i] = GE_mutex_create();
		}
	}
	if (a_natural_64_count > 0) {
		l_once_mutexes->natural_64_mutex = (EIF_POINTER*)GE_malloc_atomic_uncollectable(a_natural_64_count * sizeof(EIF_POINTER));
		for (i = 0; i < a_natural_64_count; i++) {
			l_once_mutexes->natural_64_mutex[i] = GE_mutex_create();
		}
	}
	if (a_pointer_count > 0) {
		l_once_mutexes->pointer_mutex = (EIF_POINTER*)GE_malloc_atomic_uncollectable(a_pointer_count * sizeof(EIF_POINTER));
		for (i = 0; i < a_pointer_count; i++) {
			l_once_mutexes->pointer_mutex[i] = GE_mutex_create();
		}
	}
	if (a_real_32_count > 0) {
		l_once_mutexes->real_32_mutex = (EIF_POINTER*)GE_malloc_atomic_uncollectable(a_real_32_count * sizeof(EIF_POINTER));
		for (i = 0; i < a_real_32_count; i++) {
			l_once_mutexes->real_32_mutex[i] = GE_mutex_create();
		}
	}
	if (a_real_64_count > 0) {
		l_once_mutexes->real_64_mutex = (EIF_POINTER*)GE_malloc_atomic_uncollectable(a_real_64_count * sizeof(EIF_POINTER));
		for (i = 0; i < a_real_64_count; i++) {
			l_once_mutexes->real_64_mutex[i] = GE_mutex_create();
		}
	}
	if (a_reference_count > 0) {
		l_once_mutexes->reference_mutex = (EIF_POINTER*)GE_malloc_atomic_uncollectable(a_reference_count * sizeof(EIF_POINTER));
		for (i = 0; i < a_reference_count; i++) {
			l_once_mutexes->reference_mutex[i] = GE_mutex_create();
		}
	}
	if (a_procedure_count > 0) {
		l_once_mutexes->procedure_mutex = (EIF_POINTER*)GE_malloc_atomic_uncollectable(a_procedure_count * sizeof(EIF_POINTER));
		for (i = 0; i < a_procedure_count; i++) {
			l_once_mutexes->procedure_mutex[i] = GE_mutex_create();
		}
	}
	return l_once_mutexes;
}

/*
 * Keep track of the numbers of once-per-thread features.
 */
void GE_thread_onces_set_counts(
	unsigned int a_boolean_count,
	unsigned int a_character_8_count,
	unsigned int a_character_32_count,
	unsigned int a_integer_8_count,
	unsigned int a_integer_16_count,
	unsigned int a_integer_32_count,
	unsigned int a_integer_64_count,
	unsigned int a_natural_8_count,
	unsigned int a_natural_16_count,
	unsigned int a_natural_32_count,
	unsigned int a_natural_64_count,
	unsigned int a_pointer_count,
	unsigned int a_real_32_count,
	unsigned int a_real_64_count,
	unsigned int a_reference_count,
	unsigned int a_procedure_count)
{
	GE_thread_onces_boolean_count = a_boolean_count;
	GE_thread_onces_character_8_count = a_character_8_count;
	GE_thread_onces_character_32_count = a_character_32_count;
	GE_thread_onces_integer_8_count = a_integer_8_count;
	GE_thread_onces_integer_16_count = a_integer_16_count;
	GE_thread_onces_integer_32_count = a_integer_32_count;
	GE_thread_onces_integer_64_count = a_integer_64_count;
	GE_thread_onces_natural_8_count = a_natural_8_count;
	GE_thread_onces_natural_16_count = a_natural_16_count;
	GE_thread_onces_natural_32_count = a_natural_32_count;
	GE_thread_onces_natural_64_count = a_natural_64_count;
	GE_thread_onces_pointer_count = a_pointer_count;
	GE_thread_onces_real_32_count = a_real_32_count;
	GE_thread_onces_real_64_count = a_real_64_count;
	GE_thread_onces_reference_count = a_reference_count;
	GE_thread_onces_procedure_count = a_procedure_count;
}

/*
 * Initialize `process_onces' and `thread_onces' in `a_context'.
 */
static void GE_thread_init_onces(GE_context* a_context)
{
	a_context->process_onces = GE_new_onces(
		GE_process_onces->boolean_count,
		GE_process_onces->character_8_count,
		GE_process_onces->character_32_count,
		GE_process_onces->integer_8_count,
		GE_process_onces->integer_16_count,
		GE_process_onces->integer_32_count,
		GE_process_onces->integer_64_count,
		GE_process_onces->natural_8_count,
		GE_process_onces->natural_16_count,
		GE_process_onces->natural_32_count,
		GE_process_onces->natural_64_count,
		GE_process_onces->pointer_count,
		GE_process_onces->real_32_count,
		GE_process_onces->real_64_count,
		GE_process_onces->reference_count,
		GE_process_onces->procedure_count);
	a_context->thread_onces = GE_new_onces(
		GE_thread_onces_boolean_count,
		GE_thread_onces_character_8_count,
		GE_thread_onces_character_32_count,
		GE_thread_onces_integer_8_count,
		GE_thread_onces_integer_16_count,
		GE_thread_onces_integer_32_count,
		GE_thread_onces_integer_64_count,
		GE_thread_onces_natural_8_count,
		GE_thread_onces_natural_16_count,
		GE_thread_onces_natural_32_count,
		GE_thread_onces_natural_64_count,
		GE_thread_onces_pointer_count,
		GE_thread_onces_real_32_count,
		GE_thread_onces_real_64_count,
		GE_thread_onces_reference_count,
		GE_thread_onces_procedure_count);
}

/* Global mutex to protect creation of once-per-object data. */
EIF_POINTER GE_once_per_object_data_mutex;

/* Key to access Thread Specific Data. */
static EIF_TSD_TYPE GE_thread_context_key;

#define GE_THREAD_OK 1
#define GE_THREAD_BUSY 2
#define GE_THREAD_TIMEDOUT 3
#define GE_THREAD_ERROR 4

/*
 * Create a new mutex.
 * Do not raise an exception in case of error (just return a null pointer).
 */
static EIF_POINTER GE_unprotected_mutex_create(uintptr_t a_spin_count)
{
#ifdef GE_USE_POSIX_THREADS
	EIF_MUTEX_TYPE* l_mutex;

	l_mutex = (EIF_MUTEX_TYPE*)GE_unprotected_malloc_atomic_uncollectable(sizeof(EIF_MUTEX_TYPE));
	if (l_mutex) {
			/* Make the mutex recursive by default. */
			/* This allows a thread to lock the mutex several times without blocking itself. */
		pthread_mutexattr_t l_attr;
		pthread_mutexattr_init(&l_attr);
		pthread_mutexattr_settype(&l_attr, PTHREAD_MUTEX_RECURSIVE);
		if (pthread_mutex_init(l_mutex, &l_attr)) {
			GE_free(l_mutex);
			l_mutex = NULL;
		}
		pthread_mutexattr_destroy(&l_attr);
	}
	return (EIF_POINTER)l_mutex;
#elif defined EIF_WINDOWS
	EIF_CS_TYPE* l_section;

	l_section = (EIF_CS_TYPE*)GE_unprotected_malloc_atomic_uncollectable(sizeof(EIF_CS_TYPE));
	if (l_section) {
		if (!InitializeCriticalSectionAndSpinCount(l_section, (DWORD)a_spin_count)) {
			GE_free(l_section);
			l_section = NULL;
		}
	}
	return (EIF_POINTER)l_section;
#else
	return (EIF_POINTER)0;
#endif
}

/*
 * Create a new mutex.
 */
EIF_POINTER GE_mutex_create(void)
{
	EIF_POINTER l_mutex;

	l_mutex = GE_unprotected_mutex_create(4000);
	if (!l_mutex) {
		GE_raise_with_message(GE_EX_EXT, "Cannot create mutex");
	}
	return l_mutex;
}

/*
 * Lock mutex.
 * Do not raise an exception in case of error (just return a status).
 */
static int GE_unprotected_mutex_lock(EIF_POINTER a_mutex)
{
#ifdef GE_USE_POSIX_THREADS
	if (pthread_mutex_lock((EIF_MUTEX_TYPE*)a_mutex)) {
		return GE_THREAD_ERROR;
	}
	return GE_THREAD_OK;
#elif defined EIF_WINDOWS
	EnterCriticalSection((EIF_CS_TYPE*)a_mutex);
	return GE_THREAD_OK;
#else
	return GE_THREAD_ERROR;
	GE_raise_with_message(GE_EX_EXT, "Cannot lock mutex");
#endif
}

/*
 * Lock mutex.
 */
void GE_mutex_lock(EIF_POINTER a_mutex)
{
	if (GE_unprotected_mutex_lock(a_mutex) != GE_THREAD_OK) {
		GE_raise_with_message(GE_EX_EXT, "Cannot lock mutex");
	}
}

/*
 * Try to lock mutex.
 * Do not raise an exception in case of error (just return a status).
 */
static int GE_unprotected_mutex_try_lock(EIF_POINTER a_mutex)
{
#ifdef GE_USE_POSIX_THREADS
	switch (pthread_mutex_trylock((EIF_MUTEX_TYPE*)a_mutex)) {
	case EBUSY:
		return GE_THREAD_BUSY;
	case 0:
		return GE_THREAD_OK;
	default:
		return GE_THREAD_ERROR;
	}
#elif defined EIF_WINDOWS
	if (TryEnterCriticalSection((EIF_CS_TYPE*)a_mutex)) {
		return GE_THREAD_OK;
	} else {
		return GE_THREAD_BUSY;
	}
#else
	return GE_THREAD_ERROR;
#endif
}

/*
 * Try to lock mutex. Return True on success.
 */
EIF_BOOLEAN GE_mutex_try_lock(EIF_POINTER a_mutex)
{
	switch (GE_unprotected_mutex_try_lock(a_mutex)) {
	case GE_THREAD_OK:
		return EIF_TRUE;
	case GE_THREAD_BUSY:
		return EIF_FALSE;
	default:
		GE_raise_with_message(GE_EX_EXT, "Cannot lock mutex");
		return EIF_FALSE;
	}
}

/*
 * Unlock mutex.
 * Do not raise an exception in case of error (just return a status).
 */
static int GE_unprotected_mutex_unlock(EIF_POINTER a_mutex)
{
#ifdef GE_USE_POSIX_THREADS
	if (pthread_mutex_unlock((EIF_MUTEX_TYPE*)a_mutex)) {
		return GE_THREAD_ERROR;
	}
	return GE_THREAD_OK;
#elif defined EIF_WINDOWS
	LeaveCriticalSection((EIF_CS_TYPE*)a_mutex);
	return GE_THREAD_OK;
#else
	return GE_THREAD_ERROR;
#endif
}

/*
 * Unlock mutex.
 */
void GE_mutex_unlock(EIF_POINTER a_mutex)
{
	if (GE_unprotected_mutex_unlock(a_mutex) != GE_THREAD_OK) {
		GE_raise_with_message(GE_EX_EXT, "Cannot unlock mutex");
	}
}

/*
 * Destroy and free all resources used by mutex.
 * Do not raise an exception in case of error (just return a status).
 */
static int GE_unprotected_mutex_destroy(EIF_POINTER a_mutex)
{
#ifdef GE_USE_POSIX_THREADS
	int l_result = GE_THREAD_OK;

	if (a_mutex) {
		if (pthread_mutex_destroy((EIF_MUTEX_TYPE*)a_mutex)) {
			l_result = GE_THREAD_ERROR;
		}
		GE_free(a_mutex);
	}
	return l_result;
#elif defined EIF_WINDOWS
	if (a_mutex) {
		DeleteCriticalSection((EIF_CS_TYPE*)a_mutex);
		GE_free(a_mutex);
	}
	return GE_THREAD_OK;
#else
	return GE_THREAD_ERROR;
#endif
}

/*
 * Destroy and free all resources used by mutex.
 */
void GE_mutex_destroy(EIF_POINTER a_mutex)
{
	GE_unprotected_mutex_destroy(a_mutex);
}

/*
 * Create a new semaphore allowing `a_count' threads
 * to go into a critical section.
 * Do not raise an exception in case of error (just return a null pointer).
 */
static EIF_POINTER GE_unprotected_semaphore_create(EIF_INTEGER a_count)
{
	EIF_SEM_TYPE* l_semaphore;

#ifdef GE_USE_POSIX_THREADS
	l_semaphore = (EIF_SEM_TYPE*)GE_unprotected_malloc_atomic_uncollectable(sizeof(EIF_SEM_TYPE));
	if (l_semaphore) {
		if (sem_init(l_semaphore, 0, a_count)) {
			GE_free(l_semaphore);
			l_semaphore = NULL;
		}
	}
	return (EIF_POINTER)l_semaphore;
#elif defined EIF_WINDOWS
	l_semaphore = CreateSemaphore(NULL, (LONG)a_count, (LONG)0x7fffffff, NULL);
	return (EIF_POINTER)l_semaphore;
#else
	return (EIF_POINTER)0;
#endif
}

/*
 * Create a new semaphore allowing `a_count' threads
 * to go into a critical section.
 */
EIF_POINTER GE_semaphore_create(EIF_INTEGER a_count)
{
	EIF_POINTER l_semaphore;

	l_semaphore = GE_unprotected_semaphore_create(a_count);
	if (!l_semaphore) {
		GE_raise_with_message(GE_EX_EXT, "Cannot create semaphore");
	}
	return l_semaphore;
}

/*
 * Decrement semaphore count, waiting if necessary
 * until that becomes possible.
 * Do not raise an exception in case of error (just return a status).
 */
static int GE_unprotected_semaphore_wait(EIF_POINTER a_semaphore)
{
#ifdef GE_USE_POSIX_THREADS
	if (sem_wait((EIF_SEM_TYPE*)a_semaphore)) {
		return GE_THREAD_ERROR;
	}
	return GE_THREAD_OK;
#elif defined EIF_WINDOWS
	switch (WaitForSingleObject((EIF_SEM_TYPE*)a_semaphore, INFINITE)) {
	case WAIT_FAILED:
	case WAIT_ABANDONED:
		return GE_THREAD_ERROR;
	}
	return GE_THREAD_OK;
#else
	return GE_THREAD_ERROR;
#endif
}

/*
 * Decrement semaphore count, waiting if necessary
 * until that becomes possible.
 */
void GE_semaphore_wait(EIF_POINTER a_semaphore)
{
	if (GE_unprotected_semaphore_wait(a_semaphore) != GE_THREAD_OK) {
		GE_raise_with_message(GE_EX_EXT, "Cannot wait on semaphore");
	}
}

/*
 * Has client been successful in decrementing semaphore
 * count without waiting?
 * Do not raise an exception in case of error (just return a status).
 */
static int GE_unprotected_semaphore_try_wait(EIF_POINTER a_semaphore)
{
#ifdef GE_USE_POSIX_THREADS
	if (sem_trywait((EIF_SEM_TYPE*)a_semaphore)) {
		switch (errno) {
		case EBUSY:
		case EAGAIN:
			return GE_THREAD_BUSY;
		case 0:
			return GE_THREAD_OK;
		default:
			return GE_THREAD_ERROR;
		}
	} else {
		return GE_THREAD_OK;
	}
#elif defined EIF_WINDOWS
	switch (WaitForSingleObject((EIF_SEM_TYPE*)a_semaphore, 0)) {
	case WAIT_FAILED:
	case WAIT_ABANDONED:
		return GE_THREAD_ERROR;
	case WAIT_TIMEOUT:
		return GE_THREAD_BUSY;
	default:
		return GE_THREAD_OK;
	}
#else
	return GE_THREAD_ERROR;
#endif
}

/*
 * Has client been successful in decrementing semaphore
 * count without waiting?
 */
EIF_BOOLEAN GE_semaphore_try_wait(EIF_POINTER a_semaphore)
{
	switch (GE_unprotected_semaphore_try_wait(a_semaphore)) {
	case GE_THREAD_OK:
		return EIF_TRUE;
	case GE_THREAD_BUSY:
		return EIF_FALSE;
	default:
		GE_raise_with_message(GE_EX_EXT, "Cannot try_wait on semaphore");
		return EIF_FALSE;
	}
}

/*
 * Increment semaphore count.
 * Do not raise an exception in case of error (just return a status).
 */
static int GE_unprotected_semaphore_post(EIF_POINTER a_semaphore)
{
#ifdef GE_USE_POSIX_THREADS
	if (sem_post((EIF_SEM_TYPE*)a_semaphore)) {
		return GE_THREAD_ERROR;
	}
	return GE_THREAD_OK;
#elif defined EIF_WINDOWS
	if (!ReleaseSemaphore((EIF_SEM_TYPE*)a_semaphore, 1, NULL)) {
		return GE_THREAD_ERROR;
	}
	return GE_THREAD_OK;
#else
	return GE_THREAD_ERROR;
#endif
}

/*
 * Increment semaphore count.
 */
void GE_semaphore_post(EIF_POINTER a_semaphore)
{
	if (GE_unprotected_semaphore_post(a_semaphore) != GE_THREAD_OK) {
		GE_raise_with_message(GE_EX_EXT, "Cannot post on semaphore");
	}
}

/*
 * Destroy and free all resources used by semaphore.
 * Do not raise an exception in case of error (just return a status).
 */
static int GE_unprotected_semaphore_destroy(EIF_POINTER a_semaphore)
{
#ifdef GE_USE_POSIX_THREADS
	int l_result = GE_THREAD_OK;

	if (a_semaphore) {
		if (sem_destroy((EIF_SEM_TYPE*)a_semaphore)) {
			l_result = GE_THREAD_ERROR;
		}
		GE_free(a_semaphore);
	}
	return l_result;
#elif defined EIF_WINDOWS
	int l_result = GE_THREAD_OK;

	if (a_semaphore) {
		if (!CloseHandle((EIF_SEM_TYPE*)a_semaphore)) {
			l_result = GE_THREAD_ERROR;
		}
	}
	return l_result;
#else
	return GE_THREAD_ERROR;
#endif
}

/*
 * Destroy and free all resources used by semaphore.
 */
void GE_semaphore_destroy(EIF_POINTER a_semaphore)
{
	GE_unprotected_semaphore_destroy(a_semaphore);
}

/*
 * Create a new condition variable.
 * Do not raise an exception in case of error (just return a null pointer).
 */
static EIF_POINTER GE_unprotected_condition_variable_create(void)
{
	EIF_COND_TYPE* l_condition_variable;

#ifdef GE_USE_POSIX_THREADS
	l_condition_variable = (EIF_COND_TYPE*)GE_unprotected_malloc_atomic_uncollectable(sizeof(EIF_COND_TYPE));
	if (l_condition_variable) {
		if (pthread_cond_init(l_condition_variable, NULL)) {
			GE_free(l_condition_variable);
			l_condition_variable = NULL;
		}
	}
	return (EIF_POINTER)l_condition_variable;
#elif defined EIF_WINDOWS
	EIF_SEM_TYPE* l_semaphore;
	EIF_CS_TYPE* l_section;

	l_condition_variable = (EIF_COND_TYPE*)GE_unprotected_malloc_atomic_uncollectable(sizeof(EIF_COND_TYPE));
	if (l_condition_variable) {
		memset(l_condition_variable, 0, sizeof(EIF_COND_TYPE));
		l_semaphore = (EIF_SEM_TYPE*)GE_unprotected_semaphore_create(0);
		if (l_semaphore) {
			l_condition_variable->semaphore = l_semaphore;
			l_section = (EIF_CS_TYPE*)GE_unprotected_mutex_create(100);
			if (l_section) {
				l_condition_variable->csection = l_section;
			} else {
				GE_unprotected_semaphore_destroy((EIF_POINTER)l_semaphore);
				GE_free(l_condition_variable);
				l_condition_variable = NULL;
			}
		} else {
			GE_free(l_condition_variable);
			l_condition_variable = NULL;
		}
	}
	return (EIF_POINTER)l_condition_variable;
#else
	return (EIF_POINTER)0;
#endif
}

/*
 * Create a new condition variable.
 */
EIF_POINTER GE_condition_variable_create(void)
{
	EIF_POINTER l_condition_variable;

	l_condition_variable = GE_unprotected_condition_variable_create();
	if (!l_condition_variable) {
		GE_raise_with_message(GE_EX_EXT, "Cannot create condition variable");
	}
	return l_condition_variable;
}

/*
 * Unblock all threads blocked on condition variable.
 * Do not raise an exception in case of error (just return a status).
 */
static int GE_unprotected_condition_variable_broadcast(EIF_POINTER a_condition_variable)
{
#ifdef GE_USE_POSIX_THREADS
	if (pthread_cond_broadcast((EIF_COND_TYPE*)a_condition_variable)) {
		return GE_THREAD_ERROR;
	}
	return GE_THREAD_OK;
#elif defined EIF_WINDOWS
	EIF_COND_TYPE* l_condition_variable;
	unsigned long l_num_wake = 0;
	int l_result = GE_THREAD_OK;

	l_condition_variable = (EIF_COND_TYPE*)a_condition_variable;
	l_result = GE_unprotected_mutex_lock((EIF_POINTER)l_condition_variable->csection);
	if (l_result == GE_THREAD_OK) {
		if (l_condition_variable->num_waiting > l_condition_variable->num_wake) {
			l_num_wake = l_condition_variable->num_waiting - l_condition_variable->num_wake;
			l_condition_variable->num_wake = l_condition_variable->num_waiting;
			l_condition_variable->generation++;
		}
		l_result = GE_unprotected_mutex_unlock((EIF_POINTER)l_condition_variable->csection);
		while ((l_result == GE_THREAD_OK) && l_num_wake) {
			l_result = GE_unprotected_semaphore_post((EIF_POINTER)l_condition_variable->semaphore);
			l_num_wake--;
		}
	}
	return l_result;
#else
	return GE_THREAD_ERROR;
#endif
}

/*
 * Unblock all threads blocked on condition variable.
 */
void GE_condition_variable_broadcast(EIF_POINTER a_condition_variable)
{
	if (GE_unprotected_condition_variable_broadcast(a_condition_variable) != GE_THREAD_OK) {
		GE_raise_with_message(GE_EX_EXT, "Cannot broadcast on condition variable");
	}
}

/*
 * Unblock one thread blocked on condition variable.
 * Do not raise an exception in case of error (just return a status).
 */
static int GE_unprotected_condition_variable_signal(EIF_POINTER a_condition_variable)
{
#ifdef GE_USE_POSIX_THREADS
	if (pthread_cond_signal((EIF_COND_TYPE*)a_condition_variable)) {
		return GE_THREAD_ERROR;
	}
	return GE_THREAD_OK;
#elif defined EIF_WINDOWS
	EIF_COND_TYPE* l_condition_variable;
	unsigned int l_wake = 0;
	int l_result = GE_THREAD_OK;

	l_condition_variable = (EIF_COND_TYPE*)a_condition_variable;
	l_result = GE_unprotected_mutex_lock((EIF_POINTER)l_condition_variable->csection);
	if (l_result == GE_THREAD_OK) {
			/* Do nothing if they are more signaled ones than awaiting threads. */
		if (l_condition_variable->num_waiting > l_condition_variable->num_wake) {
			l_wake = 1;
			l_condition_variable->num_wake++;
			l_condition_variable->generation++;
		}
		l_result = GE_unprotected_mutex_unlock((EIF_POINTER)l_condition_variable->csection);
		if ((l_result == GE_THREAD_OK) && l_wake) {
			l_result = GE_unprotected_semaphore_post((EIF_POINTER)l_condition_variable->semaphore);
		}
	}
	return l_result;
#else
	return GE_THREAD_ERROR;
#endif
}

/*
 * Unblock one thread blocked on condition variable.
 */
void GE_condition_variable_signal(EIF_POINTER a_condition_variable)
{
	if (GE_unprotected_condition_variable_signal(a_condition_variable) != GE_THREAD_OK) {
		GE_raise_with_message(GE_EX_EXT, "Cannot signal on condition variable");
	}
}

#ifdef GE_USE_POSIX_THREADS
/*
 * Given a timeout in milliseconds, computes a timespec structure equivalent.
 * `a_timeout': Timeout to convert in milliseconds.
 *
 */
static struct timespec GE_timeout_to_timespec(uintptr_t a_timeout)
{
	time_t l_seconds = a_timeout / 1000;	/* `a_timeout' is in millisecond */
	long l_nano_seconds = (a_timeout % 1000) * 1000000;	/* Reminder in nanoseconds */
	struct timespec tspec;
	struct timeval now;
	GE_ftime(&now);
	tspec.tv_sec = now.tv_sec + l_seconds;
	l_nano_seconds += (now.tv_usec * 1000);
	tspec.tv_nsec = l_nano_seconds;
		/* If `l_nano_seconds' is greater than 1 second, we need to update `tspec'
		 * accordingly otherwise we may get EINVAL on some platforms. */
	if (l_nano_seconds > 1000000000) {
		tspec.tv_sec++;
		tspec.tv_nsec -= 1000000000;
	}
	return tspec;
}
#endif

/*
 * Block calling thread on condition variable for at most `a_timeout' milliseconds.
 * Do not raise an exception in case of error (just return a status).
 */
static int GE_unprotected_condition_variable_wait_with_timeout(EIF_POINTER a_condition_variable, EIF_POINTER a_mutex, uintptr_t a_timeout)
{
#ifdef GE_USE_POSIX_THREADS
	struct timespec l_tspec = GE_timeout_to_timespec(a_timeout);
	switch (pthread_cond_timedwait((EIF_COND_TYPE*)a_condition_variable, (EIF_MUTEX_TYPE*)a_mutex, &l_tspec)) {
	case 0:
		return GE_THREAD_OK;
	case ETIMEDOUT:
		return GE_THREAD_TIMEDOUT;
	default:
		return GE_THREAD_ERROR;
	}
#elif defined EIF_WINDOWS
	EIF_COND_TYPE* l_condition_variable;
	unsigned int l_wake = 0;
	unsigned long l_generation;

	l_condition_variable = (EIF_COND_TYPE*)a_condition_variable;
	GE_unprotected_mutex_lock((EIF_POINTER)l_condition_variable->csection);
	l_condition_variable->num_waiting++;
	l_generation = l_condition_variable->generation;
	GE_unprotected_mutex_unlock((EIF_POINTER)l_condition_variable->csection);
	GE_unprotected_mutex_unlock(a_mutex);
	for (;;) {
		switch (WaitForSingleObject(l_condition_variable->semaphore, (DWORD)a_timeout)) {
		case WAIT_FAILED:
		case WAIT_ABANDONED:
			GE_unprotected_mutex_lock(a_mutex);
			return GE_THREAD_ERROR;
		case WAIT_TIMEOUT:
			GE_unprotected_mutex_lock((EIF_POINTER)l_condition_variable->csection);
			l_condition_variable->num_waiting--;
			GE_unprotected_mutex_unlock((EIF_POINTER)l_condition_variable->csection);
			GE_unprotected_mutex_lock(a_mutex);
			return GE_THREAD_TIMEDOUT;
		default:
			GE_unprotected_mutex_lock((EIF_POINTER)l_condition_variable->csection);
			if (l_condition_variable->num_wake) {
				if (l_condition_variable->generation != l_generation) {
					l_condition_variable->num_wake--;
					l_condition_variable->num_waiting--;
					GE_unprotected_mutex_unlock((EIF_POINTER)l_condition_variable->csection);
					GE_unprotected_mutex_lock(a_mutex);
					return GE_THREAD_OK;
				} else {
					l_wake = 1;
				}
			}
			GE_unprotected_mutex_unlock((EIF_POINTER)l_condition_variable->csection);
			if (l_wake) {
				l_wake = 0;
				GE_unprotected_semaphore_post((EIF_POINTER)l_condition_variable->semaphore);
			}
		}
	}
#else
	return GE_THREAD_ERROR;
#endif
}

/*
 * Block calling thread on condition variable for at most `a_timeout' milliseconds.
 * Return 1 is we got the condition variable on time, otherwise return 0.
 */
EIF_INTEGER GE_condition_variable_wait_with_timeout(EIF_POINTER a_condition_variable, EIF_POINTER a_mutex, EIF_INTEGER a_timeout)
{
	switch (GE_unprotected_condition_variable_wait_with_timeout(a_condition_variable, a_mutex, (uintptr_t)a_timeout)) {
	case GE_THREAD_OK:
		return 1;
	case GE_THREAD_TIMEDOUT:
		return 0;
	default:
		GE_raise_with_message(GE_EX_EXT, "Cannot wait with timeout on condition variable");
		return -1;
	}
}

/*
 * Block calling thread on condition variable.
 * Do not raise an exception in case of error (just return a status).
 */
static int GE_unprotected_condition_variable_wait(EIF_POINTER a_condition_variable, EIF_POINTER a_mutex)
{
#ifdef GE_USE_POSIX_THREADS
	if (pthread_cond_wait((EIF_COND_TYPE*)a_condition_variable, (EIF_MUTEX_TYPE*)a_mutex)) {
		return GE_THREAD_ERROR;
	}
	return GE_THREAD_OK;
#elif defined EIF_WINDOWS
	return GE_unprotected_condition_variable_wait_with_timeout(a_condition_variable, a_mutex, (uintptr_t)INFINITE);
#else
	return GE_THREAD_ERROR;
#endif
}

/*
 * Block calling thread on condition variable.
 */
void GE_condition_variable_wait(EIF_POINTER a_condition_variable, EIF_POINTER a_mutex)
{
	if (GE_unprotected_condition_variable_wait(a_condition_variable, a_mutex) != GE_THREAD_OK) {
		GE_raise_with_message(GE_EX_EXT, "Cannot wait on condition variable");
	}
}

/*
 * Destroy and free all resources used by condition variable.
 * Do not raise an exception in case of error (just return a status).
 */
static int GE_unprotected_condition_variable_destroy(EIF_POINTER a_condition_variable)
{
#ifdef GE_USE_POSIX_THREADS
	int l_result = GE_THREAD_OK;

	if (a_condition_variable) {
		if (pthread_cond_destroy((EIF_COND_TYPE*)a_condition_variable)) {
			l_result = GE_THREAD_ERROR;
		}
		GE_free(a_condition_variable);
	}
	return l_result;
#elif defined EIF_WINDOWS
	EIF_COND_TYPE* l_condition_variable;
	EIF_CS_TYPE* l_section;
	int l_result = GE_THREAD_OK;
	int l_other_result;

	if (a_condition_variable) {
		l_condition_variable = (EIF_COND_TYPE*)a_condition_variable;
		l_other_result = GE_unprotected_semaphore_destroy((EIF_POINTER)l_condition_variable->semaphore);
		if (l_other_result != GE_THREAD_OK) {
			l_result = l_other_result;
		}
		l_other_result = GE_unprotected_mutex_destroy((EIF_POINTER)l_condition_variable->csection);
		if (l_other_result != GE_THREAD_OK) {
			l_result = l_other_result;
		}
		GE_free(a_condition_variable);
	}
	return l_result;
#else
	return GE_THREAD_ERROR;
#endif
}

/*
 * Destroy and free all resources used by condition variable.
 */
void GE_condition_variable_destroy(EIF_POINTER a_condition_variable)
{
	GE_unprotected_condition_variable_destroy(a_condition_variable);
}

/*
 * Create a new read-write lock.
 * Do not raise an exception in case of error (just return a null pointer).
 */
static EIF_POINTER GE_unprotected_read_write_lock_create(void)
{
	EIF_RWL_TYPE* l_read_write_lock;

#ifdef GE_USE_POSIX_THREADS
	l_read_write_lock = (EIF_RWL_TYPE*)GE_unprotected_malloc_atomic_uncollectable(sizeof(EIF_RWL_TYPE));
	if (l_read_write_lock) {
		if (pthread_rwlock_init(l_read_write_lock, NULL)) {
			GE_free(l_read_write_lock);
			l_read_write_lock = NULL;
		}
	}
	return (EIF_POINTER)l_read_write_lock;
#elif defined EIF_WINDOWS
	EIF_MUTEX_TYPE* l_mutex;
	EIF_COND_TYPE* l_readers_ok;
	EIF_COND_TYPE* l_writers_ok;

	l_read_write_lock = (EIF_RWL_TYPE*)GE_unprotected_malloc_atomic_uncollectable(sizeof(EIF_RWL_TYPE));
	if (l_read_write_lock) {
		l_mutex = (EIF_MUTEX_TYPE*)GE_unprotected_mutex_create(4000);
		if (l_mutex) {
			l_readers_ok = (EIF_COND_TYPE*)GE_unprotected_condition_variable_create();
			if (l_readers_ok) {
				l_writers_ok = (EIF_COND_TYPE*)GE_unprotected_condition_variable_create();
				if (l_writers_ok) {
					l_read_write_lock->m = l_mutex;
					l_read_write_lock->readers_ok = l_readers_ok;
					l_read_write_lock->writers_ok = l_writers_ok;
					l_read_write_lock->rwlock = 0;
					l_read_write_lock->waiting_writers = 0;
				} else {
					GE_unprotected_condition_variable_destroy((EIF_POINTER)l_readers_ok);
					GE_unprotected_mutex_destroy((EIF_POINTER)l_mutex);
					GE_free(l_read_write_lock);
					l_read_write_lock = NULL;
				}
			} else {
				GE_unprotected_mutex_destroy((EIF_POINTER)l_mutex);
				GE_free(l_read_write_lock);
				l_read_write_lock = NULL;
			}
		} else {
			GE_free(l_read_write_lock);
			l_read_write_lock = NULL;
		}
	}
	return (EIF_POINTER)l_read_write_lock;
#else
	return (EIF_POINTER)0;
#endif
}

/*
 * Create a new read-write lock.
 */
EIF_POINTER GE_read_write_lock_create(void)
{
	EIF_POINTER l_read_write_lock;

	l_read_write_lock = GE_unprotected_read_write_lock_create();
	if (!l_read_write_lock) {
		GE_raise_with_message(GE_EX_EXT, "Cannot create read-write lock variable");
	}
	return l_read_write_lock;
}

/*
 * Acquire a read lock. Multiple readers can go if there are no writer.
 * Do not raise an exception in case of error (just return a status).
 */
static int GE_unprotected_read_write_lock_read_lock(EIF_POINTER a_read_write_lock)
{
#ifdef GE_USE_POSIX_THREADS
	if (pthread_rwlock_rdlock((EIF_RWL_TYPE*)a_read_write_lock)) {
		return GE_THREAD_ERROR;
	}
	return GE_THREAD_OK;
#elif defined EIF_WINDOWS
	EIF_RWL_TYPE* l_read_write_lock;
	int l_result;

	l_read_write_lock = (EIF_RWL_TYPE*)a_read_write_lock;
	l_result = GE_unprotected_mutex_lock((EIF_POINTER)l_read_write_lock->m);
	if (l_result == GE_THREAD_OK) {
		while ((l_read_write_lock->rwlock < 0 || l_read_write_lock->waiting_writers) && (l_result == GE_THREAD_OK)) {
			l_result = GE_unprotected_condition_variable_wait((EIF_POINTER)l_read_write_lock->readers_ok, (EIF_POINTER)l_read_write_lock->m);
		}
		if (l_result == GE_THREAD_OK) {
			l_read_write_lock->rwlock++;
		}
		l_result = GE_unprotected_mutex_unlock((EIF_POINTER)l_read_write_lock->m);
	}
	return l_result;
#else
	return GE_THREAD_ERROR;
#endif
}

/*
 * Acquire a read lock. Multiple readers can go if there are no writer.
 */
void GE_read_write_lock_read_lock(EIF_POINTER a_read_write_lock)
{
	if (GE_unprotected_read_write_lock_read_lock(a_read_write_lock) != GE_THREAD_OK) {
		GE_raise_with_message(GE_EX_EXT, "Cannot read lock");
	}
}

/*
 * Acquire a write lock. Only a single write can proceed.
 * Do not raise an exception in case of error (just return a status).
 */
static int GE_unprotected_read_write_lock_write_lock(EIF_POINTER a_read_write_lock)
{
#ifdef GE_USE_POSIX_THREADS
	if (pthread_rwlock_wrlock((EIF_RWL_TYPE*)a_read_write_lock)) {
		return GE_THREAD_ERROR;
	}
	return GE_THREAD_OK;
#elif defined EIF_WINDOWS
	EIF_RWL_TYPE* l_read_write_lock;
	int l_result;

	l_read_write_lock = (EIF_RWL_TYPE*)a_read_write_lock;
	l_result = GE_unprotected_mutex_lock((EIF_POINTER)l_read_write_lock->m);
	if (l_result == GE_THREAD_OK) {
		while ((l_read_write_lock->rwlock != 0) && (l_result == GE_THREAD_OK)) {
			l_read_write_lock->waiting_writers++;
			l_result = GE_unprotected_condition_variable_wait((EIF_POINTER)l_read_write_lock->writers_ok, (EIF_POINTER)l_read_write_lock->m);
			l_read_write_lock->waiting_writers--;
		}
		if (l_result == GE_THREAD_OK) {
			l_read_write_lock->rwlock = -1;
		}
		l_result = GE_unprotected_mutex_unlock((EIF_POINTER)l_read_write_lock->m);
	}
	return l_result;
#else
	return GE_THREAD_ERROR;
#endif
}

/*
 * Acquire a write lock. Only a single write can proceed.
 */
void GE_read_write_lock_write_lock(EIF_POINTER a_read_write_lock)
{
	if (GE_unprotected_read_write_lock_write_lock(a_read_write_lock) != GE_THREAD_OK) {
		GE_raise_with_message(GE_EX_EXT, "Cannot write lock");
	}
}

/*
 * Unlock a read or write lock.
 * Do not raise an exception in case of error (just return a status).
 */
static int GE_unprotected_read_write_lock_unlock(EIF_POINTER a_read_write_lock)
{
#ifdef GE_USE_POSIX_THREADS
	if (pthread_rwlock_unlock((EIF_RWL_TYPE*)a_read_write_lock)) {
		return GE_THREAD_ERROR;
	}
	return GE_THREAD_OK;
#elif defined EIF_WINDOWS
	EIF_RWL_TYPE* l_read_write_lock;
	int l_result;
	int ww, wr;

	l_read_write_lock = (EIF_RWL_TYPE*)a_read_write_lock;
	l_result = GE_unprotected_mutex_lock((EIF_POINTER)l_read_write_lock->m);
	if (l_result == GE_THREAD_OK) {
			/* rwlock < 0 iflocked for writing */
		if (l_read_write_lock->rwlock < 0) {
			l_read_write_lock->rwlock = 0;
		} else {
			l_read_write_lock->rwlock--;
		}

			/* Keep flags that show if there are waiting readers or writers
			 * so that we can wake them up outside the mocitor lock. */
		ww = (l_read_write_lock->waiting_writers && l_read_write_lock->rwlock == 0);
		wr = (l_read_write_lock->waiting_writers == 0);

		l_result = GE_unprotected_mutex_unlock((EIF_POINTER)l_read_write_lock->m);
		if (l_result == GE_THREAD_OK) {
				/* Wake up a waiting writer first. Otherwise wake up all readers. */
			if (ww) {
				l_result = GE_unprotected_condition_variable_signal((EIF_POINTER)l_read_write_lock->writers_ok);
			} else if (wr) {
				l_result = GE_unprotected_condition_variable_broadcast((EIF_POINTER)l_read_write_lock->readers_ok);
			}
		}
	}
	return l_result;
#else
	return GE_THREAD_ERROR;
#endif
}

/*
 * Unlock a read or write lock.
 */
void GE_read_write_lock_unlock(EIF_POINTER a_read_write_lock)
{
	if (GE_unprotected_read_write_lock_unlock(a_read_write_lock) != GE_THREAD_OK) {
		GE_raise_with_message(GE_EX_EXT, "Cannot unlock read-write lock");
	}
}

/*
 * Destroy and free all resources used by read-write lock.
 * Do not raise an exception in case of error (just return a status).
 */
static int GE_unprotected_read_write_lock_destroy(EIF_POINTER a_read_write_lock)
{
#ifdef GE_USE_POSIX_THREADS
	int l_result = GE_THREAD_OK;

	if (a_read_write_lock) {
		if (pthread_rwlock_destroy((EIF_RWL_TYPE*)a_read_write_lock)) {
			l_result = GE_THREAD_ERROR;
		}
		GE_free(a_read_write_lock);
	}
	return l_result;
#elif defined EIF_WINDOWS
	EIF_RWL_TYPE* l_read_write_lock;
	int l_result = GE_THREAD_OK;
	int l_other_result;

	if (a_read_write_lock) {
		l_read_write_lock = (EIF_RWL_TYPE*)a_read_write_lock;
		l_other_result = GE_unprotected_mutex_destroy((EIF_POINTER)l_read_write_lock->m);
		if (l_other_result != GE_THREAD_OK) {
			l_result = l_other_result;
		}
		l_other_result = GE_unprotected_condition_variable_destroy((EIF_POINTER)l_read_write_lock->readers_ok);
		if (l_other_result != GE_THREAD_OK) {
			l_result = l_other_result;
		}
		l_other_result = GE_unprotected_condition_variable_destroy((EIF_POINTER)l_read_write_lock->writers_ok);
		if (l_other_result != GE_THREAD_OK) {
			l_result = l_other_result;
		}
		GE_free(l_read_write_lock);
	}
	return l_result;
#else
	return GE_THREAD_ERROR;
#endif
}

/*
 * Destroy and free all resources used by read-write lock.
 */
void GE_read_write_lock_destroy(EIF_POINTER a_read_write_lock)
{
	GE_unprotected_read_write_lock_destroy(a_read_write_lock);
}

/*
 * Pointer to current thread.
 */
static EIF_THR_TYPE GE_current_thread(void)
{
#ifdef GE_USE_POSIX_THREADS
	return pthread_self();
#elif defined EIF_WINDOWS
		/* On Windows, `GetCurrentThread' returns a pseudo handle to the Current thread, */
		/* so we have to do something special on Windows. */
	return OpenThread(THREAD_ALL_ACCESS, FALSE, GetCurrentThreadId());
#endif
}

/*
 * Register context of current thread.
 */
static void GE_register_thread_context(GE_context* a_context)
{
	EIF_TSD_SET(GE_thread_context_key, a_context, "Cannot bind thread context to TSD.");
}

/*
 * Set priority level to thread.
 */
static void GE_thread_set_priority(EIF_THR_TYPE a_thread_id, unsigned int a_priority)
{
#ifdef GE_USE_POSIX_THREADS
	struct sched_param l_param;
	memset(&l_param, 0, sizeof(struct sched_param));
	l_param.sched_priority = a_priority;
	pthread_setschedparam(a_thread_id, SCHED_OTHER, &l_param);
#elif defined(EIF_WINDOWS)
	int l_win_priority;
	switch (a_priority) {
		case EIF_MIN_THR_PRIORITY:
			l_win_priority = THREAD_PRIORITY_LOWEST;
			break;
		case EIF_MAX_THR_PRIORITY:
			l_win_priority = THREAD_PRIORITY_HIGHEST;
			break;
		default:
			if (a_priority <= EIF_BELOW_NORMAL_THR_PRIORITY) {
				l_win_priority = THREAD_PRIORITY_BELOW_NORMAL;
			} else if (a_priority < EIF_ABOVE_NORMAL_THR_PRIORITY) {
				l_win_priority = THREAD_PRIORITY_NORMAL;
			} else {
				l_win_priority = THREAD_PRIORITY_ABOVE_NORMAL;
			}
	}
	SetThreadPriority(a_thread_id, l_win_priority);
#endif
}

/*
 * Routine to be called from the new thread when created.
 */
#ifdef EIF_WINDOWS
static unsigned __stdcall GE_thread_routine(void* arg)
#else
static void* GE_thread_routine(void* arg)
#endif
{
	GE_thread_context* l_thread_context = (GE_thread_context*)arg;
	GE_context* l_context;
	GE_thread_context* l_parent_thread_context = l_thread_context->parent_context;

	SIGBLOCK;
	GE_unprotected_mutex_lock((EIF_POINTER)l_parent_thread_context->children_mutex);
	/* Wait for the parent thread to set the thread id. */
#ifdef GE_USE_SCOOP
	if (l_thread_context->is_scoop_processor) {
		l_thread_context->parent_context = 0;
	}
#endif
	GE_unprotected_mutex_unlock((EIF_POINTER)l_parent_thread_context->children_mutex);
	l_context = (GE_context*)GE_malloc_atomic_uncollectable(sizeof(GE_context));
	*l_context = GE_default_context;
	l_context->thread = l_thread_context;
	GE_thread_init_onces(l_context);
	GE_init_exception(l_context);
	GE_register_thread_context(l_context);
	GE_thread_set_priority(l_thread_context->thread_id, l_thread_context->initial_priority);
	SIGRESUME;
	if (l_thread_context->current) {
#ifdef GE_USE_SCOOP
		l_context->scoop_processor = l_thread_context->current->scoop_processor;
		if (l_thread_context->is_scoop_processor) {
				/* Do not keep track of the current object so that it can be
				reclaimed by the GC if it is not referenced anywhere else. */
			l_context->scoop_processor->context = l_context;
			l_thread_context->current = EIF_VOID;
			GE_scoop_processor_run(l_context);
		} else
#endif
		if (l_thread_context->routine) {
			l_thread_context->routine(l_thread_context->current, 0);
		}
	}
	GE_thread_exit();
#ifdef EIF_WINDOWS
	return 0;
#else
	return NULL;
#endif
}

/*
 * Initialize data to handle threads.
 * To be called at the beginning of the main function
 * on the main thread.
 */
void GE_init_thread(GE_context* a_context)
{
	GE_thread_context* l_thread_context;

	EIF_TSD_CREATE(GE_thread_context_key, "Cannot create GE_thread_context_key");
	l_thread_context = (GE_thread_context*)GE_unprotected_calloc_uncollectable(1, sizeof(GE_thread_context));
	if (l_thread_context) {
		a_context->thread = l_thread_context;
		l_thread_context->thread_id = GE_current_thread();
		l_thread_context->is_alive = 1;
#ifdef GE_USE_SCOOP
		l_thread_context->is_scoop_processor = 1;
#endif
		GE_thread_init_onces(a_context);
		GE_register_thread_context(a_context);
	} else {
		GE_raise_with_message(GE_EX_EXT, "Cannot create thread context");
	}
}

/*
 * Create a new thread with attributes `attr' and execute
 * Eiffel routine `routine' on object `current'.
 */
void GE_thread_create_with_attr(EIF_REFERENCE current, void (*routine)(EIF_REFERENCE, EIF_INTEGER), void (*set_terminated)(EIF_REFERENCE,EIF_BOOLEAN), EIF_THR_ATTR_TYPE* attr, int is_scoop_processor)
{
	EIF_THR_TYPE l_thread_id;
	GE_thread_context* l_thread_context;
	GE_thread_context* l_current_thread_context;
	EIF_MUTEX_TYPE* l_mutex;
	EIF_COND_TYPE* l_condition_variable;
	unsigned int l_attr_stack_size = 0;
	unsigned int l_attr_priority = GE_thread_default_priority();
	int l_raise_error = 0;
	l_thread_context = (GE_thread_context*)GE_unprotected_calloc_uncollectable(1, sizeof(GE_thread_context));
	if (!l_thread_context) {
		GE_raise_with_message(GE_EX_EXT, "Cannot create thread");
	} else {
		if (attr) {
			l_attr_stack_size = attr->stack_size;
			l_attr_priority = attr->priority;
		}
		l_thread_context->current = current;
		l_thread_context->routine = routine;
		l_thread_context->set_terminated = set_terminated;
		l_thread_context->initial_priority = l_attr_priority;
		l_thread_context->is_alive = 1;
#ifdef GE_USE_SCOOP
		l_thread_context->is_scoop_processor = is_scoop_processor;
#endif
		l_current_thread_context = GE_thread_current_context()->thread;
		l_thread_context->parent_context = l_current_thread_context;
		if (!l_current_thread_context->children_mutex) {
				/* This is the first thread that we create from this thread. */
			l_mutex = (EIF_MUTEX_TYPE*)GE_unprotected_mutex_create(4000);
			if (l_mutex) {
				l_condition_variable = (EIF_COND_TYPE*)GE_unprotected_condition_variable_create();
				if (l_condition_variable) {
					l_current_thread_context->children_mutex = l_mutex;
					l_current_thread_context->children_cond = l_condition_variable;
				} else {
					GE_unprotected_mutex_destroy((EIF_POINTER)l_mutex);
					GE_free(l_thread_context);
					GE_raise_with_message(GE_EX_EXT, "Cannot create thread children condition variable");
				}
			} else {
				GE_free(l_thread_context);
				GE_raise_with_message(GE_EX_EXT, "Cannot create thread children mutex");
			}
		}
#ifdef GE_USE_POSIX_THREADS
		{
			pthread_attr_t l_attr;
			int res;

			if (pthread_attr_init(&l_attr) == 0) {
					/* Initialize the stack size if more than the minimum. */
				if (l_attr_stack_size >= PTHREAD_STACK_MIN) {
					pthread_attr_setstacksize(&l_attr, l_attr_stack_size);
				}
				if (l_attr_priority != EIF_DEFAULT_THR_PRIORITY) {
					struct sched_param l_param;
					memset(&l_param, 0, sizeof(struct sched_param));
					l_param.sched_priority = l_attr_priority;
					pthread_attr_setschedpolicy(&l_attr, SCHED_OTHER);
					pthread_attr_setschedparam(&l_attr, &l_param);
				}
					/* We always create threads detached. */
				pthread_attr_setdetachstate(&l_attr, PTHREAD_CREATE_DETACHED);
				SIGBLOCK;
				GE_unprotected_mutex_lock((EIF_POINTER)l_current_thread_context->children_mutex);
					/* Use the mutex even it case of success to force the thread being created to wait for its thread id to be set. */
				if (pthread_create(&l_thread_id, &l_attr, GE_thread_routine, l_thread_context) == 0) {
					l_thread_context->thread_id = l_thread_id;
#ifdef GE_USE_SCOOP
					if (!l_thread_context->is_scoop_processor) {
#endif
						l_current_thread_context->last_thread_id = l_thread_id;
						l_current_thread_context->n_children++;
#ifdef GE_USE_SCOOP
					}
#endif
				} else {
					l_raise_error = 1;
				}
				GE_unprotected_mutex_unlock((EIF_POINTER)l_current_thread_context->children_mutex);
				SIGRESUME;
				pthread_attr_destroy(&l_attr);
			} else {
				l_raise_error = 1;
			}
		}
#elif defined EIF_WINDOWS
		SIGBLOCK;
		GE_unprotected_mutex_lock((EIF_POINTER)l_current_thread_context->children_mutex);
			/* Use the mutex even it case of success to force the thread being created to wait for its thread id to be set. */
		l_thread_id = (EIF_THR_TYPE)_beginthreadex(NULL, (unsigned int)l_attr_stack_size, GE_thread_routine, l_thread_context, 0, NULL);
		if (l_thread_id == 0) {
			l_raise_error = 1;
		} else {
			l_thread_context->thread_id = l_thread_id;
#ifdef GE_USE_SCOOP
			if (!l_thread_context->is_scoop_processor) {
#endif
				l_current_thread_context->last_thread_id = l_thread_id;
				l_current_thread_context->n_children++;
#ifdef GE_USE_SCOOP
			}
#endif
		}
		GE_unprotected_mutex_unlock((EIF_POINTER)l_current_thread_context->children_mutex);
		SIGRESUME;
#endif
#if defined GE_USE_POSIX_THREADS || defined EIF_WINDOWS
		if (l_raise_error) {
#endif
			GE_free(l_thread_context);
			GE_raise_with_message(GE_EX_EXT, "Cannot create thread");
#if defined GE_USE_POSIX_THREADS || defined EIF_WINDOWS
		}
#endif
	}
}

/*
 * Execution context of current thread.
 */
GE_context* GE_thread_current_context(void)
{
	GE_context* volatile l_context;

	EIF_TSD_GET(GE_context*, GE_thread_context_key, l_context, "Cannot get execution context for current thread");
	return l_context;
}

/*
 * Thread ID of current thread.
 */
EIF_POINTER GE_thread_id(void)
{
	GE_context* volatile l_context;

	EIF_TSD_GET(GE_context*, GE_thread_context_key, l_context, "Cannot get execution context for current thread");
	return (EIF_POINTER)(l_context->thread->thread_id);
}

/*
 * Thread ID of last thread created from current thread.
 */
EIF_POINTER GE_last_thread_created(void)
{
	GE_context* volatile l_context;

	EIF_TSD_GET(GE_context*, GE_thread_context_key, l_context, "Cannot get execution context for current thread");
	return (EIF_POINTER)(l_context->thread->last_thread_id);
}

#ifdef EIF_WINDOWS
/*
 * Support for Windows GUI that requires that all GUI operations are performed in the same thread.
 * Allocate new structure of the given size `a_size', assign it to `wel_per_thread_data'.
 * Return newly allocated memory block. It will be freed automatically on thread termination.
 */
void* GE_thread_create_wel_per_thread_data(size_t a_size)
{
	void* l_result;
	GE_context* volatile l_context;

	EIF_TSD_GET(GE_context*, GE_thread_context_key, l_context, "Cannot get execution context for current thread");
	l_result = (void*)GE_calloc_uncollectable(1, a_size);
	l_context->wel_per_thread_data = l_result;
	return l_result;
}
#endif

/*
 * Waits until a thread sets `terminated' from `obj' to True,
 * which means it is terminated. The calling thread must be the
 * direct parent of the thread, or the function might loop indefinitely.
 */
void GE_thread_wait(EIF_REFERENCE obj, EIF_BOOLEAN (*get_terminated)(EIF_REFERENCE))
{
	GE_context* volatile l_context;
	GE_thread_context* l_thread_context;

	EIF_TSD_GET(GE_context*, GE_thread_context_key, l_context, "Cannot get execution context for current thread");
	l_thread_context = l_context->thread;
		/* If no thread has been launched, the mutex isn't initialized. */
	if (l_thread_context->children_mutex) {
		SIGBLOCK;
		GE_unprotected_mutex_lock((EIF_POINTER)l_thread_context->children_mutex);
		while (get_terminated(obj) == EIF_FALSE) {
			GE_unprotected_condition_variable_wait((EIF_POINTER)l_thread_context->children_cond, (EIF_POINTER)l_thread_context->children_mutex);
		}
		GE_unprotected_mutex_unlock((EIF_POINTER)l_thread_context->children_mutex);
		SIGRESUME;
	}
}

/*
 * Waits until a thread sets `terminated' from `obj' to True,
 * which means it is terminated, or reaching `a_timeout_ms'.
 * The calling thread must be the direct parent of the thread,
 * or the function might loop indefinitely.
 */
EIF_BOOLEAN GE_thread_wait_with_timeout(EIF_REFERENCE obj, EIF_BOOLEAN (*get_terminated)(EIF_REFERENCE), EIF_NATURAL_64 a_timeout_ms)
{
	GE_context* volatile l_context;
	GE_thread_context* l_thread_context;
	int l_result = GE_THREAD_OK;

	EIF_TSD_GET(GE_context*, GE_thread_context_key, l_context, "Cannot get execution context for current thread");
	l_thread_context = l_context->thread;
		/* If no thread has been launched, the mutex isn't initialized. */
	if (l_thread_context->children_mutex) {
		SIGBLOCK;
		GE_unprotected_mutex_lock((EIF_POINTER)l_thread_context->children_mutex);
		while ((get_terminated(obj) == EIF_FALSE) && (l_result == GE_THREAD_OK)) {
			l_result = GE_unprotected_condition_variable_wait_with_timeout((EIF_POINTER)l_thread_context->children_cond, (EIF_POINTER)l_thread_context->children_mutex, a_timeout_ms);
		}
		GE_unprotected_mutex_unlock((EIF_POINTER)l_thread_context->children_mutex);
		SIGRESUME;
	}
	return (l_result == GE_THREAD_TIMEDOUT ? EIF_FALSE : EIF_TRUE);
}

/*
 * Yields execution to other threads.
 */
void GE_thread_yield(void)
{
#ifdef GE_USE_POSIX_THREADS
#ifdef _POSIX_PRIORITY_SCHEDULING
	sched_yield();
#else
	usleep(1);
#endif
#elif defined EIF_WINDOWS
	if (SwitchToThread()) {
			/* We are able to give our timeslice to another thread of equal priority running on the same processor. */
	} else {
			/* There are no waiting threads on the current processor with the same thread priority so Sleep(0) yields to any available threads on any processor.
			 * However the current thread is still marked as runnable so if there are no threads of the same thread priority in a waiting state then it will return
			 * immediately and not relinquish our timeslice, which Sleep(1) will do but unfortunately the timer resolution may be up to 15ms on multicore systems before returning. */
		Sleep(0);
	}
#endif
}

/*
 * The calling thread waits for all other children threads to terminate.
 */
void GE_thread_join_all(void)
{
	GE_context* volatile l_context;
	GE_thread_context* l_thread_context;

	EIF_TSD_GET(GE_context*, GE_thread_context_key, l_context, "Cannot get execution context for current thread");
	l_thread_context = l_context->thread;
		/* If no thread has been launched, the mutex isn't initialized. */
	if (l_thread_context->children_mutex) {
		SIGBLOCK;
		GE_unprotected_mutex_lock((EIF_POINTER)l_thread_context->children_mutex);
		while (l_thread_context->n_children > 0) {
			GE_unprotected_condition_variable_wait((EIF_POINTER)l_thread_context->children_cond, (EIF_POINTER)l_thread_context->children_mutex);
		}
		GE_unprotected_mutex_unlock((EIF_POINTER)l_thread_context->children_mutex);
		SIGRESUME;
	}
}

/*
 * Function called to terminate a thread launched by Eiffel with `GE_thread_create_with_attr'.
 * This function must be called from the thread itself (not the parent).
 */
void GE_thread_exit(void)
{
	GE_context* volatile l_context;
	GE_thread_context* l_thread_context;
	GE_thread_context* l_parent_thread_context;
	EIF_THR_TYPE l_thread_id;
	int l_free_thread_context;

	EIF_TSD_GET(GE_context*, GE_thread_context_key, l_context, "Cannot get execution context for current thread");
	l_thread_context = l_context->thread;
	if (!l_thread_context->thread_exiting) {
		l_thread_context->thread_exiting = 1;
		if (l_thread_context->current) {
			if (l_thread_context->set_terminated) {
				l_thread_context->set_terminated(l_thread_context->current, EIF_TRUE);
			}
			l_thread_context->current = EIF_VOID;
		}
		l_parent_thread_context = l_thread_context->parent_context;
		if (l_parent_thread_context) {
			SIGBLOCK;
			GE_unprotected_mutex_lock((EIF_POINTER)l_parent_thread_context->children_mutex);
			l_parent_thread_context->n_children--;
			l_free_thread_context = (!l_parent_thread_context->is_alive) && (l_parent_thread_context->n_children <= 0);
			GE_unprotected_condition_variable_broadcast((EIF_POINTER)l_parent_thread_context->children_cond);
			GE_unprotected_mutex_unlock((EIF_POINTER)l_parent_thread_context->children_mutex);
			SIGRESUME;
			if (l_free_thread_context) {
				GE_unprotected_mutex_destroy((EIF_POINTER)l_parent_thread_context->children_mutex);
				GE_unprotected_condition_variable_destroy((EIF_POINTER)l_parent_thread_context->children_cond);
				GE_free(l_parent_thread_context);
				l_thread_context->parent_context = NULL;
			}
		}
		l_thread_id = l_thread_context->thread_id;
		if (l_thread_context->children_mutex) {
			SIGBLOCK;
			GE_unprotected_mutex_lock((EIF_POINTER)l_thread_context->children_mutex);
			l_free_thread_context = (l_thread_context->n_children <= 0);
			if (!l_free_thread_context) {
					/* We cannot destroy ourself because we still have some running children
					 * threads, we therefore need to mark ourself dead. */
				l_thread_context->is_alive = 0;
			}
			GE_unprotected_mutex_unlock((EIF_POINTER)l_thread_context->children_mutex);
			SIGRESUME;
		} else {
			l_free_thread_context = 1;
		}
		if (l_free_thread_context) {
			if (l_thread_context->children_mutex) {
				GE_unprotected_mutex_destroy((EIF_POINTER)l_thread_context->children_mutex);
				GE_unprotected_condition_variable_destroy((EIF_POINTER)l_thread_context->children_cond);
			}
			GE_free(l_thread_context);
		}
#ifdef EIF_WINDOWS
		if (!CloseHandle(l_thread_id)) {
			GE_raise_with_message(GE_EX_EXT, "Cannot close thread");
		}
#endif
		EIF_TSD_SET(GE_thread_context_key, 0, "Cannot remove thread context to TSD.");
		GE_free_onces(l_context->process_onces);
		GE_free_onces(l_context->thread_onces);
#ifdef GE_USE_SCOOP
		GE_free_scoop_processor(l_context->scoop_processor);
#endif
		GE_free_exception(l_context);
		if (l_context->wel_per_thread_data) {
			GE_free(l_context->wel_per_thread_data);
			l_context->wel_per_thread_data = 0;
		}
		GE_free(l_context);
#ifdef GE_USE_POSIX_THREADS
		pthread_exit(NULL);
#elif defined EIF_WINDOWS
		_endthreadex(0);
#endif
	}
}

/*
 * Default thread priority level.
 */
EIF_INTEGER GE_thread_default_priority(void)
{
	return EIF_DEFAULT_THR_PRIORITY;
}

/*
 * Minimum thread priority level.
 */
EIF_INTEGER GE_thread_min_priority(void)
{
	return EIF_MIN_THR_PRIORITY;
}

/*
 * Maximum thread priority level.
 */
EIF_INTEGER GE_thread_max_priority(void)
{
	return EIF_MAX_THR_PRIORITY;
}

#ifdef __cplusplus
}
#endif

#endif
#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"C functions used to implement class CONSOLE"

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2007-2018, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef GE_CONSOLE_C
#define GE_CONSOLE_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef GE_CONSOLE_H
#include "ge_console.h"
#endif
#ifndef GE_EIFFEL_H
#include "ge_eiffel.h"
#endif
#ifndef GE_EXCEPTION_H
#include "ge_exception.h"
#endif
#ifdef GE_USE_THREADS
#ifndef GE_THREAD_H
#include "ge_thread.h"
#endif
#endif

#ifdef EIF_WINDOWS
#include <stdio.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef EIF_WINDOWS

static EIF_BOOLEAN GE_console_allocated = EIF_FALSE;
#ifdef GE_USE_THREADS
static EIF_POINTER GE_console_mutex = 0;
#endif

/*
 * Initialize mutex to determine whether a new
 * console needs to be created.
 */
void GE_init_console(void)
{
#ifdef GE_USE_THREADS
	GE_console_mutex = GE_mutex_create();
#endif
}

/*
 * Create a new DOS console if needed (i.e. in case of a Windows application).
 */
void GE_show_console(void)
{
#ifdef GE_USE_THREADS
	if (GE_console_mutex) {
		GE_mutex_lock(GE_console_mutex);
#endif
	if (!GE_console_allocated) {
		HANDLE hconin, hconout, hconerr;
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		BOOL bLaunched;
		BOOL bSuccess;
		int hCrt;
#ifdef __LCC__
extern FILE * __cdecl _fdopen(int, const char *);
extern int _open_osfhandle(long, int);
#elif !defined EIF_BORLAND
		FILE *hf;
#endif

		bSuccess = AllocConsole();
			/* Get all Std handles and raise an IO exception if we fail getting one. */
		hconout = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hconout == INVALID_HANDLE_VALUE) {
#ifdef GE_USE_THREADS
			GE_mutex_unlock(GE_console_mutex);
#endif
			GE_raise(GE_EX_PROG);
		}
		hconerr = GetStdHandle(STD_ERROR_HANDLE);
		if (hconerr == INVALID_HANDLE_VALUE) {
#ifdef GE_USE_THREADS
			GE_mutex_unlock(GE_console_mutex);
#endif
			GE_raise(GE_EX_PROG);
		}
		hconin = GetStdHandle(STD_INPUT_HANDLE);
		if (hconin == INVALID_HANDLE_VALUE) {
#ifdef GE_USE_THREADS
			GE_mutex_unlock(GE_console_mutex);
#endif
			GE_raise(GE_EX_PROG);
		}
		if (bSuccess) {
				/*
					Console was manually created, we are most likely in
					a Windows application that tries to output something.
					Therefore we need to correctly associated all standard
					handles `stdin', `stdout' and `stderr' to the new created console.
					Note: For Borland, the above trick does not work, one has to
					duplicate the handle, unfortunately the solution does not work
					with Microsoft which explains the ifdef statement.
				*/
#if defined(__LCC__) || defined(EIF_BORLAND)
			hCrt = _open_osfhandle((intptr_t)hconout, _O_TEXT);
			dup2(hCrt, _fileno(stdout));
			hf = _fdopen (hCrt, "w");
			*stdout = *hf;
			setvbuf(stdout, NULL, _IONBF, 0);
#else
			if (!freopen("CONOUT$", "w", stdout)) {
#ifdef GE_USE_THREADS
				GE_mutex_unlock(GE_console_mutex);
#endif
				GE_raise(GE_EX_PROG);
			}
#endif
#if defined(__LCC__) || defined(EIF_BORLAND)
			hCrt = _open_osfhandle((intptr_t)hconerr, _O_TEXT);
			dup2(hCrt, _fileno(stderr));
			hf = _fdopen(hCrt, "w");
			*stderr = *hf;
			setvbuf(stderr, NULL, _IONBF, 0);
#else
			/* There is no "CONERR$". Use "CONOUT$ instead. "*/
			if (!freopen("CONOUT$", "w", stderr)) {
#ifdef GE_USE_THREADS
				GE_mutex_unlock(GE_console_mutex);
#endif
				GE_raise(GE_EX_PROG);
			}
#endif
#if defined(__LCC__) || defined(EIF_BORLAND)
			hCrt = _open_osfhandle((intptr_t)hconin, _O_TEXT | _O_RDONLY);
			dup2(hCrt, _fileno(stdin));
			hf = _fdopen(hCrt, "r");
			*stdin = *hf;
#else
			if (!freopen("CONIN$", "r", stdin)) {
#ifdef GE_USE_THREADS
				GE_mutex_unlock(GE_console_mutex);
#endif
				GE_raise(GE_EX_PROG);
			}
#endif
		}
			/*
				We are computing the cursor position to figure out, if the application
				has been launched from a DOS console or from the Windows Shell.
			*/
/*
		GetConsoleScreenBufferInfo(hconout, &csbi);
		bLaunched = ((csbi.dwCursorPosition.X == 0) && (csbi.dwCursorPosition.Y == 0));
		if ((csbi.dwSize.X <= 0) || (csbi.dwSize.Y <= 0))
			bLaunched = FALSE;

		if (bLaunched == TRUE)
			eif_register_cleanup (eif_console_cleanup);
*/

		GE_console_allocated = EIF_TRUE;
	}
#ifdef GE_USE_THREADS
		GE_mutex_unlock(GE_console_mutex);
	}
#endif
}

#endif

#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"C functions used to implement class ARGUMENTS"

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2007-2024, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef GE_ARGUMENTS_C
#define GE_ARGUMENTS_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef GE_ARGUMENTS_H
#include "ge_arguments.h"
#endif
#ifndef GE_CONSOLE_H
#include "ge_console.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

int GE_argc;
EIF_NATIVE_CHAR** GE_argv;

	/* Clean up function */
typedef void (* EIF_CLEANUP)(EIF_BOOLEAN);
void eif_register_cleanup(EIF_CLEANUP f)
{
	/* TODO: Needed to compile EiffelNet */
}

#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"C functions used to implement the program initialization"

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2007-2017, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef GE_MAIN_C
#define GE_MAIN_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef GE_MAIN_H
#include "ge_main.h"
#endif
#ifndef GE_ARGUMENTS_H
#include "ge_arguments.h"
#endif
#ifndef GE_NATIVE_STRING_H
#include "ge_native_string.h"
#endif
#ifndef GE_GC_H
#include "ge_gc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * System name.
 */
char* GE_system_name = NULL;

/*
 * Root class name.
 */
char* GE_root_class_name = NULL;

#ifdef EIF_WINDOWS

/*
 * Used in WEL.
 */
HINSTANCE eif_hInstance;
HINSTANCE eif_hPrevInstance;
LPWSTR eif_lpCmdLine;
int eif_nCmdShow;

/*
 * Break the shell command held in `cmd', putting each shell word
 * in a separate array entry, hence building an argument
 * suitable for the 'main'. Note that `cmd' will be altered
 * and `argvp' will point to some chunks of it.
 */
void GE_get_argcargv(EIF_NATIVE_CHAR* cmd, int* argc, EIF_NATIVE_CHAR*** argvp)
{
	int quoted = 0; /* parsing inside a quoted string? */
	int nbs; /* number of backspaces */
	int i;
	EIF_NATIVE_CHAR *p = NULL, *pe = NULL; /* pointers in `cmd' */
	EIF_NATIVE_CHAR *qb = NULL, *q = NULL; /* pointers in arguments */

	*argc = 0;
	/* Remove leading and trailing white spaces */
	for (p = cmd; *p == L' ' || *p == L'\t'; p++)
		; /* empty */
	for (pe = p + GE_nstrlen(p) - 1; pe >= p && (*pe == L' ' || *pe == L'\t'); pe--)
		; /* empty */
	if (p <= pe) {
		*argc = *argc + 1; /* at least one argument */
		qb = q = GE_unprotected_malloc_atomic_uncollectable((pe - p + 2) * sizeof(EIF_NATIVE_CHAR));
		if (!qb) {
			return;
		}
		do {
			switch(*p) {
				case L' ':
				case L'\t':
					if (quoted) {
						do {
							*q++ = *p++;
						} while(*p == L' ' || *p == L'\t');
					} else {
						do {
							p++;
						} while(*p == L' ' || *p == L'\t');
						*q++ = L'\0';
						*argc = *argc + 1;
					}
					break;
				case L'\"':
					quoted = ! quoted;
					p++;
					break;
				case L'\\':
					for (nbs = 0; *p == L'\\'; nbs++)
						*q++ = *p++;
					if (*p == L'\"') {
						if (nbs % 2) { /* odd number of backslashes */
							q -= (nbs + 1) / 2;
							*q++ = *p++;
						} else { /* even number of backslashes */
							quoted = ! quoted;
							q -= nbs / 2;
							p++;
						}
					}
					break;
				default:
					*q++ = *p++;
			}
		} while (p <= pe);
		*q++ = L'\0';
	}

	if (!argvp) {
		GE_free(qb);
		return;
	}

	*argvp = (EIF_NATIVE_CHAR**)GE_unprotected_malloc_atomic_uncollectable((*argc+1)*sizeof(EIF_NATIVE_CHAR*));
	if (!(*argvp)) {
		GE_free(qb);
		return;
	}

	for (i = 0; i < *argc; i++) {
		(*argvp)[i] = qb;
		qb += GE_nstrlen(qb) + 1;
	}
	(*argvp)[i] = (EIF_NATIVE_CHAR*)0;
}

int main(int dummy_arg1, char** dummy_argv)
{
	int code;
	int argc;
	EIF_NATIVE_CHAR** argv;
	EIF_NATIVE_CHAR* cmd;

	GE_set_windows_exception_filter();
		/* Variables used in WEL. */
	eif_hInstance = GetModuleHandle(NULL);
	eif_hPrevInstance = NULL;
	eif_lpCmdLine = GetCommandLineW();
	eif_nCmdShow = SW_SHOW;
	cmd = GE_nstrdup(GetCommandLineW());
	GE_get_argcargv(cmd, &argc, &argv);
	code = GE_main(argc, argv);
	free(cmd);
	if (argc > 0) {
		GE_free(argv[0]);
	}
	GE_free(argv);
	return code;
}

/*
 * Main entry point when compiling a Windows application.
 * See:
 *    http://en.wikipedia.org/wiki/WinMain
 *    http://msdn2.microsoft.com/en-us/library/ms633559.aspx
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return main(0, NULL);
}

#else

int main(int argc, char** argv)
{
	return GE_main(argc, argv);
}

#endif

#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"C functions used to implement class EXCEPTION"

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2007-2024, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef GE_EXCEPTION_C
#define GE_EXCEPTION_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef GE_EXCEPTION_H
#include "ge_exception.h"
#endif
#ifndef GE_GC_H
#include "ge_gc.h"
#endif
#ifndef GE_STRING_H
#include "ge_string.h"
#endif
#ifndef GE_CONSOLE_H
#include "ge_console.h"
#endif
#ifndef GE_MAIN_H
#include "ge_main.h"
#endif
#ifdef GE_USE_THREADS
#ifndef GE_THREAD_H
#include "ge_thread.h"
#endif
#endif

#ifdef EIF_WINDOWS
#ifdef __LCC__
#include <windows.h>
#else
#include <winbase.h>
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Pre-defined exception tags. No restriction on size.
 */
static char* GE_exception_tags[] = {
	(char *) 0,							/* Nothing */
	"Feature call on void target.",		/* GE_EX_VOID */
	"No more memory.",					/* GE_EX_MEM */
	"Precondition violated.",			/* GE_EX_PRE */
	"Postcondition violated.",			/* GE_EX_POST */
	"Floating point exception.",		/* GE_EX_FLOAT */
	"Class invariant violated.",		/* GE_EX_CINV */
	"Assertion violated.",				/* GE_EX_CHECK */
	"Routine failure.",					/* GE_EX_FAIL */
	"Unmatched inspect value.",			/* GE_EX_WHEN */
	"Non-decreasing loop variant or negative value reached.",		/* GE_EX_VAR */
	"Loop invariant violated.",			/* GE_EX_LINV */
	"Operating system signal.",			/* GE_EX_SIG */
	"Eiffel run-time panic.",			/* GE_EX_BYE */
	"Exception in rescue clause.",		/* GE_EX_RESC */
	"Out of memory.",					/* GE_EX_OMEM */
	"Resumption attempt failed.",		/* GE_EX_RES */
	"Create on deferred.",				/* GE_EX_CDEF */
	"External event.",					/* GE_EX_EXT */
	"Void assigned to expanded.",		/* GE_EX_VEXP */
	"Exception in signal handler.",		/* GE_EX_HDLR */
	"I/O error.",						/* GE_EX_IO */
	"Operating system error.",			/* GE_EX_SYS */
	"Retrieval error.",					/* GE_EX_RETR */
	"Developer exception.",				/* GE_EX_PROG */
	"Eiffel run-time fatal error.",		/* GE_EX_FATAL */
	"CECIL cannot call melted code",	/* GE_EX_DOL */
	"Runtime I/O error.",				/* GE_EX_ISE_IO */
	"COM error.",						/* GE_EX_COM */
	"Runtime check violated.",			/* GE_EX_RT_CHECK */
	"Old expression evaluation failed.",/* GE_EX_OLD */
	"Serialization failed."				/* GE_EX_SEL */
	"SCOOP processor dirty."			/* GE_EX_DIRTY */
};

/*
 * Exception tag associated with `a_code'.
 */
char* GE_exception_tag(long a_code)
{
	if (a_code < 0) {
		return "User-defined exception.";
	} else if (a_code < 1 || a_code > GE_EX_NEX) {
		return "Unknown exception.";
	} else {
		return GE_exception_tags[a_code];
	}
}

/*
 * Initialize `a_trace'.
 */
static void GE_init_exception_trace_buffer(GE_exception_trace_buffer* a_trace)
{
	a_trace->count = 0;
	a_trace->capacity = 0;
	a_trace->area = (char*)GE_unprotected_calloc_atomic_uncollectable(4096, 1);
	if (a_trace->area) {
		a_trace->capacity = 4096;
	}
}

/*
 * Free memory allocated in `a_trace'.
 */
static void GE_free_exception_trace_buffer(GE_exception_trace_buffer* a_trace)
{
	a_trace->count = 0;
	a_trace->capacity = 0;
	if (a_trace->area) {
	 GE_free(a_trace->area);
	}
}

/*
 * Append `a_string' to `a_trace'.
 * Resize area if needed.
 */
static void GE_append_to_exception_trace_buffer(GE_exception_trace_buffer* a_trace, char* a_string)
{
	int l_length = strlen(a_string);
	uint32_t l_new_capacity;
	char* l_new_area;

	if (a_trace->area) {
		if ((a_trace->capacity - a_trace->count) <= l_length) {
			l_new_capacity = a_trace->capacity + l_length + 512;
			l_new_area = (char*)GE_unprotected_recalloc(a_trace->area, a_trace->capacity, l_new_capacity, 1);
			if (l_new_area) {
				a_trace->area = l_new_area;
				a_trace->capacity = l_new_capacity;
			}
		}
	} else {
		GE_init_exception_trace_buffer(a_trace);
	}
	if ((a_trace->capacity - a_trace->count) > l_length) {
		strcpy(a_trace->area + a_trace->count, a_string);
		a_trace->count += l_length;
	}
}

/*
 * Wipe out `a_trace'.
 */
static void GE_wipe_out_exception_trace_buffer(GE_exception_trace_buffer* a_trace)
{
	char* l_area = a_trace->area;

	if (l_area) {
		memset(l_area, 0, a_trace->count);
		a_trace->count = 0;
	}
}

/*
 * Print first line of exception which contains `a_class_name', `a_feature_name'
 * and `a_tag_name' to `a_trace'.
 */
static void GE_print_class_feature_tag(GE_exception_trace_buffer* a_trace, const char* a_class_name, const char* a_feature_name, const char* a_tag_name)
{
	char buffer[256];
	int l_class_count, l_feature_count, l_tag_count;

		/* We are trying to provide the best format possible so that we can see all the useful
		 * information about the location and the reason of the crash.
		 * Note that the C format `%width[.precision]s' means that we will display `width'
		 * characters on the screen and only `precision' characters from `s' will be displayed,
		 * meaning that if `precision' is smaller than `width' the text is right aligned.
		 *
		 * Note: because `buffer' has a fixed size of 256, we need to use `precision' to avoid
		 * writting more than `buffer' can hold. And for `sprintf', a null character is appended
		 * after the last character written, which should be taken into account.
		 */
	l_class_count = (int)strlen(a_class_name);
	l_feature_count = (int)strlen(a_feature_name);
	l_tag_count = (int)strlen(a_tag_name);

		/* 1 - precision of 211 = 254 - 43, 43 being number of characters written
		 *      for `a_class_name' and `a_feature_name'. */
		/* 2 - precision of 234 = 254 - 20, 20 being number of characters written
		 *      for `a_class_name'.*/
		/* 3 - precision of 254, 254 being number of characters written
		 *      excluding `\n' and null character appended at the end */
		/* 4 - precision of 251, 231 and 208 being number of characters written
		 *      excluding eclipse from above numbers. */
	if (l_class_count > 19) {
		if (l_class_count > 251) {
			sprintf(buffer, "%.251s...\n", a_class_name);
		} else {
			sprintf(buffer, "%*.254s\n", l_class_count, a_class_name);
		}
		GE_append_to_exception_trace_buffer(a_trace, buffer);
		if (l_feature_count > 22) {
			if (l_feature_count > 231) {
				sprintf(buffer, "%.231s...\n", a_feature_name);
			} else {
				sprintf(buffer, "%*.234s\n", 20 + l_feature_count, a_feature_name);
			}
			GE_append_to_exception_trace_buffer(a_trace, buffer);
			if (l_tag_count > 0) {
				if (l_tag_count > 208) {
					sprintf(buffer, "%.208s...\n", a_tag_name);
				} else {
					sprintf(buffer, "%*.211s\n", 43 + l_tag_count, a_tag_name);
				}
				GE_append_to_exception_trace_buffer(a_trace, buffer);
			}
		} else {
			if (l_tag_count > 208) {
				sprintf(buffer, "%*.22s %.208s...\n", 20 + l_feature_count, a_feature_name, a_tag_name);
			} else {
				sprintf(buffer, "%*.22s %*.211s\n", 20 + l_feature_count, a_feature_name, (43 + l_tag_count) - (20 + l_feature_count + 1), a_tag_name);
			}
			GE_append_to_exception_trace_buffer(a_trace, buffer);
		}
	} else {
		if (l_feature_count > 22) {
			if (l_feature_count > 208) {
				sprintf(buffer, "%-19.19s %.208s...\n", a_class_name, a_feature_name);
			} else {
				sprintf(buffer, "%-19.19s %*.211s\n", a_class_name, l_feature_count, a_feature_name);
			}
			GE_append_to_exception_trace_buffer(a_trace, buffer);
			if (l_tag_count > 0) {
				if (l_tag_count > 208) {
					sprintf(buffer, "%.208s...\n", a_tag_name);
				} else {
					sprintf(buffer, "%*.211s\n", 43 + l_tag_count, a_tag_name);
				}
				GE_append_to_exception_trace_buffer(a_trace, buffer);
			}
		} else {
			if (l_tag_count > 208) {
				sprintf(buffer, "%-19.19s %-22.22s %-29.208s...\n", a_class_name, a_feature_name, a_tag_name);
			} else {
				sprintf(buffer, "%-19.19s %-22.22s %-29.211s\n", a_class_name, a_feature_name, a_tag_name);
			}
			GE_append_to_exception_trace_buffer(a_trace, buffer);
		}
	}
}

/*
 * Print second line of exception which contains `a_object_addr', `a_location',
 * `a_reason' and `a_effect' to `a_trace'.
 */
static void GE_print_object_location_reason_effect(GE_exception_trace_buffer* a_trace, void* a_object_addr, char* a_location, char* a_reason, char* a_effect)
{
	char buffer[256];
	int l_location_count, l_reason_count, l_effect_count;

		/* We are trying to provide the best format possible so that we can see all the useful
		 * information about the location and the reason of the crash.
		 * Note that the C format `%width[.precision]s' means that we will display `width'
		 * characters on the screen and only `precision' characters from `s' will be displayed,
		 * meaning that if `precision' is smaller than `width' the text is right aligned.
		 *
		 * Note: because `buffer' has a fixed size of 256, we need to use `precision' to avoid
		 * writting more than `buffer' can hold.
		 */

	l_location_count = (int)strlen(a_location);
	l_reason_count = (int)strlen(a_reason);
	l_effect_count = (int)strlen(a_effect);

		/* 1 - precision of 211 = 254 - 43, 43 being number of characters written
			   for `a_object_addr' and `a_location'.
		 * 2 - precision of 181 = 254 - 73, 73 being number of characters written
			   for `a_object_addr', `a_location' and `a_reason'.
		 * 3 - 22, number of characters written for `a_location'
		 * 4 - 29, number of characters written for `a_reason' */

		/* Check this for formatting using `printf':
		 * http://en.wikipedia.org/wiki/Printf#printf_format_placeholders */

		/* Print object address with 16 digits to be ready when pointers
		 * will be on 64 bits on all platform. */
	sprintf(buffer, "<%016" EIF_POINTER_DISPLAY ">  ", (unsigned long)(intptr_t)a_object_addr);
	GE_append_to_exception_trace_buffer(a_trace, buffer);

	if (l_location_count > 22) {
		sprintf(buffer, "%*.254s\n", l_location_count, a_location);
		GE_append_to_exception_trace_buffer(a_trace, buffer);
		if (l_reason_count > 29) {
			sprintf(buffer, "%*.211s\n", 43 + l_reason_count, a_reason);
			GE_append_to_exception_trace_buffer(a_trace, buffer);
			sprintf(buffer, "%*.181s\n", 73 + l_effect_count, a_effect);
			GE_append_to_exception_trace_buffer(a_trace, buffer);
		} else {
			sprintf(buffer, "%*.29s %*.181s\n", 43 + l_reason_count, a_reason, (73 + l_effect_count) - (43 + l_reason_count + 1), a_effect);
			GE_append_to_exception_trace_buffer(a_trace, buffer);
		}
	} else {
		if (l_reason_count > 29) {
			sprintf(buffer,"%-22.22s %*.211s\n", a_location, l_reason_count, a_reason);
			GE_append_to_exception_trace_buffer(a_trace, buffer);
			sprintf(buffer, "%*.181s\n", 73 + l_effect_count, a_effect);
			GE_append_to_exception_trace_buffer(a_trace, buffer);
		} else {
			sprintf(buffer,"%-22.22s %-29.29s %*.181s\n", a_location, a_reason, l_effect_count, a_effect);
			GE_append_to_exception_trace_buffer(a_trace, buffer);
		}
	}
}

/*
 * Print to `a_trace' the exception trace corresponding to `a_context'.
 */
static void GE_print_exception_trace(GE_context* a_context, long a_code, const char* a_tag, GE_exception_trace_buffer* a_trace)
{
	char buffer[1024];
	GE_call* l_call;
	char* l_tag;
	char l_tag_buf[256];
	void* l_object = NULL;
	char* l_location = "";
	char* l_reason;
	char* l_effect;
	char* l_root_class;
	char* l_root_feature = "root's creation";
	const char* l_class_name;
	const char* l_feature_name;

#ifdef GE_USE_THREADS
	sprintf(buffer, "%s\n", "******************************** Thread exception *****************************");
	GE_append_to_exception_trace_buffer(a_trace, buffer);
	if (a_context == GE_main_context) {
			/* Main thread. */
		sprintf(buffer,"%-19.19s %-22.22s 0x%" EIF_POINTER_DISPLAY " %s\n", "In thread", "Root thread", (unsigned long)0, "(thread id)");
	} else {
		sprintf(buffer,"%-19.19s %-22.22s 0x%" EIF_POINTER_DISPLAY " %s\n", "In thread", "Child thread", (unsigned long)(uint64_t)a_context->thread->thread_id, "(thread id)");
	}
	GE_append_to_exception_trace_buffer(a_trace, buffer);
	sprintf(buffer, "%s\n", "*******************************************************************************");
	GE_append_to_exception_trace_buffer(a_trace, buffer);
#endif
	sprintf(buffer, "%s\n", "-------------------------------------------------------------------------------");
	GE_append_to_exception_trace_buffer(a_trace, buffer);
	sprintf(buffer, "%-19.19s %-22.22s %-29.29s %-6.6s\n", "Class / Object", "Routine", "Nature of exception", "Effect");
	GE_append_to_exception_trace_buffer(a_trace, buffer);
	sprintf(buffer, "%s\n", "-------------------------------------------------------------------------------");
	GE_append_to_exception_trace_buffer(a_trace, buffer);
	if (GE_root_class_name) {
		l_root_class = GE_root_class_name;
	} else {
		l_root_class = "ROOT CLASS";
	}
	l_call = a_context->call;
	if (l_call) {
		l_class_name = l_call->class_name;
		l_feature_name = l_call->feature_name;
#ifdef GE_USE_CURRENT_IN_EXCEPTION_TRACE
		l_object = l_call->object;
#endif
	} else {
		l_class_name = l_root_class;
		l_feature_name = l_root_feature;
	}
	if (a_tag) {
		sprintf(l_tag_buf, "%.254s:", a_tag);
		l_tag = l_tag_buf;
	} else {
		l_tag = "";
	}
	GE_print_class_feature_tag(a_trace, l_class_name, l_feature_name, l_tag);
	l_reason = GE_exception_tag(a_code);
	l_effect = "Fail";
	GE_print_object_location_reason_effect(a_trace, l_object, l_location, l_reason, l_effect);
	sprintf(buffer, "%s\n", "-------------------------------------------------------------------------------");
	GE_append_to_exception_trace_buffer(a_trace, buffer);
	while (l_call) {
		l_tag = "";
		GE_print_class_feature_tag(a_trace, l_call->class_name, l_call->feature_name, l_tag);
#ifdef GE_USE_CURRENT_IN_EXCEPTION_TRACE
		l_object = l_call->object;
#endif
		l_reason = "Routine failure.";
		l_effect = "Fail";
		GE_print_object_location_reason_effect(a_trace, l_object, l_location, l_reason, l_effect);
		sprintf(buffer, "%s\n", "-------------------------------------------------------------------------------");
		GE_append_to_exception_trace_buffer(a_trace, buffer);
		l_call = l_call->caller;
	}
	l_tag = "";
	GE_print_class_feature_tag(a_trace, l_root_class, l_root_feature, l_tag);
#ifdef GE_USE_CURRENT_IN_EXCEPTION_TRACE
	l_object = NULL;
#endif
	l_reason = "Routine failure.";
	l_effect = "Exit";
	GE_print_object_location_reason_effect(a_trace, l_object, l_location, l_reason, l_effect);
	sprintf(buffer, "%s\n", "-------------------------------------------------------------------------------");
	GE_append_to_exception_trace_buffer(a_trace, buffer);
}

/*
 * Default initialization for `GE_context'.
 */
GE_context GE_default_context = {0, 0, 0, 0, 0, 0, '\1', 0, 0, {0, 0, 0}, {0, 0, 0}, 1
#ifdef GE_USE_THREADS
	, 0, 0, 0, 0
#endif
#ifdef GE_USE_SCOOP
	, 0
#endif
	};

/*
 * Execution context of main thread.
 * Should be used from the main thread only.
 */
GE_context* GE_main_context;

/*
 * Execution context of current thread.
 */
GE_context* GE_current_context(void)
{
#ifdef GE_USE_THREADS
	return GE_thread_current_context();
#else
	return GE_main_context;
#endif
}

/*
 * Initialization of exception handling.
 */
void GE_init_exception(GE_context* a_context)
{
	EIF_REFERENCE l_exception_manager;

	GE_init_exception_trace_buffer(&a_context->exception_trace_buffer);
	GE_init_exception_trace_buffer(&a_context->last_exception_trace);
	l_exception_manager = GE_new_exception_manager(a_context, EIF_TRUE);
	a_context->exception_manager = l_exception_manager;
#ifdef GE_USE_THREADS
	a_context->thread->exception_manager = l_exception_manager;
#endif
	GE_init_exception_manager(a_context, l_exception_manager);
}

/*
 * Free memory allocated in `a_context' for exception handling.
 */
void GE_free_exception(GE_context* a_context)
{
	GE_free_exception_trace_buffer(&a_context->exception_trace_buffer);
	GE_free_exception_trace_buffer(&a_context->last_exception_trace);
	a_context->exception_manager = EIF_VOID;
#ifdef GE_USE_THREADS
	a_context->thread->exception_manager = EIF_VOID;
#endif
}

/*
 * Pointer to function to create a new exception manager object
 * (of type ISE_EXCEPTION_MANAGER).
 */
EIF_REFERENCE (*GE_new_exception_manager)(GE_context*, EIF_BOOLEAN);

/*
 * Pointer to Eiffel routine ISE_EXCEPTION_MANAGER.init_exception_manager.
 */
void (*GE_init_exception_manager)(GE_context*, EIF_REFERENCE);

/*
 * Pointer to Eiffel routine ISE_EXCEPTION_MANAGER.last_exception.
 */
EIF_REFERENCE (*GE_last_exception)(GE_context*, EIF_REFERENCE);

/*
 * Pointer to Eiffel routine ISE_EXCEPTION_MANAGER.once_raise.
 */
void (*GE_once_raise)(GE_context*, EIF_REFERENCE, EIF_REFERENCE);

/*
 * Pointer to Eiffel routine ISE_EXCEPTION_MANAGER.set_exception_data.
 */
void (*GE_set_exception_data)(GE_context*, EIF_REFERENCE, EIF_INTEGER_32, EIF_BOOLEAN, EIF_INTEGER_32, EIF_INTEGER_32, EIF_REFERENCE, EIF_REFERENCE, EIF_REFERENCE, EIF_REFERENCE, EIF_REFERENCE, EIF_REFERENCE, EIF_INTEGER_32, EIF_BOOLEAN);

/*
 * Jump to execute the rescue of the last routine with a rescue
 * in the call stack.
 */
void GE_jump_to_last_rescue(GE_context* a_context)
{
	char* l_exception_trace;

	GE_rescue* r = a_context->last_rescue;
	if (r != 0) {
		a_context->last_rescue = r->previous;
		a_context->raising_exception = '\0';
		GE_longjmp(r->jb, 1);
	}
	if (a_context->exception_trace_enabled) {
		GE_show_console();
		if (GE_system_name) {
			fprintf(stderr, "\n%s: system execution failed.\n", GE_system_name);
		} else {
			fprintf(stderr, "\nsystem execution failed.\n");
		}
		l_exception_trace = a_context->last_exception_trace.area;
		if (l_exception_trace) {
			fprintf(stderr, "Following is the set of recorded exceptions:\n\n");
			fprintf(stderr, "%s", l_exception_trace);
		} else {
			fprintf(stderr, "No exception trace found.\n");
		}
	}
	exit(1);
}

/*
 * Call feature EXCEPTION_MANAGER.set_exception_data.
 */
static void GE_call_set_exception_data(GE_context* a_context, long code, int new_obj, int signal_code, int error_code, const char* tag, char* recipient, char* eclass, char* rf_routine, char* rf_class, char* trace, int line_number, int is_invariant_entry)
{
	EIF_REFERENCE l_tag;
	EIF_REFERENCE l_recipient;
	EIF_REFERENCE l_eclass;
	EIF_REFERENCE l_rf_routine;
	EIF_REFERENCE l_rf_class;
	EIF_REFERENCE l_trace;
	EIF_REFERENCE l_exception_manager;

	l_exception_manager = a_context->exception_manager;
	if (!l_exception_manager) {
		GE_init_exception(a_context);
		l_exception_manager = a_context->exception_manager;
	}
	if (tag) {
		l_tag = GE_str(tag);
	} else {
		l_tag = GE_ms("", 0);
	}
	if (recipient) {
		l_recipient = GE_str(recipient);
	} else {
		l_recipient = GE_ms("", 0);
	}
	if (eclass) {
		l_eclass = GE_str(eclass);
	} else {
		l_eclass = GE_ms("", 0);
	}
	if (rf_routine) {
		l_rf_routine = GE_str(rf_routine);
	} else {
		l_rf_routine = GE_ms("", 0);
	}
	if (rf_class) {
		l_rf_class = GE_str(rf_class);
	} else {
		l_rf_class = GE_ms("", 0);
	}
	if (trace && a_context->exception_trace_enabled) {
		l_trace = GE_str(trace);
	} else {
		l_trace = GE_ms("", 0);
	}
	GE_set_exception_data(a_context, l_exception_manager, (EIF_INTEGER_32) code, EIF_TEST(new_obj), (EIF_INTEGER_32) signal_code, (EIF_INTEGER_32) error_code, l_tag, l_recipient, l_eclass, l_rf_routine, l_rf_class, l_trace, (EIF_INTEGER_32) line_number, EIF_TEST(is_invariant_entry));
}

/*
 * Raise an exception.
 */
static void GE_raise_exception(long code, int new_obj, int signal_code, int error_code, const char* tag, char* recipient, char* eclass, char* rf_routine, char* rf_class, char* trace, int line_number, int is_invariant_entry)
{
	char* l_trace;
	GE_exception_trace_buffer* l_trace_buffer;
	GE_context* l_context;

	l_context = GE_current_context();
	if (!l_context) {
		GE_show_console();
		if (GE_system_name) {
			fprintf(stderr, "\n%s: system execution failed.\n", GE_system_name);
		} else {
			fprintf(stderr, "\nsystem execution failed.\n");
		}
		fprintf(stderr, "Panic: No execution context found.\n");
		exit(1);
	} else if (l_context->raising_exception) {
		GE_show_console();
		if (GE_system_name) {
			fprintf(stderr, "\n%s: system execution failed.\n", GE_system_name);
		} else {
			fprintf(stderr, "\nsystem execution failed.\n");
		}
		fprintf(stderr, "Panic: A crash occurred while processing exceptions.\n");
		l_trace = l_context->last_exception_trace.area;
		if (l_trace) {
			fprintf(stderr, "Following is the set of recorded exceptions\n");
			fprintf(stderr, "NB: The raised panic may have induced completely inconsistent information:\n\n");
			fprintf(stderr, "%s", l_trace);
		} else {
			fprintf(stderr, "No exception trace found.\n");
		}
		exit(1);
	} else {
		l_context->raising_exception = '\1';
		l_context->exception_code = code;
		l_context->exception_tag = tag;
		if (code != GE_EX_FAIL) {
			GE_wipe_out_exception_trace_buffer(&l_context->last_exception_trace);
		}
		l_trace = trace;
		if (l_context->exception_trace_enabled && !l_trace) {
			l_trace_buffer = &l_context->exception_trace_buffer;
			GE_wipe_out_exception_trace_buffer(l_trace_buffer);
			GE_print_exception_trace(l_context, code, tag, l_trace_buffer);
			l_trace = l_trace_buffer->area;
		}
		if ((code != GE_EX_FAIL) && l_trace) {
			GE_append_to_exception_trace_buffer(&l_context->last_exception_trace, l_trace);
		}
		GE_call_set_exception_data(l_context, code, new_obj, signal_code, error_code, tag, recipient, eclass, rf_routine, rf_class, l_trace, line_number, is_invariant_entry);
		l_context->raising_exception = '\0';
		l_context->exception_code = 0;
		l_context->exception_tag = (char*)0;
		GE_jump_to_last_rescue(l_context);
	}
}

/*
 * Raise an exception with code `a_code'.
 */
void GE_raise(long a_code)
{
	GE_raise_with_message(a_code, NULL);
}

/*
 * Raise an exception with code `a_code' and message `msg'.
 */
void GE_raise_with_message(long a_code, const char* msg)
{
	GE_raise_exception(a_code, 1, -1, -1, msg, NULL, NULL, NULL, NULL, NULL, -1, 0);
}

/*
 * Raise an exception from EXCEPTION_MANAGER.
 */
void GE_developer_raise(long a_code, char* a_meaning, char* a_message)
{
	GE_raise_exception(a_code, 0, -1, -1, a_message, NULL, NULL, NULL, NULL, NULL, -1, 0);
}

/*
 * Raise exception which was raised the first time a once routine
 * was executed when executing it again.
 */
void GE_raise_once_exception(GE_context* a_context, EIF_REFERENCE a_exception)
{
	EIF_REFERENCE l_exception_manager;

	l_exception_manager = a_context->exception_manager;
	if (l_exception_manager) {
		GE_once_raise(a_context, l_exception_manager, a_exception);
	}
}

/*
 * Exception, if any, which was last raised in `a_context'.
 */
EIF_REFERENCE GE_last_exception_raised(GE_context* a_context)
{
	EIF_REFERENCE l_exception_manager;

	l_exception_manager = a_context->exception_manager;
	if (l_exception_manager) {
		return GE_last_exception(a_context, l_exception_manager);
	}
	return EIF_VOID;
}

/*
 * Set `in_assertion' to 'not b'.
 * Return the opposite of previous value.
 */
EIF_BOOLEAN GE_check_assert(EIF_BOOLEAN b)
{
	EIF_BOOLEAN l_old_value;
	GE_context* l_context;

	l_context = GE_current_context();
	l_old_value = EIF_TEST(!(l_context->in_assertion));
	l_context->in_assertion = (b?0:1);
	return l_old_value;
}

/*
 * Check whether the type id of `obj' is not in `type_ids'.
 * If it is, then raise a CAT-call exception. Don't do anything if `obj' is Void.
 * `nb' is the number of ids in `type_ids' and is expected to be >0.
 * `type_ids' is sorted in increasing order.
 * Return `obj'.
 */
EIF_REFERENCE GE_check_catcall(EIF_REFERENCE obj, EIF_TYPE_INDEX type_ids[], int nb)
{
	if (obj) {
		int type_id = obj->id;
		if (type_id < type_ids[0]) {
			/* Done */
		} else if (type_id > type_ids[nb-1]) {
			/* Done */
		} else {
			int i;
			for (i = 0; i < nb; i++) {
				if (type_id == type_ids[i]) {
					GE_show_console();
					fprintf(stderr, "CAT-call error!\n");
#ifdef EIF_DEBUG
					{
						char c;
						fprintf(stderr, "Press Enter...\n");
						scanf("%c", &c);
					}
#endif
					GE_raise_with_message(GE_EX_PROG, "CAT-call error.");
					break;
				} else if (type_id < type_ids[i]) {
						/* type-ids are sorted in increasing order. */
					break;
				}
			}
		}
	}
	return (obj);
}

/*
 * Check whether `obj' is Void.
 * If it is, then raise a call-on-void-target exception.
 * Return `obj'.
 */
EIF_REFERENCE GE_check_void(EIF_REFERENCE obj)
{
	if (!obj) {
		GE_show_console();
		fprintf(stderr, "Call on Void target!\n");
#ifdef EIF_DEBUG
		{
			char c;
			fprintf(stderr, "Press Enter...\n");
			scanf("%c", &c);
		}
#endif
		GE_raise(GE_EX_VOID);
	}
	return (obj);
}

/*
 * Check whether `ptr' is a null pointer.
 * If it is, then raise a no-more-memory exception.
 * Return `ptr'.
 */
void* GE_check_null(void* ptr)
{
	if (!ptr) {
		GE_show_console();
		fprintf(stderr, "No more memory!\n");
#ifdef EIF_DEBUG
		{
			char c;
			fprintf(stderr, "Press Enter...\n");
			scanf("%c", &c);
		}
#endif
		GE_raise(GE_EX_MEM);
	}
	return (ptr);
}

#ifdef EIF_WINDOWS
static LONG WINAPI GE_windows_exception_filter(LPEXCEPTION_POINTERS a_exception)
{
		/* In order to be able to catch exceptions that cannot be caught by
		 * just using signals on Windows, we need to set `windows_exception_filter'
		 * as an unhandled exception filter.
		 */

	switch (a_exception->ExceptionRecord->ExceptionCode) {
		case STATUS_STACK_OVERFLOW:
			GE_raise_with_message(GE_EX_EXT, "Stack overflow");
			break;

		case STATUS_INTEGER_DIVIDE_BY_ZERO:
			GE_raise_with_message(GE_EX_FLOAT, "Integer division by Zero");
			break;

		default:
			GE_raise_with_message(GE_EX_EXT, "Unhandled exception");
			break;
	}

		/* Possible return values include:
		 * EXCEPTION_CONTINUE_EXECUTION : Returns to the statement that caused the exception
		 *    and re-executes that statement. (Causes an infinite loop of calling the exception
		 *    handler if the handler does not fix the problem)
		 * EXCEPTION_EXECUTE_HANDLER: Passes the exception to default handler, in our case
		 *    none, since `windows_exception_filter' is the default one now.
		 * EXCEPTION_CONTINUE_SEARCH: Continue to search up the stack for a handle
		 */
	return EXCEPTION_EXECUTE_HANDLER;
}

/*
 * Set default exception handler.
 */
void GE_set_windows_exception_filter(void)
{
	LPTOP_LEVEL_EXCEPTION_FILTER old_exception_handler = NULL;
	old_exception_handler = SetUnhandledExceptionFilter (GE_windows_exception_filter);
}
#endif

#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"C functions used to implement once features"

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2017, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef GE_ONCE_C
#define GE_ONCE_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef GE_ONCE_H
#include "ge_once.h"
#endif
#ifndef GE_GC_H
#include "ge_gc.h"
#endif
#ifdef GE_USE_THREADS
#ifndef GE_THREAD_H
#include "ge_thread.h"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Variable to keep track of the call status
 * and results of once-per-process features.
 */
GE_onces* GE_process_onces;

/*
 * Initialize `GE_process_onces'.
 */
void GE_init_onces(
	unsigned int a_boolean_count,
	unsigned int a_character_8_count,
	unsigned int a_character_32_count,
	unsigned int a_integer_8_count,
	unsigned int a_integer_16_count,
	unsigned int a_integer_32_count,
	unsigned int a_integer_64_count,
	unsigned int a_natural_8_count,
	unsigned int a_natural_16_count,
	unsigned int a_natural_32_count,
	unsigned int a_natural_64_count,
	unsigned int a_pointer_count,
	unsigned int a_real_32_count,
	unsigned int a_real_64_count,
	unsigned int a_reference_count,
	unsigned int a_procedure_count)
{
	GE_process_onces = GE_new_onces(
		a_boolean_count,
		a_character_8_count,
		a_character_32_count,
		a_integer_8_count,
		a_integer_16_count,
		a_integer_32_count,
		a_integer_64_count,
		a_natural_8_count,
		a_natural_16_count,
		a_natural_32_count,
		a_natural_64_count,
		a_pointer_count,
		a_real_32_count,
		a_real_64_count,
		a_reference_count,
		a_procedure_count);
#ifdef GE_USE_THREADS
	GE_process_once_mutexes = GE_new_once_mutexes(
		a_boolean_count,
		a_character_8_count,
		a_character_32_count,
		a_integer_8_count,
		a_integer_16_count,
		a_integer_32_count,
		a_integer_64_count,
		a_natural_8_count,
		a_natural_16_count,
		a_natural_32_count,
		a_natural_64_count,
		a_pointer_count,
		a_real_32_count,
		a_real_64_count,
		a_reference_count,
		a_procedure_count);
#endif
}

/*
 * Create a new 'GE_onces' struct which can deal with the
 * numbers of once features passed as argument.
 */
GE_onces* GE_new_onces(
	unsigned int a_boolean_count,
	unsigned int a_character_8_count,
	unsigned int a_character_32_count,
	unsigned int a_integer_8_count,
	unsigned int a_integer_16_count,
	unsigned int a_integer_32_count,
	unsigned int a_integer_64_count,
	unsigned int a_natural_8_count,
	unsigned int a_natural_16_count,
	unsigned int a_natural_32_count,
	unsigned int a_natural_64_count,
	unsigned int a_pointer_count,
	unsigned int a_real_32_count,
	unsigned int a_real_64_count,
	unsigned int a_reference_count,
	unsigned int a_procedure_count)
{
	GE_onces* l_onces;

	l_onces = (GE_onces*)GE_calloc_atomic_uncollectable(1, sizeof(GE_onces));
	if (a_boolean_count > 0) {
		l_onces->boolean_value = (EIF_BOOLEAN*)GE_calloc_atomic_uncollectable(a_boolean_count, sizeof(EIF_BOOLEAN));
		l_onces->boolean_exception = (EIF_REFERENCE*)GE_calloc_uncollectable(a_boolean_count, sizeof(EIF_REFERENCE));
		l_onces->boolean_status = (unsigned char*)GE_calloc_atomic_uncollectable(a_boolean_count, sizeof(unsigned char));
		l_onces->boolean_count = a_boolean_count;
	}
	if (a_character_8_count > 0) {
		l_onces->character_8_value = (EIF_CHARACTER_8*)GE_calloc_atomic_uncollectable(a_character_8_count, sizeof(EIF_CHARACTER_8));
		l_onces->character_8_exception = (EIF_REFERENCE*)GE_calloc_uncollectable(a_character_8_count, sizeof(EIF_REFERENCE));
		l_onces->character_8_status = (unsigned char*)GE_calloc_atomic_uncollectable(a_character_8_count, sizeof(unsigned char));
		l_onces->character_8_count = a_character_8_count;
	}
	if (a_character_32_count > 0) {
		l_onces->character_32_value = (EIF_CHARACTER_32*)GE_calloc_atomic_uncollectable(a_character_32_count, sizeof(EIF_CHARACTER_32));
		l_onces->character_32_exception = (EIF_REFERENCE*)GE_calloc_uncollectable(a_character_32_count, sizeof(EIF_REFERENCE));
		l_onces->character_32_status = (unsigned char*)GE_calloc_atomic_uncollectable(a_character_32_count, sizeof(unsigned char));
		l_onces->character_32_count = a_character_32_count;
	}
	if (a_integer_8_count > 0) {
		l_onces->integer_8_value = (EIF_INTEGER_8*)GE_calloc_atomic_uncollectable(a_integer_8_count, sizeof(EIF_INTEGER_8));
		l_onces->integer_8_exception = (EIF_REFERENCE*)GE_calloc_uncollectable(a_integer_8_count, sizeof(EIF_REFERENCE));
		l_onces->integer_8_status = (unsigned char*)GE_calloc_atomic_uncollectable(a_integer_8_count, sizeof(unsigned char));
		l_onces->integer_8_count = a_integer_8_count;
	}
	if (a_integer_16_count > 0) {
		l_onces->integer_16_value = (EIF_INTEGER_16*)GE_calloc_atomic_uncollectable(a_integer_16_count, sizeof(EIF_INTEGER_16));
		l_onces->integer_16_exception = (EIF_REFERENCE*)GE_calloc_uncollectable(a_integer_16_count, sizeof(EIF_REFERENCE));
		l_onces->integer_16_status = (unsigned char*)GE_calloc_atomic_uncollectable(a_integer_16_count, sizeof(unsigned char));
		l_onces->integer_16_count = a_integer_16_count;
	}
	if (a_integer_32_count > 0) {
		l_onces->integer_32_value = (EIF_INTEGER_32*)GE_calloc_atomic_uncollectable(a_integer_32_count, sizeof(EIF_INTEGER_32));
		l_onces->integer_32_exception = (EIF_REFERENCE*)GE_calloc_uncollectable(a_integer_32_count, sizeof(EIF_REFERENCE));
		l_onces->integer_32_status = (unsigned char*)GE_calloc_atomic_uncollectable(a_integer_32_count, sizeof(unsigned char));
		l_onces->integer_32_count = a_integer_32_count;
	}
	if (a_integer_64_count > 0) {
		l_onces->integer_64_value = (EIF_INTEGER_64*)GE_calloc_atomic_uncollectable(a_integer_64_count, sizeof(EIF_INTEGER_64));
		l_onces->integer_64_exception = (EIF_REFERENCE*)GE_calloc_uncollectable(a_integer_64_count, sizeof(EIF_REFERENCE));
		l_onces->integer_64_status = (unsigned char*)GE_calloc_atomic_uncollectable(a_integer_64_count, sizeof(unsigned char));
		l_onces->integer_64_count = a_integer_64_count;
	}
	if (a_natural_8_count > 0) {
		l_onces->natural_8_value = (EIF_NATURAL_8*)GE_calloc_atomic_uncollectable(a_natural_8_count, sizeof(EIF_NATURAL_8));
		l_onces->natural_8_exception = (EIF_REFERENCE*)GE_calloc_uncollectable(a_natural_8_count, sizeof(EIF_REFERENCE));
		l_onces->natural_8_status = (unsigned char*)GE_calloc_atomic_uncollectable(a_natural_8_count, sizeof(unsigned char));
		l_onces->natural_8_count = a_natural_8_count;
	}
	if (a_natural_16_count > 0) {
		l_onces->natural_16_value = (EIF_NATURAL_16*)GE_calloc_atomic_uncollectable(a_natural_16_count, sizeof(EIF_NATURAL_16));
		l_onces->natural_16_exception = (EIF_REFERENCE*)GE_calloc_uncollectable(a_natural_16_count, sizeof(EIF_REFERENCE));
		l_onces->natural_16_status = (unsigned char*)GE_calloc_atomic_uncollectable(a_natural_16_count, sizeof(unsigned char));
		l_onces->natural_16_count = a_natural_16_count;
	}
	if (a_natural_32_count > 0) {
		l_onces->natural_32_value = (EIF_NATURAL_32*)GE_calloc_atomic_uncollectable(a_natural_32_count, sizeof(EIF_NATURAL_32));
		l_onces->natural_32_exception = (EIF_REFERENCE*)GE_calloc_uncollectable(a_natural_32_count, sizeof(EIF_REFERENCE));
		l_onces->natural_32_status = (unsigned char*)GE_calloc_atomic_uncollectable(a_natural_32_count, sizeof(unsigned char));
		l_onces->natural_32_count = a_natural_32_count;
	}
	if (a_natural_64_count > 0) {
		l_onces->natural_64_value = (EIF_NATURAL_64*)GE_calloc_atomic_uncollectable(a_natural_64_count, sizeof(EIF_NATURAL_64));
		l_onces->natural_64_exception = (EIF_REFERENCE*)GE_calloc_uncollectable(a_natural_64_count, sizeof(EIF_REFERENCE));
		l_onces->natural_64_status = (unsigned char*)GE_calloc_atomic_uncollectable(a_natural_64_count, sizeof(unsigned char));
		l_onces->natural_64_count = a_natural_64_count;
	}
	if (a_pointer_count > 0) {
		l_onces->pointer_value = (EIF_POINTER*)GE_calloc_atomic_uncollectable(a_pointer_count, sizeof(EIF_POINTER));
		l_onces->pointer_exception = (EIF_REFERENCE*)GE_calloc_uncollectable(a_pointer_count, sizeof(EIF_REFERENCE));
		l_onces->pointer_status = (unsigned char*)GE_calloc_atomic_uncollectable(a_pointer_count, sizeof(unsigned char));
		l_onces->pointer_count = a_pointer_count;
	}
	if (a_real_32_count > 0) {
		l_onces->real_32_value = (EIF_REAL_32*)GE_calloc_atomic_uncollectable(a_real_32_count, sizeof(EIF_REAL_32));
		l_onces->real_32_exception = (EIF_REFERENCE*)GE_calloc_uncollectable(a_real_32_count, sizeof(EIF_REFERENCE));
		l_onces->real_32_status = (unsigned char*)GE_calloc_atomic_uncollectable(a_real_32_count, sizeof(unsigned char));
		l_onces->real_32_count = a_real_32_count;
	}
	if (a_real_64_count > 0) {
		l_onces->real_64_value = (EIF_REAL_64*)GE_calloc_atomic_uncollectable(a_real_64_count, sizeof(EIF_REAL_64));
		l_onces->real_64_exception = (EIF_REFERENCE*)GE_calloc_uncollectable(a_real_64_count, sizeof(EIF_REFERENCE));
		l_onces->real_64_status = (unsigned char*)GE_calloc_atomic_uncollectable(a_real_64_count, sizeof(unsigned char));
		l_onces->real_64_count = a_real_64_count;
	}
	if (a_reference_count > 0) {
		l_onces->reference_value = (EIF_REFERENCE*)GE_calloc_uncollectable(a_reference_count, sizeof(EIF_REFERENCE));
		l_onces->reference_exception = (EIF_REFERENCE*)GE_calloc_uncollectable(a_reference_count, sizeof(EIF_REFERENCE));
		l_onces->reference_status = (unsigned char*)GE_calloc_atomic_uncollectable(a_reference_count, sizeof(unsigned char));
		l_onces->reference_count = a_reference_count;
	}
	if (a_procedure_count > 0) {
		l_onces->procedure_exception = (EIF_REFERENCE*)GE_calloc_uncollectable(a_procedure_count, sizeof(EIF_REFERENCE));
		l_onces->procedure_status = (unsigned char*)GE_calloc_atomic_uncollectable(a_procedure_count, sizeof(unsigned char));
		l_onces->procedure_count = a_procedure_count;
	}
	return l_onces;
}

/*
 * Free memory allocated by `a_onces'.
 */
void GE_free_onces(GE_onces* a_onces)
{
	if (a_onces->boolean_value) {
		GE_free(a_onces->boolean_value);
	}
	if (a_onces->boolean_exception) {
		GE_free(a_onces->boolean_exception);
	}
	if (a_onces->boolean_status) {
		GE_free(a_onces->boolean_status);
	}
	if (a_onces->character_8_value) {
		GE_free(a_onces->character_8_value);
	}
	if (a_onces->character_8_exception) {
		GE_free(a_onces->character_8_exception);
	}
	if (a_onces->character_8_status) {
		GE_free(a_onces->character_8_status);
	}
	if (a_onces->character_32_value) {
		GE_free(a_onces->character_32_value);
	}
	if (a_onces->character_32_exception) {
		GE_free(a_onces->character_32_exception);
	}
	if (a_onces->character_32_status) {
		GE_free(a_onces->character_32_status);
	}
	if (a_onces->integer_8_value) {
		GE_free(a_onces->integer_8_value);
	}
	if (a_onces->integer_8_exception) {
		GE_free(a_onces->integer_8_exception);
	}
	if (a_onces->integer_8_status) {
		GE_free(a_onces->integer_8_status);
	}
	if (a_onces->integer_16_value) {
		GE_free(a_onces->integer_16_value);
	}
	if (a_onces->integer_16_exception) {
		GE_free(a_onces->integer_16_exception);
	}
	if (a_onces->integer_16_status) {
		GE_free(a_onces->integer_16_status);
	}
	if (a_onces->integer_32_value) {
		GE_free(a_onces->integer_32_value);
	}
	if (a_onces->integer_32_exception) {
		GE_free(a_onces->integer_32_exception);
	}
	if (a_onces->integer_32_status) {
		GE_free(a_onces->integer_32_status);
	}
	if (a_onces->integer_64_value) {
		GE_free(a_onces->integer_64_value);
	}
	if (a_onces->integer_64_exception) {
		GE_free(a_onces->integer_64_exception);
	}
	if (a_onces->integer_64_status) {
		GE_free(a_onces->integer_64_status);
	}
	if (a_onces->natural_8_value) {
		GE_free(a_onces->natural_8_value);
	}
	if (a_onces->natural_8_exception) {
		GE_free(a_onces->natural_8_exception);
	}
	if (a_onces->natural_8_status) {
		GE_free(a_onces->natural_8_status);
	}
	if (a_onces->natural_16_value) {
		GE_free(a_onces->natural_16_value);
	}
	if (a_onces->natural_16_exception) {
		GE_free(a_onces->natural_16_exception);
	}
	if (a_onces->natural_16_status) {
		GE_free(a_onces->natural_16_status);
	}
	if (a_onces->natural_32_value) {
		GE_free(a_onces->natural_32_value);
	}
	if (a_onces->natural_32_exception) {
		GE_free(a_onces->natural_32_exception);
	}
	if (a_onces->natural_32_status) {
		GE_free(a_onces->natural_32_status);
	}
	if (a_onces->natural_64_value) {
		GE_free(a_onces->natural_64_value);
	}
	if (a_onces->natural_64_exception) {
		GE_free(a_onces->natural_64_exception);
	}
	if (a_onces->natural_64_status) {
		GE_free(a_onces->natural_64_status);
	}
	if (a_onces->pointer_value) {
		GE_free(a_onces->pointer_value);
	}
	if (a_onces->pointer_exception) {
		GE_free(a_onces->pointer_exception);
	}
	if (a_onces->pointer_status) {
		GE_free(a_onces->pointer_status);
	}
	if (a_onces->real_32_value) {
		GE_free(a_onces->real_32_value);
	}
	if (a_onces->real_32_exception) {
		GE_free(a_onces->real_32_exception);
	}
	if (a_onces->real_32_status) {
		GE_free(a_onces->real_32_status);
	}
	if (a_onces->real_64_value) {
		GE_free(a_onces->real_64_value);
	}
	if (a_onces->real_64_exception) {
		GE_free(a_onces->real_64_exception);
	}
	if (a_onces->real_64_status) {
		GE_free(a_onces->real_64_status);
	}
	if (a_onces->reference_value) {
		GE_free(a_onces->reference_value);
	}
	if (a_onces->reference_exception) {
		GE_free(a_onces->reference_exception);
	}
	if (a_onces->reference_status) {
		GE_free(a_onces->reference_status);
	}
	if (a_onces->procedure_exception) {
		GE_free(a_onces->procedure_exception);
	}
	if (a_onces->procedure_status) {
		GE_free(a_onces->procedure_status);
	}
	GE_free(a_onces);
}

#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"C functions used to implement class IDENTIFIED"

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2007-2018, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef GE_IDENTIFIED_C
#define GE_IDENTIFIED_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef GE_IDENTIFIED_H
#include "ge_identified.h"
#endif
#ifndef GE_GC_H
#include "ge_gc.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Weak pointers.
 */

/*
 * Weak pointer structure.
 */
typedef struct GE_weak_pointer {
	EIF_REFERENCE object;
} GE_weak_pointer;

/*
 * Create a new weak pointer to `object'.
 */
GE_weak_pointer* GE_new_weak_pointer(EIF_REFERENCE object)
{
	GE_weak_pointer* wp;

	if (object == 0) {
		return (GE_weak_pointer*)0;
	} else {
		wp = (GE_weak_pointer*)GE_malloc_atomic(sizeof(GE_weak_pointer));
		wp->object = object;
#ifdef GE_USE_BOEHM_GC
		GC_GENERAL_REGISTER_DISAPPEARING_LINK((void**)(&wp->object), GC_base(object));
#endif
		return wp;
	}
}

/*
 * Direct access to the object held in the weak pointer.
 */
EIF_REFERENCE GE_weak_pointer_object_without_lock(GE_weak_pointer* wp)
{
	if (wp == 0) {
		return (EIF_REFERENCE)0;
	} else {
		return (wp->object);
	}
}

/*
 * Access to the object held in the weak pointer.
 */
#ifdef GE_USE_BOEHM_GC
EIF_REFERENCE GE_weak_pointer_object(GE_weak_pointer* wp)
{
	return (EIF_REFERENCE)GC_call_with_alloc_lock((GC_fn_type)GE_weak_pointer_object_without_lock, wp);
}
#else /* No GC */
#define GE_weak_pointer_object(wp) GE_weak_pointer_object_without_lock(wp)
#endif


/*
 * Identified data structures and functions.
 */

/*
 * Size of inner arrays in `GE_id_objects'.
 */
#define GE_ID_OBJECT_CHUNK_SIZE 512

/*
 * Number of slots to be added to the outer array of
 * `GE_id_objects' when resizing it.
 */
#define GE_ID_OBJECT_CAPACITY_INCREMENT 512

/*
 * Array of arrays of weak pointers. Each inner array has the same size
 * (GE_ID_OBJECT_CHUNK_SIZE). Given an id, the weak pointer associated
 * the object, if any, is at index (id % GE_ID_OBJECT_CHUNK_SIZE) in the
 * inner array located at index (id / GE_ID_OBJECT_CHUNK_SIZE) in the
 * outer array.
 */
GE_weak_pointer*** GE_id_objects;

/*
 * Capacity of the outer array of `GE_id_objects'.
 */
EIF_INTEGER_32 GE_id_objects_capacity;

/*
 * Last id associated with an object.
 */
EIF_INTEGER_32 GE_last_object_id;

/*
 * Initialize data to keep track of object ids.
 */
void GE_init_identified(void)
{
	GE_id_objects = (GE_weak_pointer***)0;
	GE_id_objects_capacity = 0;
	GE_last_object_id = 0;
}

/*
 * Get a new id for `object', assuming it is NOT in the stack.
 */
EIF_INTEGER_32 GE_object_id(EIF_OBJECT object)
{
	GE_weak_pointer** id_object_chunk;
	EIF_INTEGER_32 i;
	EIF_INTEGER_32 l_old_capacity;

	GE_last_object_id++;
	i = GE_last_object_id / GE_ID_OBJECT_CHUNK_SIZE;
	if (i >= GE_id_objects_capacity) {
		l_old_capacity = GE_id_objects_capacity;
		GE_id_objects_capacity = GE_id_objects_capacity + GE_ID_OBJECT_CAPACITY_INCREMENT;
		if (GE_id_objects == 0) {
			GE_id_objects = GE_calloc_atomic_uncollectable(GE_id_objects_capacity, sizeof(GE_weak_pointer**));
		} else {
			GE_id_objects = GE_recalloc(GE_id_objects, l_old_capacity, GE_id_objects_capacity, sizeof(GE_weak_pointer**));
		}
	}
	id_object_chunk = GE_id_objects[i];
	if (id_object_chunk == 0) {
		id_object_chunk = GE_calloc_uncollectable(GE_ID_OBJECT_CHUNK_SIZE, sizeof(GE_weak_pointer*));
		GE_id_objects[i] = id_object_chunk;
	}
	i = GE_last_object_id % GE_ID_OBJECT_CHUNK_SIZE;
	id_object_chunk[i] = GE_new_weak_pointer(object);
	return GE_last_object_id;
}

/*
 * Return the object associated with `id'.
 */
EIF_REFERENCE GE_id_object(EIF_INTEGER_32 id)
{
	GE_weak_pointer** id_object_chunk;
	GE_weak_pointer* wp;
	EIF_REFERENCE object;
	EIF_INTEGER_32 i;

	i = id / GE_ID_OBJECT_CHUNK_SIZE;
	if (i >= GE_id_objects_capacity) {
		return (EIF_REFERENCE) 0;
	} else {
		id_object_chunk = GE_id_objects[i];
		if (id_object_chunk == 0) {
			return (EIF_REFERENCE)0;
		} else {
			i = id % GE_ID_OBJECT_CHUNK_SIZE;
			wp = id_object_chunk[i];
			if (wp == 0) {
				return (EIF_REFERENCE)0;
			} else {
				object = GE_weak_pointer_object(wp);
				if (object == 0) {
					id_object_chunk[i] = (GE_weak_pointer*)0;
#ifdef GE_USE_BOEHM_GC
					GC_unregister_disappearing_link((void**)(&wp->object));
#endif
					GE_free(wp);
					return (EIF_REFERENCE)0;
				} else {
					return object;
				}
			}
		}
	}
}

/*
 * Remove the object associated with `id' from the stack.
 */
void GE_object_id_free(EIF_INTEGER_32 id)
{
	GE_weak_pointer** id_object_chunk;
	GE_weak_pointer* wp;
	EIF_REFERENCE object;
	EIF_INTEGER_32 i;

	i = id / GE_ID_OBJECT_CHUNK_SIZE;
	if (i < GE_id_objects_capacity) {
		id_object_chunk = GE_id_objects[i];
		if (id_object_chunk != 0) {
			i = id % GE_ID_OBJECT_CHUNK_SIZE;
			wp = id_object_chunk[i];
			if (wp != 0) {
				object = GE_weak_pointer_object(wp);
				if (object != 0) {
#ifdef GE_USE_BOEHM_GC
					GC_unregister_disappearing_link((void**)(&wp->object));
#endif
				}
				id_object_chunk[i] = (GE_weak_pointer*)0;
				GE_free(wp);
			}
		}
	}
}

#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"Part of ISE Eiffel runtime. Needed to compile the EiffelCOM library."

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2010-2018, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef EIF_CECIL_C
#define EIF_CECIL_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef EIF_CECIL_H
#include "eif_cecil.h"
#endif
#ifndef GE_CONSOLE_H
#include "ge_console.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

EIF_PROCEDURE eif_procedure(char* rout, EIF_TYPE_ID cid)
{
	/* TODO */
	GE_show_console();
	fprintf(stderr, "'eif_procedure' in 'eif_cecil.c' not implemented\n");
	return (EIF_PROCEDURE)0;
}

EIF_INTEGER_FUNCTION eif_integer_function(char* rout, EIF_TYPE_ID cid)
{
	/* TODO */
	GE_show_console();
	fprintf(stderr, "'eif_integer_function' in 'eif_cecil.c' not implemented\n");
	return (EIF_INTEGER_FUNCTION)0;
}

EIF_REFERENCE_FUNCTION eif_reference_function(char* rout, EIF_TYPE_ID cid)
{
	/* TODO */
	GE_show_console();
	fprintf(stderr, "'eif_reference_function' in 'eif_cecil.c' not implemented\n");
	return (EIF_REFERENCE_FUNCTION)0;
}

EIF_POINTER_FUNCTION eif_pointer_function(char* rout, EIF_TYPE_ID cid)
{
	/* TODO */
	GE_show_console();
	fprintf(stderr, "'eif_pointer_function' in 'eif_cecil.c' not implemented\n");
	return (EIF_POINTER_FUNCTION)0;
}

EIF_BOOLEAN_FUNCTION eif_boolean_function(char* rout, EIF_TYPE_ID cid)
{
	/* TODO */
	GE_show_console();
	fprintf(stderr, "'eif_boolean_function' in 'eif_cecil.c' not implemented\n");
	return (EIF_BOOLEAN_FUNCTION)0;
}

EIF_REAL_32_FUNCTION eif_real_32_function(char* rout, EIF_TYPE_ID cid)
{
	/* TODO */
	GE_show_console();
	fprintf(stderr, "'eif_real_32_function' in 'eif_cecil.c' not implemented\n");
	return (EIF_REAL_32_FUNCTION)0;
}

EIF_REAL_64_FUNCTION eif_real_64_function(char* rout, EIF_TYPE_ID cid)
{
	/* TODO */
	GE_show_console();
	fprintf(stderr, "'eif_real_64_function' in 'eif_cecil.c' not implemented\n");
	return (EIF_REAL_64_FUNCTION)0;
}

EIF_CHARACTER_8_FUNCTION eif_character_8_function(char* rout, EIF_TYPE_ID cid)
{
	/* TODO */
	GE_show_console();
	fprintf(stderr, "'eif_character_8_function' in 'eif_cecil.c' not implemented\n");
	return (EIF_CHARACTER_8_FUNCTION)0;
}

EIF_OBJECT eif_create(EIF_TYPE_ID cid)
{
	/* TODO */
	GE_show_console();
	fprintf(stderr, "'eif_create' in 'eif_cecil.c' not implemented\n");
	return (EIF_OBJECT)0;
}

EIF_TYPE_ID eif_type_id(char* type_string)
{
	GE_show_console();
	fprintf(stderr, "'eif_type_id' in 'eif_cecil.c' not implemented\n");
	return EIF_NO_TYPE;
}

extern EIF_INTEGER eifaddr_offset(EIF_REFERENCE object, char *name, int * const ret)
{
	GE_show_console();
	fprintf(stderr, "'eifaddr_offset' in 'eif_cecil.c' not implemented\n");
	return 0;
}

EIF_TYPE_ID eiftype(EIF_OBJECT object)
{
		/* Obsolete. Use "eif_type_by_reference" instead.
		 * Return the Type id of the specified object.
		 */
	return (EIF_TYPE_ID)object->id;
}

EIF_TYPE_ID eif_type_by_reference(EIF_REFERENCE object)
{
		/* Return type id of the direct reference "object" */
	return (EIF_TYPE_ID)object->id;
}

EIF_TYPE_INDEX eif_gen_param_id(EIF_TYPE_INDEX dftype, uint32_t pos)
{
	GE_show_console();
	fprintf(stderr, "'eif_gen_param_id' in 'eif_cecil.c' not implemented\n");
	return 0;
}

#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"Part of ISE Eiffel runtime. Needed to compile the EiffelCOM library."

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2010-2017, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef EIF_PLUG_C
#define EIF_PLUG_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef EIF_PLUG_H
#include "eif_plug.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

int nstcall = 0;

#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"C functions used to implement class EXCEPTIONS"

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2007-2018, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef EIF_EXCEPT_C
#define EIF_EXCEPT_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef EIF_EXCEPT_H
#include "eif_except.h"
#endif
#ifndef GE_STRING_H
#include "ge_string.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Raise an Eiffel exception.
 */
void eraise(const char* name, long code)
{
	GE_raise_with_message(code, name);
}

/*
 * Raise an Eiffel exception of the given code with no associated tag.
 */
void xraise(int code)
{
	GE_raise((long)code);
}

/*
 * Raise an "Operating system error" exception.
 */
void esys(void)
{
	if (errno == 0) {
			/* External event */
		GE_raise(GE_EX_EXT);
	} else {
			/* Operating system error */
		GE_raise(GE_EX_SYS);
	}
}

/*
 * As a special case, an I/O error is raised when a system call which is I/O bound fails.
 */
void eise_io(const char* tag)
{
	GE_raise_with_message(GE_EX_ISE_IO, tag);
}

/*
 * Raise a "No more memory" exception.
 */
 void enomem(void) {
	GE_raise(GE_EX_OMEM);
 }

/*
 * Raise EiffelCOM exception.
 */
void com_eraise(const char* tag, long num)
{
	GE_raise_with_message(num, tag);
}

/*
 * Terminate execution with exit status `code',
 * without triggering an exception.
 */
void esdie(int code)
{
/* TODO: check that the Boehm GC triggers the 'dispose' routines. */
	exit(code);
}

/*
 * Exception tag associated with `code'.
 * This is a duplication from Eiffel classes, but still used for trace printing and in EiffelCom.
 */
EIF_REFERENCE eename(long code)
{
	return GE_str(GE_exception_tag(code));
}

/*
 * Is exception `ex' defined?
 * Used in EiffelCOM.
 */
char eedefined(long ex)
{
	return (char)((ex > 0 && ex <= GE_EX_NEX)? 1 : 0);
}

/*
 * Enable/diable printing of the exception trace.
 * Per thead information.
 */
void eetrace(char b)
{
	GE_context* context;

	context = GE_current_context();
	context->exception_trace_enabled = b;
}

/*
 * Is current execution during rescue?
 */
EIF_BOOLEAN eif_is_in_rescue(void)
{
	GE_context* context;

	context = GE_current_context();
	return (EIF_TEST(context->in_rescue > 0));
}

#ifdef EIF_WINDOWS
/*
 * Set default exception handler.
 */
void set_windows_exception_filter(void)
{
	GE_set_windows_exception_filter();
}
#endif

#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"C functions used to implement class REAL"

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2013-2017, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef GE_REAL_C
#define GE_REAL_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef GE_REAL_H
#include "ge_real.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Bit representation of `v'.
 */
EIF_NATURAL_32 GE_real_32_to_bits(EIF_REAL_32 v)
{
	return *(EIF_NATURAL_32*)&v;
}

/*
 * Bit representation of `v'.
 */
EIF_NATURAL_64 GE_real_64_to_bits(EIF_REAL_64 v)
{
	return *(EIF_NATURAL_64*)&v;
}

/*
 * Real value with `v' as bit representation.
 */
EIF_REAL_32 GE_real_32_from_bits(EIF_NATURAL_32 v)
{
	return *(EIF_REAL_32*)&v;
}

/*
 * Real value with `v' as bit representation.
 */
EIF_REAL_64 GE_real_64_from_bits(EIF_NATURAL_64 v)
{
	return *(EIF_REAL_64*)&v;
}

#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"C functions used to implement class FILE"

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2006-2024, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef EIF_FILE_C
#define EIF_FILE_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef EIF_FILE_H
#include "eif_file.h"
#endif
#ifndef EIF_EXCEPT_H
#include "eif_except.h"
#endif
#ifndef GE_STRING_H
#include "ge_string.h"
#endif
#ifndef GE_REAL_H
#include "ge_real.h"
#endif

/* HAS_CHOWN:
 *	This symbol, if defined, indicates that the chown routine is
 *	available.
 */
#ifndef EIF_WINDOWS
#define HAS_CHOWN
#endif

/* HAS_GETEUID:
 *	This symbol, if defined, indicates that the geteuid system call is
 *	available
 */
#ifndef EIF_WINDOWS
#define  HAS_GETEUID
#endif

/* HAS_GETGRGID:
 *	This symbol, if defined, indicates that the getgrgid system call is
 *	available
 */
#ifndef EIF_WINDOWS
#define  HAS_GETGRGID
#endif

/* HAS_GETGROUPS:
 *	This symbol, if defined, indicates that the getgroups() routine is
 *	available to get the list of process groups.  If unavailable, multiple
 *	groups are probably not supported.
 */
#ifndef EIF_WINDOWS
#define HAS_GETGROUPS
#define Groups_t gid_t	/* Type for 2nd arg to getgroups() */
#endif

/* HAS_GETPWUID:
 *	This symbol, if defined, indicates that the getpwuid system call is
 *	available
 */
#ifndef EIF_WINDOWS
#define  HAS_GETPWUID
#endif

/* HAS_LINK:
 *	This symbol, if defined, indicates that the link routine is
 *	available to create hard links.
 */
#ifndef EIF_WINDOWS
#define HAS_LINK
#endif

/* HAS_LSTAT:
 *	This symbol, if defined, indicates that the lstat routine is
 *	available to do file stats on symbolic links.
 */
#ifndef EIF_WINDOWS
#define HAS_LSTAT
#endif


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef EIF_WINDOWS
#if defined(__BORLANDC__) && (__BORLANDC__ < 0x600) /* Borland before 6.0 */
#include <utime.h>
#else
#include <sys/utime.h>
#ifdef __LCC__
extern int utime(const char *, struct utimbuf *);
#include <sys/stat.h>
#endif
#endif
#include <io.h> /* for access, chmod */
#include <direct.h> /* for (ch|mk|rm)dir */
#include <share.h>  /* added for temporary file */
#include <fcntl.h>  /* added for temporary file */
#include <wchar.h>
#else
#include <utime.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#endif

#ifndef NAME_MAX
#define NAME_MAX 10 /* Maximum length for user/group name */
#endif

#ifndef FILE_TYPE_MAX
#define FILE_TYPE_MAX 5		/* max size of fopen type string (like "a+b") */
#endif

#ifndef S_IRUSR
#define S_IRUSR 0000400
#endif
#ifndef S_IWUSR
#define S_IWUSR 0000200
#endif
#ifndef S_IXUSR
#define S_IXUSR 0000100
#endif
#ifndef S_IRGRP
#define S_IRGRP 0000040
#endif
#ifndef S_IWGRP
#define S_IWGRP 0000020
#endif
#ifndef S_IXGRP
#define S_IXGRP 0000010
#endif
#ifndef S_IROTH
#define S_IROTH 0000004
#endif
#ifndef S_IWOTH
#define S_IWOTH 0000002
#endif
#ifndef S_IXOTH
#define S_IXOTH 0000001
#endif
#ifndef S_IFDIR
#define S_IFDIR 0040000
#endif
#ifndef S_IFCHR
#define S_IFCHR 0020000
#endif
#ifdef __LCC__
#ifndef _S_IFCHR
#define _S_IFCHR S_IFCHR
#endif
#endif
#ifndef S_IFBLK
#define S_IFBLK 0060000
#endif
#ifndef S_IFREG
#define S_IFREG 0100000
#endif
#ifndef S_IFIFO
#define S_IFIFO 0010000
#endif
#ifndef S_IFLNK
#define S_IFLNK 0120000
#endif
#ifndef S_IFSOCK
#define S_IFSOCK 0140000
#endif
#ifndef S_IFMT
#define S_IFMT 0170000
#endif
#ifndef S_ISREG
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISCHR
#define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
#endif
#ifndef S_ISBLK
#define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#endif
#ifndef S_ISLNK
#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#endif
#ifndef S_ISFIFO
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#endif
#ifndef S_ISSOCK
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
#endif

/* The following access constants seems to be universally accepeted. And
 * anyway, it's only after BSD 4.1 that the symbolic constants appeared.
 */
#ifndef R_OK
#define R_OK 4
#define W_OK 2
#define X_OK 1
#define F_OK 0
#endif

#define FS_START	0			/* Beginning of file for `fseek' */
#define FS_CUR		1			/* Current position for `fseek' */
#define FS_END		2			/* End of file for `fseek' */
/* Keep only permission mode */
#ifndef ST_MODE
#define ST_MODE 0x0fff
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef EIF_WINDOWS
#	ifdef __LCC__
#		define rt_stat		_wstat
#		define rt_fstat		fstat
#	elif defined(EIF_64_BITS)
#		define rt_stat		_wstat64
#		define rt_fstat		_fstat64
#	else
#		define rt_stat		_wstat64i32
#		define rt_fstat		_fstat64i32
#	endif
#	define rt_access		_waccess
#else
#	define rt_stat			stat
#	define rt_lstat			lstat
#	define rt_fstat			fstat
#	define rt_access		access
#endif

static int rt_mkdir(EIF_FILENAME path, int mode)
{
#ifdef EIF_WINDOWS
	return _wmkdir(path);
#else
	return mkdir(path, mode);
#endif
}

static int rt_rename(EIF_FILENAME from, EIF_FILENAME to)
{
#ifdef EIF_WINDOWS
	return _wrename(from, to);
#else
	return rename(from, to);
#endif
}

static int rt_rmdir(EIF_FILENAME path)
{
#ifdef EIF_WINDOWS
	return _wrmdir(path);
#else
	return rmdir(path);
#endif
}

static int rt_unlink(EIF_FILENAME path)
{
#ifdef EIF_WINDOWS
	return _wunlink(path);
#else
	return unlink(path);
#endif
}

static FILE* rt_file_fopen(EIF_FILENAME name, EIF_FILENAME type)
{
#ifdef EIF_WINDOWS
	return _wfopen(name, type);
#else
	return fopen(name, type);
#endif
}

static FILE* rt_file_fdopen(int fd, EIF_FILENAME type)
{
#ifdef EIF_WINDOWS
	return _wfdopen(fd, type);
#else
	return fdopen(fd, type);
#endif
}

static FILE* rt_file_freopen(EIF_FILENAME name, EIF_FILENAME type, FILE* stream)
{
#ifdef EIF_WINDOWS
	return _wfreopen(name, type, stream);
#else
	return freopen(name, type, stream);
#endif
}

static int rt_utime(EIF_FILENAME path, struct utimbuf* times)
{
#ifdef EIF_WINDOWS
	return _wutime(path, (struct _utimbuf*)times);
#else
	return utime(path, times);
#endif
}

/*
 * Swallow next character if it is a new line.
 */
static void rt_swallow_nl(FILE* f)
{
		/* getc() cannot be used as it doesn't set the EOF flag */

	if (f != stdin) {
		if (fscanf(f, "\n") == EOF && ferror(f)) {
			eise_io("FILE: error during reading the end of the file,");
		}
	} else {
		int c;

		errno = 0;
		c = getc(f);
		if (c == EOF && ferror(f))
			eise_io("FILE: error during reading the end of the file.");

		if (c != '\n' && EOF == ungetc(c, f))
			eise_io("FILE: End of file.");
	}
}

static void rt_file_stat(EIF_FILENAME path, rt_stat_buf* buf)
{
		/* To preserve compatibility we always follow symbolic links and raise an exception upon failure. */
	if (eif_file_stat(path, buf, 1) == -1) {
		esys();
	}
}

/*
 * Set `file_type', given `how' the file is being opened and a `mode' create a platform specific character
 * string (UTF-16 encoding on Windows and a byte sequence otherwise) that can be used by
 * the underlying OS system call to open the mode in the proper mode (e.g. open append,
 * read-write mode, ....).
 */
static void rt_file_set_open_mode(EIF_NATIVE_CHAR file_type[FILE_TYPE_MAX], int how, char mode)
{
	file_type[4] = '\0';
	file_type[3] = '\0';
	file_type[2] = '\0';
	if (how >= 10) how -= 10;
	switch (how) {
		case 0:
		case 3: file_type[0] = 'r'; break;
		case 1:
		case 4: file_type[0] = 'w'; break;
		case 2:
		case 5: file_type[0] = 'a'; break;
		default: file_type[0] = 'r'; break;
	}
	file_type[1] = mode;
	switch (how) {
		case 3:
		case 4:
		case 5:
			if (mode == '\0') {
				file_type[1] = '+';
			} else {
				file_type[2] = '+';
			}
	}
#ifdef EIF_WINDOWS
	/* We make sure that files created in Eiffel are not inheritable
	 * by default as otherwise it makes things too complicated for the
	 * end user when spawing child processes. */
	if (file_type[1] == '\0') {
		file_type[1] = 'N';
	} else if (file_type[2] == '\0') {
		file_type[2] = 'N';
	} else {
		file_type[3] = 'N';
	}
#endif
}

/*
 * Create directory `dirname'.
 */
void eif_file_mkdir(EIF_FILENAME dirname)
{
	errno = 0;
	if (rt_mkdir(dirname, 0777) == -1) {
		esys(); /* Raise exception */
	}
}

/*
 * Rename file `from' into `to'.
 */
void eif_file_rename(EIF_FILENAME from, EIF_FILENAME to)
{
	for (;;) {
#ifdef EIF_WINDOWS
		if (eif_file_exists(to)) {
				/* To have the same behavior as Unix, we need to remove the destination file if it exists.
				 * Of course we can do this only if `from' and `to' do not represent the same file.
				 * To check this, we use `CreateFile' to open both file, and then using the information
				 * returned by `GetFileInformationByHandle' we can check whether or not they are indeed
				 * the same. */
			BY_HANDLE_FILE_INFORMATION l_to_info, l_from_info;
			HANDLE l_from_file = CreateFileW(from, GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			HANDLE l_to_file = CreateFileW(to, GENERIC_READ, FILE_SHARE_READ, NULL,
					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

			if ((l_from_file == INVALID_HANDLE_VALUE) || (l_to_file == INVALID_HANDLE_VALUE)) {
					/* We do not need the handles anymore, simply close them. Since Microsoft
					 * API accepts INVALID_HANDLE_VALUE we don't check the validity of arguments. */
				CloseHandle(l_from_file);
				CloseHandle(l_to_file);

					/* For some reasons we cannot open the file. This should not happen, maybe the OS has
					 * removed `from' or `to'. In that case, we simply try to remove destination as we were
					 * doing in former revision of `file_rename'. */
				_wremove(to);
			} else {
				BOOL success = GetFileInformationByHandle(l_from_file, &l_from_info);
				if (success) {
					success = GetFileInformationByHandle(l_to_file, &l_to_info);
						/* We do not need the handles anymore, simply close them. */
					CloseHandle(l_from_file);
					CloseHandle(l_to_file);
					if (success) {
							/* Check that `from' and `to' do not represent the same file. */
						if
							((l_from_info.dwVolumeSerialNumber != l_to_info.dwVolumeSerialNumber) ||
							(l_from_info.nFileIndexLow != l_to_info.nFileIndexLow) ||
							(l_from_info.nFileIndexHigh != l_to_info.nFileIndexHigh))
						{
							_wremove(to);
						} else {
								/* Files are identical, nothing to be done apart from */
							break;
						}
					} else {
							/* An error occurred while retrieving the information about `from' and `to'. Like
							 * for the case where `l_from_file' and `l_to_file' are invalid, we try to remove
							 * the file. */
						_wremove(to);
					}
				} else {
						/* We do not need the handles anymore, simply close them. */
					CloseHandle(l_from_file);
					CloseHandle(l_to_file);
						/* An error occurred while retrieving the information about `from' and `to'. Like
						 * for the case where `l_from_file' and `l_to_file' are invalid, we try to remove
						 * the file. */
					_wremove(to);
				}
			}
		}
#endif
		errno = 0;
		if (rt_rename(from, to) == -1) {
			esys(); /* Raise exception */
		}
		break;
	}
}

/*
 * Link file `from' into `to'.
 */
void eif_file_link(EIF_FILENAME from, EIF_FILENAME to)
{
#ifdef HAS_LINK
	errno = 0;
	if (link(from, to) == -1) {
		esys(); /* Raise exception */
	}
#endif
}

/*
 * Delete file or directory `name'.
 */
void eif_file_unlink(EIF_FILENAME name)
{
	rt_stat_buf buf;				/* File statistics */
	int status;						/* Status from system call */

	status = eif_file_stat(name, &buf, 0);				/* Side effect: ensure file exists */
	if (status == -1) {				/* An error occurred */
		esys();
	} else {
		for (;;) {
			errno = 0;						/* Reset error condition */
			if (S_ISDIR(buf.st_mode))		/* Directory */
				status = rt_rmdir(name);		/* Remove only if empty */
			else
				status = rt_unlink(name);		/* Not a directory */
			if (status == -1) {				/* An error occurred */
				if (errno == EINTR)			/* Interrupted by signal */
					continue;				/* Re-issue system call */
				else
					esys();					/* Raise exception */
			}
			break;
		}
	}
}

/*
 * Open file `name' with the corresponding type `how'.
 */
EIF_POINTER eif_file_open(EIF_FILENAME name, int how)
{
	FILE* fp;
	EIF_NATIVE_CHAR l_mode[FILE_TYPE_MAX];
	errno = 0;
#if defined EIF_WINDOWS
	rt_file_set_open_mode(l_mode, how, (how < 10 ? 't' : 'b'));
#else
	rt_file_set_open_mode(l_mode, how, '\0');
#endif
	fp = (FILE*)rt_file_fopen(name, l_mode);
	if (fp == (FILE*)0) {
		esys(); /* Open failed, raise exception */
	}
	return (EIF_POINTER)fp;
}

/*
 * Open file `fd' with the corresponding type `how'.
 */
EIF_POINTER eif_file_dopen(int fd, int how)
{
	FILE* fp;
	EIF_NATIVE_CHAR l_mode[FILE_TYPE_MAX];
	errno = 0;
#ifdef EIF_WINDOWS
	rt_file_set_open_mode(l_mode, how, (how < 10 ? 't' : 'b'));
#else
	rt_file_set_open_mode(l_mode, how, '\0');
#endif
	fp = (FILE*)rt_file_fdopen(fd, l_mode);
	if (fp == (FILE*)0) {
		esys(); /* Open failed, raise exception */
	}
	return (EIF_POINTER)fp;
}

/*
 * Reopen file `name' with the corresponding type `how' and substitute that
 * to the old stream described by `old'. This is useful to redirect 'stdout'
 * to another place, for instance.
 */
EIF_POINTER eif_file_reopen(EIF_FILENAME name, int how, FILE *old)
{
	FILE* fp;
	EIF_NATIVE_CHAR l_mode[FILE_TYPE_MAX];
	errno = 0;
#ifdef EIF_WINDOWS
	rt_file_set_open_mode(l_mode, how, (how < 10 ? 't' : 'b'));
#else
	rt_file_set_open_mode(l_mode, how, '\0');
#endif
	fp = (FILE*)rt_file_freopen(name, l_mode, old);
	if (fp == (FILE*)0) {
		esys(); /* Open failed, raise exception */
	}
	return (EIF_POINTER)fp;
}

/*
 * Close the file.
 */
void eif_file_close(FILE* fp)
{
	errno = 0;
	if (fclose(fp) != 0) {
		esys(); /* Close failed, raise exception */
	}
}

/*
 * Flush data held in stdio buffer.
 */
void eif_file_flush(FILE* fp)
{
	errno = 0;
	if (fflush(fp) != 0) {
	    esys(); /* Flush failed, raise exception */
	}
#ifdef EIF_WINDOWS
		/* On Windows, it does not write directly to disk, so we have to force it. See KB66052:
		 * http://support.microsoft.com/kb/66052
		 * We ignore bad file descriptor case, as it is most likely when calling it on one of the standard
		 * input/outputs. */
#ifdef __LCC__
extern int _commit(int);
#endif
	if ((0 != _commit(fileno(fp))) && (errno != EBADF)) {
		esys();
	}
#endif
}

/*
 * Return the associated file descriptor.
 */
EIF_INTEGER eif_file_fd(FILE* f)
{
	int res;
	if (!f) {
		res = 0;
		eraise("invalid file pointer", EN_EXT);
	} else {
		res = fileno(f);
		if (res == -1) {
			eraise("error occurred", EN_EXT);
		}
	}
	return (EIF_INTEGER)res;
}

/*
 * Get a character from `f'.
 */
EIF_CHARACTER_8 eif_file_gc(FILE* f)
{
	int c;
	errno = 0;
	c = getc(f);
	if (c == EOF && ferror(f)) {
		eise_io("FILE: unable to read CHARACTER value.");
	}
	return (EIF_CHARACTER_8)c;
}

/*
 * Get a string from `f' and fill it into `s' (at most `bound' characters),
 * with `start' being the amount of bytes already stored within s. This
 * means we really have to read (bound - start) characters.
 */
EIF_INTEGER eif_file_gs(FILE* f, char* s, EIF_INTEGER bound, EIF_INTEGER start)
{
	EIF_INTEGER amount;
	int c = '\0';
	EIF_INTEGER read;

	amount = bound - start;
	s += start;
	errno = 0;
	read = 0;
	while (amount-- > 0) {
		if ((c = getc(f)) == '\n' || c == EOF)
			break;
		*s++ = c;
		read++;
	}
	if (c == EOF && ferror(f)) {
			/* An I/O error occurred */
			/* Raise exception */
		eise_io("FILE: unable to read current line.");
	}
	if (c == EOF || c == '\n') {
#ifdef EIF_WINDOWS
		if ((read > 0) && (*(s-1) == '\r')) {
			return read - 1;
		}
#endif
		return read;
	}
	if (amount == -1)
		return (read + 1);
	return bound - start + 1;
}

/*
 * Read min (bound, remaining bytes in file) characters into `s' and
 * return the number of characters read.
 */
EIF_INTEGER eif_file_gss(FILE* f, char* s, EIF_INTEGER bound)
{
	size_t amount = fread(s, sizeof(char), (size_t)bound, f);

	if (ferror(f)) {	/* An I/O error occurred */
		eise_io("FILE: unable to read stream.");					/* Raise exception */
	}

	return (EIF_INTEGER)amount;	/* Number of characters read */
}

/*
 * Get a word from `f' and fill it into `s' (at most `bound' characters),
 * with `start' being the amount of bytes already stored within s. This
 * means we really have to read (bound - start) characters. Any leading
 * spaces are skipped.
 */
EIF_INTEGER eif_file_gw(FILE* f, char* s, EIF_INTEGER bound, EIF_INTEGER start)
{
	EIF_INTEGER amount;	/* Amount of bytes to be read */
	int c = 0;			/* Last char read */

	amount = bound - start;		/* Characters to be read */
	s += start;					/* Where read characters are written */
	errno = 0;					/* No error, a priori */

	if (start == 0)	{			/* First call */
		while ((c = getc(f)) != EOF)
			if (!isspace(c))
				break;
		if (c == EOF && ferror(f))	/* An I/O error occurred */
			eise_io("FILE: unable to read word.");					/* Raise exception */
		if (c == EOF)
			return (EIF_INTEGER)0;				/* Reached EOF before word */
		else if (EOF == ungetc(c, f))
			eise_io("FILE: unable to read word.");
	}

	while (amount-- > 0) {
		c = getc(f);
		if (c == EOF)
			break;
		if (isspace(c)) {
			if (EOF == ungetc(c, f))
				eise_io("FILE: unable to read word.");
			break;
		}
		*s++ = (char)c;
	}

	if (c == EOF && ferror(f))	/* An I/O error occurred */
		eise_io("FILE: unable to read word.");					/* Raise exception */

	/* If we managed to get the whole string, return the number of characters
	 * read. Otherwise, return (bound - start + 1) to indicate an error
	 * condition.
	 */

	if (c == EOF || isspace(c))
		return bound - start - amount - 1;	/* Number of characters read */

	return bound - start + 1;			/* Error condition */
}

/*
 * Look ahead one character. If EOF, return 0.
 */
EIF_CHARACTER_8 eif_file_lh(FILE* f)
{
	int c;

	errno = 0;
	c = getc(f);
	if (c == EOF && ferror(f))
		eise_io("FILE: error when reading a character ahead.");

	if (c != EOF && EOF == ungetc(c, f))
		eise_io("FILE: error when reading a character ahead.");

	return (EIF_CHARACTER_8)(c == EOF ? (char)0 : (char)c);
}

/*
 * Size of file `fp'.
 */
EIF_INTEGER eif_file_size(FILE* fp)
{
	rt_stat_buf buf;

	errno = 0;
	if (fflush(fp) != 0) {
		esys();
	}
	if (rt_fstat(fileno(fp), &buf) == -1)
		esys();		/* An error occurred: raise exception */
		/* FIXME: This code should be upgraded to use 64-bit */
	return (EIF_INTEGER)buf.st_size;
}

/*
 * Read upto next input line.
 */
void eif_file_tnil(FILE* f)
{
	int c;

	errno = 0;
	while ((c = getc(f)) != '\n' && c != EOF)
		;
	if (c == EOF && ferror(f))
		eise_io("FILE: error during reading the end of the file.");
}

/*
 * Current position within file.
 */
EIF_INTEGER eif_file_tell(FILE* f)
{
	long res;

	if (f == (FILE*)0) {
		eraise("invalid file pointer", EN_EXT);
	}

	res = ftell(f);
	if (res == -1) {
		eraise("error occurred", EN_EXT);
	}
	return (EIF_INTEGER)res;
}

/*
 * Touch file `name' by setting both access and modification time to the
 * current time stamp. This external function exists only because there
 * is no way within UNIX_FILE to get the current time stamp. Otherwise,
 * we could simply call file_utime.
 */
void eif_file_touch(EIF_FILENAME name)
{
	eif_file_utime(name, time((time_t*) 0), 2);
}

/*
 * Modify the modification and/or the access time stored in the file's
 * inode. The 'how' parameter tells which attributes should be set.
 */
void eif_file_utime(EIF_FILENAME name, time_t stamp, int how)
{
	struct utimbuf tp;	/* Time array */
	rt_stat_buf buf;	/* File statistics */
	int status;			/* System call status */

	if (how < 2) {				/* Need to fetch time from inode */
		rt_file_stat(name, &buf);
		switch (how) {
		case 0:					/* Change only access time */
			tp.actime = stamp;
			tp.modtime = buf.st_mtime;
			break;
		case 1:					/* Change only modification time */
			tp.actime = buf.st_atime;
			tp.modtime = stamp;
			break;
		}
	} else
		tp.actime = tp.modtime = stamp;	/* Change both access and modification times */
	errno = 0;

	if (rt_utime(name, &tp) == -1) {
			/* Raise exception */
		esys();
	}
}

/*
 * This is an encapsulation of the stat() system call. The routine either
 * succeeds and returns or fails and raises the appropriate exception.
 */
int eif_file_stat(EIF_FILENAME path, rt_stat_buf* buf, int follow)
{
	int status;			/* System call status */

	for (;;) {
		errno = 0;						/* Reset error condition */
#ifdef HAS_LSTAT
		status = rt_lstat(path, buf);
		if ((status == 0) && (follow) && (S_ISLNK(buf->st_mode))) {
				/* We found a file which is a symbolic link and we are asked to
				 * follow the link to fetch properties on the link location.
				 * We call `rt_stat' to make sure the link is valid. It is going to
				 * slow down current call by stating twice the info, but this
				 * case is quite rare and there is a benefit in using `lstat'
				 * over `rt_stat' the first time as more than 90% of the files
				 * we stat are not symlink. */
			status = rt_stat(path, buf);
		}
#else
		status = rt_stat(path, buf);		/* Get file statistics */
#endif
		if ((status == -1) && (errno == EINTR)) {
				/* Call was interrupted by a signal we re-issue it. */
			continue;
		}
		break;
	}
	return status;
}

/*
 * Change permissions of file `name', using an interface like chmod(1).
 * The flag is true if permissions are to be added, 0 to remove them.
 */
void eif_file_perm(EIF_FILENAME name, char* who, char* what, int flag)
{
	int fmode;					/* File mode to be altered */
	rt_stat_buf buf;			/* File statistics */

		/* We need to follow links since `chmod' does follow them to change the permissions. */
	if (eif_file_stat(name, &buf, 1)) {
		esys();
	} else {
		fmode = (int)buf.st_mode;	/* Fetch current file mode */

		switch (*who) {
		case 'u':
			while (*what)
				switch (*what++) {
#ifdef EIF_WINDOWS
				case 's': break;
				case 'r': break;
				case 'w': break;
				case 'x': break;
#else
				case 's':
					if (flag) fmode |= S_ISUID; else fmode &= ~S_ISUID;
					break;
				case 'r':
					if (flag) fmode |= S_IRUSR; else fmode &= ~S_IRUSR;
					break;
				case 'w':
					if (flag) fmode |= S_IWUSR; else fmode &= ~S_IWUSR;
					break;
				case 'x':
					if (flag) fmode |= S_IXUSR; else fmode &= ~S_IXUSR;
					break;
#endif
				default:
					eraise("invalid user permission", EN_EXT);
				}
			break;
		case 'g':
			while (*what)
				switch (*what++) {
				case 's':
#ifdef S_ISGID
					if (flag) fmode |= S_ISGID; else fmode &= ~S_ISGID;
#endif
					break;
				case 'r':
#ifdef S_IRGRP
					if (flag) fmode |= S_IRGRP; else fmode &= ~S_IRGRP;
#endif
					break;
				case 'w':
#ifdef S_IWGRP
					if (flag) fmode |= S_IWGRP; else fmode &= ~S_IWGRP;
#endif
					break;
				case 'x':
#ifdef S_IXGRP
					if (flag) fmode |= S_IXGRP; else fmode &= ~S_IXGRP;
#endif
					break;
				default:
					eraise("invalid group permission", EN_EXT);
				}
			break;
		case 'o':
			while (*what)
				switch (*what++) {
				case 't':
#ifdef S_ISVTX
					if (flag) fmode |= S_ISVTX; else fmode &= ~S_ISVTX;
#endif
					break;
				case 'r':
#ifdef S_IROTH
					if (flag) fmode |= S_IROTH; else fmode &= ~S_IROTH;
#endif
					break;
				case 'w':
#ifdef S_IWOTH
					if (flag) fmode |= S_IWOTH; else fmode &= ~S_IWOTH;
#endif
					break;
				case 'x':
#ifdef S_IXOTH
					if (flag) fmode |= S_IXOTH; else fmode &= ~S_IXOTH;
#endif
					break;
				default:
					eraise("invalid other permission", EN_EXT);
				}
			break;
		default:
			eraise("invalid permission target", EN_EXT);
		}
		eif_file_chmod(name, fmode);
	}
}

/*
 * Change permission mode on file `path'.
 */
void eif_file_chmod(EIF_FILENAME path, int mode)
{
	errno = 0;
#ifdef EIF_WINDOWS
	if (_wchmod(path, mode) == -1) {
#else
	if (chmod(path, mode) == -1) {
#endif
		esys();
	}
}

/*
 * Change the owner of the file to `uid'.
 */
void eif_file_chown(EIF_FILENAME name, int uid)
{
#ifdef HAS_CHOWN
	int gid;
	rt_stat_buf buf;

	rt_file_stat(name, &buf);
	gid = buf.st_gid;
	errno = 0;
	if (chown(name, uid, gid) == -1) {
		esys();
	}
#endif
}

/*
 * Change the group of the file to `gid'.
 */
void eif_file_chgrp(EIF_FILENAME name, int gid)
{
#ifdef HAS_CHOWN
	int uid;
	rt_stat_buf buf;

	rt_file_stat(name, &buf);
	uid = buf.st_uid;
	errno = 0;
	if (chown(name, uid, gid) == -1) {
		esys();
	}
#endif
}

/*
 * Put new_line onto `f'.
 */
void eif_file_tnwl(FILE *f)
{
	errno = 0;
	if (putc('\n', f) == EOF) {
		eise_io("FILE: unable to write new line.");
	}
}

/*
 * Append a copy of `other' to `f'.
 */
void eif_file_append(FILE* f, FILE* other, EIF_INTEGER l)
{
	size_t amount;
	char buffer[512];
	int bufsize = 512;

	errno = 0;
	if (fseek(other, 0, FS_START) != 0) {
		esys();
	}
	if (fseek(f, 0, FS_END) != 0) {
		esys();
	}
	while (l > 0) {
		amount = l;
		if (l < bufsize)
			amount = l;
		else
			amount = bufsize;
		if (amount != fread(buffer, sizeof(char), amount, other)) {
			eise_io("FILE: unable to read appended file.");
			break;
		}
		l -= (EIF_INTEGER)amount;
		if (amount != fwrite(buffer, sizeof(char), amount, f)) {
			eise_io("FILE: unable to write appended file.");
			break;
		}
	}
}

/*
 * Write string `str' on `f'.
 */
void eif_file_ps(FILE* f, char* str, EIF_INTEGER len)
{
	errno = 0;
	if (len == 0) {
		/* Nothing to be done. */
	} else if (fwrite(str, sizeof(char) * len, 1, f) != 1) {
		eise_io("FILE: unable to write STRING object.");
	}
}

/*
 * Write character `c' on `f'.
 */
void eif_file_pc(FILE* f, char c)
{
	errno = 0;
	if (putc(c, f) == EOF) {
		eise_io("FILE: unable to write CHARACTER value.");
	}
}

/*
 * Go to absolute position `pos' counted from start.
 */
void eif_file_go(FILE* f, EIF_INTEGER pos)
{
	errno = 0;
	if (fseek(f, pos, FS_START) != 0) {
		esys();
	}
	clearerr(f);
}

/*
 * Go to absolute position `pos' counted from end.
 */
void eif_file_recede(FILE* f, EIF_INTEGER pos)
{
	errno = 0;
	if (fseek(f, -pos, FS_END) != 0) {
		esys();
	}
	clearerr(f);
}

/*
 * Go to absolute position `pos' counted from current position.
 */
void eif_file_move(FILE* f, EIF_INTEGER pos)
{
	errno = 0;
	if (fseek(f, pos, FS_CUR) != 0) {
		esys();
	}
	clearerr(f);
}

/*
 * End of file.
 */
EIF_BOOLEAN eif_file_feof(FILE* fp)
{
	return (EIF_BOOLEAN)(feof(fp) != 0);
}

/*
 * Test whether file exists or not. If `name' represents a symbolic link,
 * it will check that pointed file does exist.
 */
EIF_BOOLEAN eif_file_exists(EIF_FILENAME name)
{
	int status;					/* System call status */
	rt_stat_buf buf;			/* Buffer to get file statistics */

	status = eif_file_stat(name, &buf, 1);

#ifdef EOVERFLOW
	if (status == -1) {
			/* If the file is larger than what our file routines can handle
			 * it does not mean that the file does not exist. It does but we
			 * cannot handle it.
			 * This is needed to be able to check existence of file bigger than 2GB.
			 */
		return (errno == EOVERFLOW ? EIF_TRUE : EIF_FALSE);
	} else {
		return EIF_TRUE;
	}
#else
	return (status == -1 ? EIF_FALSE : EIF_TRUE);
#endif
}

/*
 * Test whether file exists or not without following the symbolic link
 * if `name' represents one.
 */
EIF_BOOLEAN eif_file_path_exists(EIF_FILENAME name)
{
	int status;					/* System call status */
	rt_stat_buf buf;			/* Buffer to get file statistics */

	status = eif_file_stat(name, &buf, 0);

#ifdef EOVERFLOW
	if (status == -1) {
			/* If the file is larger than what our file routines can handle
			 * it does not mean that the file does not exist. It does but we
			 * cannot handle it.
			 * This is needed to be able to check existence of file bigger than 2GB.
			 */
		return (errno == EOVERFLOW ? EIF_TRUE : EIF_FALSE);
	} else {
		return EIF_TRUE;
	}
#else
	return (status == -1 ? EIF_FALSE : EIF_TRUE);
#endif
}

/*
 * Check whether access permission `op' are possible on file `name' using
 * real UID and real GID. This is probably only useful to setuid or setgid
 * programs.
 */
EIF_BOOLEAN eif_file_access(EIF_FILENAME name, EIF_INTEGER op)
{
	switch (op) {
	case 0: /* Does file exist? */
		return (EIF_BOOLEAN)((-1 != rt_access(name, F_OK)) ? '\01' : '\0');
	case 1: /* Test for search permission */
		return (EIF_BOOLEAN)((-1 != rt_access(name, X_OK)) ? '\01' : '\0');
	case 2: /* Test for write permission */
		return (EIF_BOOLEAN)((-1 != rt_access(name, W_OK)) ? '\01' : '\0');
	case 3: /* Test for read permission */
		return (EIF_BOOLEAN)((-1 != rt_access(name, R_OK)) ? '\01' : '\0');
	default:
		return EIF_FALSE;
	}
}

/*
 * Check whether the file `path' may be created: we need write permissions
 * in the parent directory and there must not be any file bearing that name
 * with no write permissions...
 */
EIF_BOOLEAN eif_file_creatable(EIF_FILENAME path, EIF_INTEGER nbytes)
{
	rt_stat_buf buf;			/* Buffer to get parent directory statistics */
	EIF_FILENAME temp = NULL;
	EIF_FILENAME ptr;
	size_t l_extra_bytes;

		/* Allocate one extra character, just to be sure since we might be appending characters. */
#ifdef EIF_WINDOWS
	l_extra_bytes = sizeof(wchar_t);
#else
	l_extra_bytes = sizeof(char);
#endif
	temp = (EIF_FILENAME)malloc(nbytes + l_extra_bytes);
	if (!temp) {
		enomem();
	} else {
			/* Search the directory separator. */
#ifdef EIF_WINDOWS
		memcpy(temp, path, nbytes);
		ptr = wcsrchr(temp, '\\');
		if (!ptr) {
				/* On Windows we can have a forward slash as separator. */
			ptr = wcsrchr(temp, '/');
		}
#else
		strcpy(temp, path);
		ptr = strrchr(temp, '/');
#endif
		if (ptr) {
			*ptr = '\0';
#ifdef EIF_WINDOWS
			if ((ptr == temp) || (*(ptr -1) == ':')) {
					/* path is of the form a:\bbb or \bbb, parent is a:\ or \ */
				wcscat(ptr, L"\\");
			}
#endif
		} else {
#ifdef EIF_WINDOWS
			wcsncpy(temp, L".", 2);
#else
			strcpy(temp, ".");
#endif
		}

			/* Does the parent exist? */
		if (!eif_file_exists(temp)) {
			free(temp);
			return (EIF_BOOLEAN)'\0';
		}

		rt_file_stat(temp, &buf);
		free(temp);

		if (S_ISDIR(buf.st_mode)) {	/* Is parent a directory? */
			if (eif_file_eaccess(&buf, 1)) {	/* Check for write permissions */
					/* Check if a non writable file `path' exists */
				if (eif_file_exists(path)) {
					rt_file_stat(path, &buf);
					if (S_ISDIR(buf.st_mode)) {
							/* File exists and it is already a directory, thus we cannot create a file. */
						return (EIF_BOOLEAN)'\0';
					} else {
						return (eif_file_eaccess(&buf, 1)); /* Check for write permissions to re create it */
					}
				}

				return (EIF_BOOLEAN)'\01';
			}
		}
	}
	return (EIF_BOOLEAN)'\0';
}

/*
 * Get an integer from `f'.
 */
EIF_INTEGER eif_file_gi(FILE* f)
{
	EIF_INTEGER i;

	errno = 0;
	if (fscanf(f, "%d", &i) < 0) {
		eise_io("FILE: unable to read INTEGER value.");
	}
	rt_swallow_nl(f);
	return i;
}

/*
 * Get a real from `f'.
 */
EIF_REAL_32 eif_file_gr(FILE* f)
{
	EIF_REAL_32 r;
	errno = 0;
	if (fscanf(f, "%f", &r) < 0) {
		eise_io("FILE: unable to read REAL value.");
	}
	rt_swallow_nl(f);
	return r;
}

/*
 * Get a double from `f'.
 */
EIF_REAL_64 eif_file_gd(FILE* f)
{
	EIF_REAL_64 d;
	errno = 0;
	if (fscanf(f, "%lf", &d) < 0) {
		eise_io("FILE: unable to read DOUBLE value.");
	}
	rt_swallow_nl(f);
	return d;
}

/*
 * Write `number' on `f'.
 */
void eif_file_pi(FILE* f, EIF_INTEGER number)
{
	errno = 0;
	if (fprintf(f, "%d", number) < 0) {
		eise_io("FILE: unable to write INTEGER value.");
	}
}

/*
 * Write `number' on `f'.
 */
void eif_file_pr(FILE* f, EIF_REAL_32 number)
{
	int res;
	errno = 0;

	if (number != number) {
		res = fprintf(f, "%s", "NaN");
	} else if (GE_real_32_is_negative_infinity(number)) {
		res = fprintf(f, "%s", "-Infinity");
	} else if (GE_real_32_is_positive_infinity(number)) {
		res = fprintf(f, "%s", "Infinity");
	} else {
		res = fprintf(f, "%g", number);
	}
    if (res < 0) {
		eise_io("FILE: unable to write REAL_32 value.");
	}
}

/*
 * Write double `val' onto `f'.
 */
void eif_file_pd(FILE* f, EIF_REAL_64 val)
{
	int res;
	errno = 0;

	if (val != val) {
		res = fprintf(f, "%s", "NaN");
	} else if (GE_real_64_is_negative_infinity(val)) {
		res = fprintf(f, "%s", "-Infinity");
	} else if (GE_real_64_is_positive_infinity(val)) {
			res = fprintf(f, "%s", "Infinity");
	} else {
		res = fprintf(f, "%.17g", val);
	}

	if (res < 0) {
		eise_io("FILE: unable to write REAL_64 value.");
	}
}

/*
 * Size of the stat structure. This is used by the Eiffel side to create
 * the area (special object) which will play the role of a stat buffer
 * structure.
 */
EIF_INTEGER stat_size(void)
{
	return (EIF_INTEGER)sizeof(rt_stat_buf);
}

#ifdef HAS_GETGROUPS
/*
 * Does the list of groups the user belongs to include `gid'?
 */
static EIF_BOOLEAN eif_group_in_list(int gid)
{
	Groups_t* group_list;
	int i, nb_groups, nb_groups_max;

	nb_groups_max = getgroups(0, (Groups_t*)0);
	if (nb_groups_max <= 0) {
		return EIF_FALSE;
	} else {
		group_list = (Groups_t*)malloc(nb_groups_max * sizeof(Groups_t));
		if (group_list == (Groups_t*)0) {
			xraise(EN_IO);
			return EIF_FALSE;
		} else if ((nb_groups = getgroups(nb_groups_max, group_list)) == -1) {
			free(group_list);
			xraise(EN_IO);
			return EIF_FALSE;
		} else {
			for (i = 0; i < nb_groups; i++)
				if (group_list[i] == gid) {
					free(group_list);
					return EIF_TRUE;
				}
			free(group_list);
			return EIF_FALSE;
		}
	}
}
#endif

/*
 * Check file permissions using effective UID and effective GID. The
 * current permission mode is held in the st_mode field of the stat()
 * buffer structure `buf'.
 */
EIF_BOOLEAN eif_file_eaccess(rt_stat_buf* buf, int op)
{
	int mode = buf->st_mode & ST_MODE;	/* Current mode */
#ifdef HAS_GETEUID
	int uid = buf->st_uid;				/* File owner */
	int gid = buf->st_gid;				/* File group */
	int euid, egid;						/* Effective user and group */
#endif

    switch (op) {
	case 0: /* Is file readable */
#ifdef EIF_WINDOWS
	return ((mode & S_IREAD) ? EIF_TRUE : EIF_FALSE);
#elif defined HAS_GETEUID
		euid = geteuid();
		egid = getegid();

		if (euid == 0)
			return EIF_TRUE;
		else if (uid == euid)
			return ((mode & S_IRUSR) ? EIF_TRUE : EIF_FALSE);
		else if (gid == egid)
			return ((mode & S_IRGRP) ? EIF_TRUE : EIF_FALSE);
#ifdef HAS_GETGROUPS
		else if (eif_group_in_list(gid))
			return ((mode & S_IRGRP) ? EIF_TRUE : EIF_FALSE);
#endif
		else
			return ((mode & S_IROTH) ? EIF_TRUE : EIF_FALSE);
#endif
	case 1: /* Is file writable */
#ifdef EIF_WINDOWS
		return ((mode & S_IWRITE) ? EIF_TRUE : EIF_FALSE);
#elif defined HAS_GETEUID
		euid = geteuid();
		egid = getegid();

		if (euid == 0)
			return EIF_TRUE;
		else if (uid == euid)
			return ((mode & S_IWUSR) ? EIF_TRUE : EIF_FALSE);
		else if (gid == egid)
			return ((mode & S_IWGRP) ? EIF_TRUE : EIF_FALSE);
#ifdef HAS_GETGROUPS
		else if (eif_group_in_list(gid))
			return ((mode & S_IWGRP) ? EIF_TRUE : EIF_FALSE);
#endif
		else
			return ((mode & S_IWOTH) ? EIF_TRUE : EIF_FALSE);
#endif
	case 2: /* Is file executable */
#ifdef EIF_WINDOWS
		return EIF_TRUE;
#elif defined HAS_GETEUID
		euid = geteuid();
		egid = getegid();

		if (euid == 0)
			return EIF_TRUE;
		else if (uid == euid)
			return ((mode & S_IXUSR) ? EIF_TRUE : EIF_FALSE);
		else if (gid == egid)
			return ((mode & S_IXGRP) ? EIF_TRUE : EIF_FALSE);
#ifdef HAS_GETGROUPS
		else if (eif_group_in_list(gid))
			return ((mode & S_IXGRP) ? EIF_TRUE : EIF_FALSE);
#endif
		else
			return ((mode & S_IXOTH) ? EIF_TRUE : EIF_FALSE);
#endif
	case 3: /* Is file setuid */
#ifdef EIF_WINDOWS
		return EIF_FALSE;
#else
		return ((mode & S_ISUID) ? EIF_TRUE : EIF_FALSE);
#endif
	case 4: /* Is file setgid */
#ifdef EIF_WINDOWS
		return EIF_FALSE;
#else
		return ((mode & S_ISGID) ? EIF_TRUE : EIF_FALSE);
#endif
	case 5: /* Is file sticky */
#ifdef EIF_WINDOWS
		return EIF_FALSE;
#else
		return ((mode & S_ISVTX) ? EIF_TRUE : EIF_FALSE);
#endif
	case 6: /* Is file owned by effective UID */
#ifdef HAS_GETEUID
		return ((uid == geteuid()) ? EIF_TRUE : EIF_FALSE);
#else
		return EIF_TRUE;
#endif
	case 7: /* Is file owned by real UID */
#ifdef HAS_GETEUID
		return ((uid == getuid()) ? EIF_TRUE : EIF_FALSE);
#else
		return EIF_TRUE;
#endif
	default:
		return EIF_FALSE;
	}
}

/*
 * Perform the field dereferencing from the appropriate stat structure,
 * which Eiffel cannot do directly.
 */
EIF_INTEGER eif_file_info(rt_stat_buf* buf, int op)
{
	switch (op) {
	case 0:	/* File permission mode */
		return (EIF_INTEGER)(buf->st_mode & ST_MODE);
	case 1:	/* Inode number */
		return (EIF_INTEGER)buf->st_ino;
	case 2:	/* Device inode resides on */
		return (EIF_INTEGER)buf->st_dev;
	case 3:	/* Device type */
		return (EIF_INTEGER)buf->st_rdev;
	case 4:	/* UID of file */
		return (EIF_INTEGER)buf->st_uid;
	case 5:	/* GID of file */
		return (EIF_INTEGER)buf->st_gid;
	case 6:	/* Size of file, in bytes */
			/* FIXME: This code should be upgraded to use 64-bit */
		return (EIF_INTEGER) buf->st_size;
	case 7:	/* Last modification time on file */
			/* FIXME: This code should be upgraded to use 64-bit */
		return (EIF_INTEGER)buf->st_mtime;
	case 8:	/* Last access made on file */
			/* FIXME: This code should be upgraded to use 64-bit */
		return (EIF_INTEGER)buf->st_atime;
	case 9:	/* Last status change */
			/* FIXME: This code should be upgraded to use 64-bit */
		return (EIF_INTEGER)buf->st_ctime;
	case 10: /* Number of links */
		return (EIF_INTEGER)buf->st_nlink;
	case 11: /* File type */
		return (EIF_INTEGER)(buf->st_mode & S_IFMT);
	case 12: /* Is file a directory */
		return (EIF_INTEGER)S_ISDIR(buf->st_mode);
	case 13: /* Is file a regular (plain) one */
		if (S_ISREG(buf->st_mode) || (0 == (buf->st_mode & S_IFMT)))
			return (EIF_INTEGER)EIF_TRUE;
		return (EIF_INTEGER)0;
	case 14: /* Is file a device, ie character or block device. */
		return (S_ISCHR(buf->st_mode) || S_ISBLK(buf->st_mode));
	case 15: /* Is file a character device */
		return (EIF_INTEGER)S_ISCHR(buf->st_mode);
	case 16: /* Is file a block device */
		return (EIF_INTEGER)S_ISBLK(buf->st_mode);
	case 17: /* Is file a FIFO */
		return (EIF_INTEGER)S_ISFIFO(buf->st_mode);
	case 18: /* Is file a symbolic link */
		return (EIF_INTEGER)S_ISLNK(buf->st_mode);
	case 19: /* Is file a socket */
		return (EIF_INTEGER)S_ISSOCK(buf->st_mode);
	default:
		return (EIF_INTEGER)0;
    }
}

/*
 * Return the Eiffel string filled in with the name associated with `uid'
 * if found in /etc/passwd. Otherwise, return fill it in with the numeric
 * value.
 */
EIF_REFERENCE eif_file_owner(int uid)
{
	char str[NAME_MAX];
#ifdef HAS_GETPWUID
	struct passwd* pp;

	pp = getpwuid(uid);
	if (pp == (struct passwd*) 0)
		sprintf(str, "%d", uid);		/* Not available: use UID */
	else
		strcpy(str, pp->pw_name);		/* Available: fetch login name */
#else
	sprintf(str, "%d", uid);			/* Not available: use UID */
#endif
	return GE_str(str);
}

/*
 * Return the Eiffel string filled in with the name associated with `gid'
 * if found in /etc/group. Otherwise, return fill it in with the numeric
 * value.
 */
EIF_REFERENCE eif_file_group(int gid)
{
	char str[NAME_MAX];
#ifdef HAS_GETGRGID
	struct group* gp; /* %%ss moved from above */

	gp = getgrgid(gid);
	if (gp == (struct group*)0)
		sprintf(str, "%d", gid);		/* Not available: use GID */
	else
		strcpy(str, gp->gr_name);		/* Available: fetch login name */
#else
	sprintf(str, "%d", gid);			/* Not available: use UID */
#endif
	return GE_str(str);
}

/*
 * Get an integer from `f'.
 */
EIF_INTEGER eif_file_gib(FILE* f)
{
	EIF_INTEGER i;
	errno = 0;
	if (fread (&i, sizeof(EIF_INTEGER), 1, f) != 1) {
		eise_io("FILE: unable to read INTEGER value.");
	}
	return i;
}

/*
 * Get a real from `f'.
 */
EIF_REAL_32 eif_file_grb(FILE* f)
{
	EIF_REAL_32 r;
	errno = 0;
	if (fread (&r, sizeof(EIF_REAL_32), 1, f) != 1) {
		eise_io("FILE: unable to read REAL_32 value.");
	}
	return r;
}

/*
 * Get a double from `f'.
 */
EIF_REAL_64 eif_file_gdb(FILE* f)
{
	EIF_REAL_64 d;
	errno = 0;
	if (fread (&d, sizeof(EIF_REAL_64), 1, f) != 1) {
		eise_io("FILE: unable to read REAL_64 value.");
	}
	return d;
}

/*
 * Open file `name' with the corresponding type `how'.
 */
EIF_POINTER eif_file_binary_open(EIF_FILENAME name, int how)
{
	FILE* fp;
	EIF_NATIVE_CHAR l_mode[FILE_TYPE_MAX];
	errno = 0;
#ifdef EIF_WINDOWS
	rt_file_set_open_mode(l_mode, how, 'b');
#else
	rt_file_set_open_mode(l_mode, how, '\0');
#endif
	fp = (FILE*)rt_file_fopen(name, l_mode);
	if (fp == (FILE*)0) {
		esys(); /* Open failed, raise exception */
	}
	return (EIF_POINTER)fp;

}

/*
 * Open file `fd' with the corresponding type `how'.
 */
EIF_POINTER eif_file_binary_dopen(int fd, int how)
{
	FILE* fp;
	EIF_NATIVE_CHAR l_mode[FILE_TYPE_MAX];
	errno = 0;
#ifdef EIF_WINDOWS
	rt_file_set_open_mode(l_mode, how, 'b');
#else
	rt_file_set_open_mode(l_mode, how, '\0');
#endif
	fp = (FILE*)rt_file_fdopen(fd, l_mode);
	if (fp == (FILE*)0) {
		esys(); /* Open failed, raise exception */
	}
	return (EIF_POINTER)fp;
}

/*
 * Reopen file `name' with the corresponding type `how' and substitute that
 * to the old stream described by `old'. This is useful to redirect 'stdout'
 * to another place, for instance.
 */
EIF_POINTER eif_file_binary_reopen(EIF_FILENAME name, int how, FILE* old)
{
	FILE* fp;
	EIF_NATIVE_CHAR l_mode[FILE_TYPE_MAX];
	errno = 0;
#ifdef EIF_WINDOWS
	rt_file_set_open_mode(l_mode, how, 'b');
#else
	rt_file_set_open_mode(l_mode, how, '\0');
#endif
	fp = (FILE*)rt_file_freopen(name, l_mode, old);
	if (fp == (FILE*)0) {
		esys(); /* Open failed, raise exception */
	}
	return (EIF_POINTER)fp;
}

/*
 * Write `number' on `f'.
 */
void eif_file_pib(FILE* f, EIF_INTEGER number)
{
	errno = 0;
	if (fwrite(&number, sizeof(EIF_INTEGER),1, f) != 1) {
		eise_io("FILE: unable to write INTEGER value.");
	}
}

/*
 * Write `number' on `f'.
 */
void eif_file_prb(FILE* f, EIF_REAL_32 number)
{
	errno = 0;
    if (fwrite(&number, sizeof(EIF_REAL_32),1, f) != 1) {
		eise_io("FILE: unable to write REAL_32 value.");
	}
}

/*
 * Write double `val' onto `f'.
 */
void eif_file_pdb(FILE* f, EIF_REAL_64 val)
{
	errno = 0;
	if (fwrite(&val, sizeof(EIF_REAL_64), 1, f) != 1) {
		eise_io("FILE: unable to write REAL_64 value.");
	}
}

/*
 * Access time of a file.
 * Seconds since epoch (01 January 1970) in UTC or 0 if time cannot be retrieved.
 * mode: Mode of operation: non-zero for modification and zero for access time.
 */
static EIF_INTEGER eif_file_date_for(EIF_FILENAME name, int mode)
{
	EIF_INTEGER result = 0;
#ifdef EIF_WINDOWS
		/* On NTFS file system, windows store UTC file stamps in 100 of nanoseconds
		 * starting from January 1st 0. Converted in seconds, this time is greater
		 * than 232 therefore we substract the EPOCH date January 1st 1970 to get
		 * a 32 bits representation of the date.
		 * FIXME: Manu 01/28/2004: On FAT file system, the date is in local time,
		 * meaning that the code below does not compensate if you change your timezone
		 * and will return a different date value for the same stamp just because
		 * you are in different time zone.
		 */

		 /* WARNING: This is using the Ansi version of the Win32 API. Remember
		  * that if you are doing any change below.
		  */

	WIN32_FIND_DATAW l_find_data;
	HANDLE l_file_handle;
	ULARGE_INTEGER l_date;

	l_file_handle = FindFirstFileW(name, &l_find_data);
	if (l_file_handle != INVALID_HANDLE_VALUE) {
			/* We do not need the file handle anymore, so we close it to
			 * avoid handle leak. */
		FindClose(l_file_handle);
		if (mode) {
			l_date.LowPart = l_find_data.ftLastWriteTime.dwLowDateTime;
			l_date.HighPart = l_find_data.ftLastWriteTime.dwHighDateTime;
		} else {
			l_date.LowPart = l_find_data.ftLastAccessTime.dwLowDateTime;
			l_date.HighPart = l_find_data.ftLastAccessTime.dwHighDateTime;
		}
			/* Convert 100-nanosecond intervals to seconds
			 * and ajust "1 Jan 1601" to "1 Jan 1970".
			 * The difference between "1 Jan 1601" and "1 Jan 1970"
			 * can be precomputed using the following code:
			 *	SYSTEMTIME epoch = {1970, 1, 0, 1, 0, 0, 0, 0};
			 *	FILETIME epoch_file;
			 *	ULARGE_INTEGER epoch_date;
			 *	SystemTimeToFileTime (&epoch, &epoch_file);
			 *	epoch_date.LowPart = epoch_file.dwLowDateTime;
			 *	epoch_date.HighPart = epoch_file.dwHighDateTime;
			 * time_shift = epoch_date.QuadPart / 10000000UL; // 11644473600
			 */
		result = (EIF_INTEGER)(l_date.QuadPart / GE_nat64(10000000) - GE_nat64(11644473600));
	}
#else
	rt_stat_buf	info;
	if (-1 != eif_file_stat(name, &info, 1)) {
		result = (mode ? (EIF_INTEGER)info.st_mtime: (EIF_INTEGER)info.st_atime);
	}
#endif
	return result;
}

/*
 * Modification time of a file.
 * Seconds since epoch (01 January 1970) in UTC or 0 if time cannot be retrieved.
 */
EIF_INTEGER eif_file_date(EIF_FILENAME name)
{
	return eif_file_date_for(name, 1);
}

/*
 * Access time of a file.
 * Seconds since epoch (01 January 1970) in UTC or 0 if time cannot be retrieved.
 */
EIF_INTEGER eif_file_access_date(EIF_FILENAME name)
{
	return eif_file_date_for(name, 0);
}

/*
 * Generate a temporary file with a name based on a template and
 * return a file descriptor to the file.
 * The template is overwritten with the name of the new file.
 * Return a non-zero file descriptor for the temporary file upon success, -1 otherwise.
 * `template'is the template to use for creating the temporary file.
 * It must match the rules for mk[s]temp (i.e. end in "XXXXXX").
 * On exit, it is updated with the new name.
 * `is_text_mode', if non-zero, then the temporary file is created
 * in text mode, otherwise in binary mode.<param>
 * Note: Code was inspired from https://github.com/mirror/mingw-w64/blob/master/mingw-w64-crt/misc/mkstemp.c
 */
EIF_INTEGER eif_file_mkstemp(EIF_FILENAME a_template, EIF_BOOLEAN is_text_mode)
{
	int fd;
#ifdef EIF_WINDOWS
	{
		size_t len = wcslen(a_template);
		size_t j, index;
		int32_t i, l_max;

		/* These are the (62) characters used in temporary filenames. */
		static const wchar_t letters[] = L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

		/* User may supply more than six trailing Xs */
		for (index = len - 6; index > 0 && a_template[index - 1] == L'X'; index--);

		/* Like OpenBSD, mkstemp() will try at least 2 ** 31 combinations before * giving up. */
		l_max = GE_max_int32;
		for (i = 0; i < l_max; i++) {
			for (j = index; j < len; j++) {
				a_template[j] = letters[rand() % 62];
			}
			fd = _wsopen(a_template, _O_CREAT | _O_EXCL | _O_RDWR | (is_text_mode ? _O_TEXT : _O_BINARY) | _O_NOINHERIT, _SH_DENYRW, _S_IREAD | _S_IWRITE);
			/* Success, if the file descriptor is valid or if there is a creation error that is not due to a file already existing. */
			if ((fd != -1) || (fd == -1 && errno != EEXIST)) {
				break;
			}
		}
	}
#else
	fd = mkstemp(a_template);
#endif

	return (EIF_INTEGER) fd;
}

/* Needed to compile ISE's compiler. */
void ca_store(char *area, long int siz, FILE *fil)
{
	if (fwrite(area, sizeof(char), siz, fil) != (size_t) siz)
		xraise(EN_IO);
}

/* Needed to compile ISE's compiler. */
void write_int(FILE *file, EIF_INTEGER_32 val)
{
	fwrite(&val, sizeof(EIF_INTEGER_32), 1, file);
}

#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"C functions used to implement class CONSOLE"

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2006-2017, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef EIF_CONSOLE_C
#define EIF_CONSOLE_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef EIF_CONSOLE_H
#include "eif_console.h"
#endif
#ifndef EIF_FILE_H
#include "eif_file.h"
#endif
#ifndef GE_CONSOLE_H
#include "ge_console.h"
#endif

#ifdef EIF_WINDOWS
#include <windows.h>
#endif
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

EIF_POINTER console_def(EIF_INTEGER file)
{
#ifdef EIF_WINDOWS
		/* The following is needed to avoid the function `WideCharToMultiByte`
		   in ENCODING_IMP.cwin_wide_char_to_multi_byte to produce an error
		   ERROR_INVALID_PARAMETER when the code page passed as argument is
		   the code page of the console. */
	UINT l_code_page;
	l_code_page = GetConsoleOutputCP();
	if (l_code_page != 0) {
		SetConsoleOutputCP(l_code_page);
	}
#endif
	switch (file) {
	case 0:
		return (EIF_POINTER)stdin;
	case 1:
		setvbuf(stdout, NULL, _IONBF, 0);
		return (EIF_POINTER)stdout;
	case 2:
		setvbuf (stderr, NULL, _IONBF, 0);
		return (EIF_POINTER)stderr;
	default:
		return NULL;
	}
}

EIF_BOOLEAN console_eof(FILE* fp)
{
	GE_show_console();
	return eif_file_feof(fp);
}

EIF_CHARACTER_8 console_separator(FILE *f)
{
	GE_show_console();
	return eif_file_lh(f);
}

void console_ps(FILE* f, char* str, EIF_INTEGER len)
{
	GE_show_console();
	eif_file_ps(f, str, len);
}

void console_pr(FILE* f, EIF_REAL_32 number)
{
	GE_show_console();
	eif_file_pr(f, number);
}

void console_pc(FILE* f, EIF_CHARACTER_8 c)
{
	GE_show_console();
	eif_file_pc(f, c);
}

void console_pd(FILE* f, EIF_REAL_64 val)
{
	GE_show_console();
	eif_file_pd(f, val);
}

void console_pi(FILE* f, EIF_INTEGER number)
{
	GE_show_console();
	eif_file_pi(f, number);
}

void console_tnwl(FILE* f)
{
	GE_show_console();
	console_pc(f,'\n');
}

EIF_CHARACTER_8 console_readchar(FILE* f)
{
	GE_show_console();
	return eif_file_gc(f);
}

EIF_REAL_32 console_readreal(FILE* f)
{
	GE_show_console();
	return eif_file_gr(f);
}

EIF_INTEGER console_readint(FILE* f)
{
	GE_show_console();
	return eif_file_gi(f);
}

EIF_REAL_64 console_readdouble(FILE* f)
{
	GE_show_console();
	return eif_file_gd(f);
}

EIF_INTEGER console_readword(FILE* f, char* s, EIF_INTEGER bound, EIF_INTEGER start)
{
	GE_show_console();
	return eif_file_gw(f, s, bound, start);
}

EIF_INTEGER console_readline(FILE* f, char* s, EIF_INTEGER bound, EIF_INTEGER start)
{
	GE_show_console();
	return eif_file_gs(f, s, bound, start);
}

void console_next_line(FILE* f)
{
	GE_show_console();
	eif_file_tnil(f);
}

EIF_INTEGER console_readstream(FILE* f, char* s, EIF_INTEGER bound)
{
	GE_show_console();
	return eif_file_gss(f, s, bound);
}

void console_file_close (FILE* f)
{
	GE_show_console();
	eif_file_close(f);
}

#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"C functions used to implement class DIRECTORY"

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2006-2018, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef EIF_DIR_C
#define EIF_DIR_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef EIF_DIR_H
#include "eif_dir.h"
#endif

#include <sys/stat.h>
#ifdef EIF_WINDOWS
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
	HANDLE handle;
	WIN32_FIND_DATAW last_entry;
	wchar_t	*name;
} GE_directory;
#ifdef __cplusplus
}
#endif
#include <io.h> /* for 'access' */
#include <direct.h> /* (ch|rm)dir */
#include <wchar.h>
#else
#include <dirent.h>
#include <unistd.h>
#endif
#include <string.h>

#ifndef PATH_MAX
#define PATH_MAX 1024 /* Maximum length of full path name */
#endif

#ifndef S_IRUSR
#define S_IRUSR 0000400
#endif
#ifndef S_IWUSR
#define S_IWUSR 0000200
#endif
#ifndef S_IXUSR
#define S_IXUSR 0000100
#endif
#ifndef S_IRGRP
#define S_IRGRP 0000040
#endif
#ifndef S_IWGRP
#define S_IWGRP 0000020
#endif
#ifndef S_IXGRP
#define S_IXGRP 0000010
#endif
#ifndef S_IROTH
#define S_IROTH 0000004
#endif
#ifndef S_IWOTH
#define S_IWOTH 0000002
#endif
#ifndef S_IXOTH
#define S_IXOTH 0000001
#endif
#ifndef S_IFDIR
#define S_IFDIR 0040000
#endif
#ifndef S_IFCHR
#define S_IFCHR 0020000
#endif
#ifdef __LCC__
#ifndef _S_IFCHR
#define _S_IFCHR S_IFCHR
#endif
#endif
#ifndef S_IFBLK
#define S_IFBLK 0060000
#endif
#ifndef S_IFREG
#define S_IFREG 0100000
#endif
#ifndef S_IFIFO
#define S_IFIFO 0010000
#endif
#ifndef S_IFLNK
#define S_IFLNK 0120000
#endif
#ifndef S_IFSOCK
#define S_IFSOCK 0140000
#endif
#ifndef S_IFMT
#define S_IFMT 0170000
#endif
#ifndef S_ISREG
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISCHR
#define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
#endif
#ifndef S_ISBLK
#define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#endif
#ifndef S_ISLNK
#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#endif
#ifndef S_ISFIFO
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#endif

/* Keep only permission mode */
#ifndef ST_MODE
#define ST_MODE 0x0fff
#endif

/* The following access constants seems to be universally accepeted. And
 * anyway, it's only after BSD 4.1 that the symbolic constants appeared.
 */
#ifndef R_OK
#define R_OK 4
#define W_OK 2
#define X_OK 1
#define F_OK 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef EIF_WINDOWS
#	ifdef __LCC__
#		define rt_stat_buf	struct stat
#	elif defined EIF_64_BITS
#		define rt_stat_buf	struct _stat64
#	else
#		define rt_stat_buf	struct _stat64i32
#	endif
#else
#	define rt_stat_buf		struct stat
#endif

#ifdef EIF_WINDOWS
#	ifdef __LCC__
#		define rt_stat		_wstat
#		define rt_fstat		fstat
#	elif defined(EIF_64_BITS)
#		define rt_stat		_wstat64
#		define rt_fstat		_fstat64
#	else
#		define rt_stat		_wstat64i32
#		define rt_fstat		_fstat64i32
#	endif
#	define rt_access		_waccess
#else
#	define rt_stat			stat
#	define rt_fstat			fstat
#	define rt_access		access
#endif

EIF_POINTER eif_dir_open(EIF_FILENAME dirname)
{
#ifdef EIF_WINDOWS
	GE_directory* result = (GE_directory*)malloc(sizeof(GE_directory));
	result->name = dirname;
	result->handle = 0;
	return result;
#else
	return (opendir(((char*)dirname)));
#endif
}

EIF_POINTER eif_dir_next(EIF_POINTER dir)
{
#ifdef EIF_WINDOWS
	BOOL r;
	size_t a_name_length;
	wchar_t * wname;
	GE_directory* GE_dir = (GE_directory*)dir;
	HANDLE h = GE_dir->handle;

	if (h) {
		r = FindNextFileW(h, &(GE_dir->last_entry));
	} else {
		r = EIF_FALSE;
			/* Allocate additional space for "\\*": 2 characters + terminating zero */
		a_name_length = wcslen(GE_dir->name);
		wname = (wchar_t *)malloc((a_name_length + 3) * sizeof(wchar_t));
		if (!wname) {
			enomem();
		} else {
			wcscpy(wname, GE_dir->name);
			if (wname[a_name_length - 1] == '\\') {
				wcscat(wname, L"*");
			} else {
				wcscat(wname , L"\\*");
			}
			h = FindFirstFileW(wname, &(GE_dir->last_entry));
			free(wname);
			if (h != INVALID_HANDLE_VALUE) {
				GE_dir->handle = h;
				r = EIF_TRUE;
			}
		}
	}
	if (r) {
		return GE_dir->last_entry.cFileName;
	} else {
		return NULL;
	}
#else
	struct dirent* p = readdir((DIR*)dir);
	if (p) {
		return p->d_name;
	} else {
		return NULL;
	}
#endif
}

EIF_POINTER eif_dir_rewind(EIF_POINTER dir, EIF_FILENAME a_name)
{
#ifdef EIF_WINDOWS
	GE_directory* GE_dir = (GE_directory*)dir;
	HANDLE h = GE_dir->handle;

	if (h) {
		FindClose(h);
	}
	GE_dir->handle = 0;
#else
	DIR* GE_dir = (DIR*)dir;
	rewinddir(GE_dir);
#endif
	return GE_dir;
}

void eif_dir_close(EIF_POINTER dir)
{
#ifdef EIF_WINDOWS
	GE_directory* GE_dir = (GE_directory*)dir;
	HANDLE h = GE_dir->handle;

	if (h) {
		FindClose(h);
	}
	GE_dir->handle = 0;
	free(GE_dir);
#else
	closedir((DIR*)dir);
#endif
}

EIF_BOOLEAN eif_dir_exists(EIF_FILENAME dirname)
{
	rt_stat_buf buf;

	return (EIF_BOOLEAN)((!rt_stat (dirname, &buf) && S_ISDIR(buf.st_mode)) ? EIF_TRUE : EIF_FALSE);
}

EIF_BOOLEAN eif_dir_is_readable(EIF_FILENAME dirname)
{
#ifdef EIF_WINDOWS
	return (EIF_BOOLEAN)(_waccess (dirname, R_OK) != -1);
#else
	int uid, gid;
	int euid, egid;
	int mode;
	rt_stat_buf buf;

	rt_stat(dirname, &buf);
	mode = buf.st_mode & ST_MODE;
	uid = buf.st_uid;
	gid = buf.st_gid;
	euid = geteuid();
	egid = getegid();
	if (euid == 0)
		return EIF_TRUE;
	else if (uid == euid)
		return ((mode & S_IRUSR) ? EIF_TRUE : EIF_FALSE);
	else if (gid == egid)
		return ((mode & S_IRGRP) ? EIF_TRUE : EIF_FALSE);
#ifdef HAS_GETGROUPS
	else if (eif_group_in_list(gid))
		return ((mode & S_IRGRP) ? EIF_TRUE : EIF_FALSE);
#endif
	else
		return ((mode & S_IROTH) ? EIF_TRUE : EIF_FALSE);
#endif
}

EIF_BOOLEAN eif_dir_is_executable(EIF_FILENAME dirname)
{
#ifdef EIF_WINDOWS
	return (EIF_BOOLEAN)(_waccess(dirname, F_OK) != -1);
#else
	int uid, gid;
	int euid, egid;
	int mode;
	rt_stat_buf buf;

	rt_stat(dirname, &buf);
	mode = buf.st_mode & ST_MODE;
	uid = buf.st_uid;
	gid = buf.st_gid;
	euid = geteuid();
	egid = getegid();
	if (euid == 0)
		return EIF_TRUE;
	else if (uid == euid)
		return ((mode & S_IXUSR) ? EIF_TRUE : EIF_FALSE);
	else if (gid == egid)
		return ((mode & S_IXGRP) ? EIF_TRUE : EIF_FALSE);
#ifdef HAS_GETGROUPS
	else if (eif_group_in_list(gid))
		return ((mode & S_IXGRP) ? EIF_TRUE : EIF_FALSE);
#endif
	else
		return ((mode & S_IXOTH) ? EIF_TRUE : EIF_FALSE);
#endif
}

EIF_BOOLEAN eif_dir_is_writable(EIF_FILENAME dirname)
{
#ifdef EIF_WINDOWS
	return (EIF_BOOLEAN)(_waccess(dirname, W_OK) != -1);
#else
	int uid, gid;
	int euid, egid;
	int mode;
	rt_stat_buf buf;

	rt_stat(dirname, &buf);
	mode = buf.st_mode & ST_MODE;
	uid = buf.st_uid;
	gid = buf.st_gid;
	euid = geteuid();
	egid = getegid();
	if (euid == 0)
		return EIF_TRUE;
	else if (uid == euid)
		return ((mode & S_IWUSR) ? EIF_TRUE : EIF_FALSE);
	else if (gid == egid)
		return ((mode & S_IWGRP) ? EIF_TRUE : EIF_FALSE);
#ifdef HAS_GETGROUPS
	else if (eif_group_in_list(gid))
		return ((mode & S_IWGRP) ? EIF_TRUE : EIF_FALSE);
#endif
	else
		return ((mode & S_IWOTH) ? EIF_TRUE : EIF_FALSE);
#endif
}

EIF_BOOLEAN eif_dir_is_deletable(EIF_FILENAME name)
{
	return eif_dir_is_writable(name);
}

EIF_CHARACTER_8 eif_dir_separator(void)
{
#ifdef EIF_WINDOWS
	return (EIF_CHARACTER_8)'\\';
#else
	return (EIF_CHARACTER_8)'/';
#endif
}

EIF_INTEGER eif_dir_current(EIF_FILENAME a_buffer, EIF_INTEGER a_count)
{
	EIF_INTEGER l_nbytes;

#ifdef EIF_WINDOWS
	wchar_t *subpart;
	wchar_t drive[2];
	drive [0] = '.';
	drive [1] = '\0';
		/* First calculate the length of the buffer we need to hold the current working directory. */
	l_nbytes = (GetFullPathNameW(drive, 0, NULL, &subpart) + 1) * sizeof(wchar_t);

	if (l_nbytes == 0) {
			/* Failure: we cannot retrieve our current directory. */
		l_nbytes = -1;
	} else {
		if (a_buffer && (a_count >= l_nbytes)) {
			l_nbytes = (GetFullPathNameW(drive, l_nbytes / sizeof(wchar_t), a_buffer, &subpart) + 1) * sizeof(wchar_t);
		}
	}
	return l_nbytes;
#else
	char *cwd;
	cwd = getcwd(NULL, PATH_MAX);

	if (cwd) {
		l_nbytes = (strlen(cwd) + 1) * sizeof(char);
		if (a_buffer && (a_count >= l_nbytes)) {
			memcpy(a_buffer, cwd, l_nbytes);
		}
		free(cwd);	/* Not `eif_free', getcwd() call malloc in POSIX.1 */
	} else {
		l_nbytes = -1;
	}

	return l_nbytes;

#endif
}

EIF_INTEGER eif_chdir(EIF_FILENAME path)
{
#ifdef EIF_WINDOWS
	return _wchdir(path);
#else
	return chdir(path);
#endif
}

#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"C functions used to implement class EXECUTION_ENVIRONMENT"

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2006-2017, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef EIF_MISC_C
#define EIF_MISC_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef EIF_MISC_H
#include "eif_misc.h"
#endif

#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#include <string.h>
#include <direct.h> /* for 'getcwd' */
#endif

#ifndef PATH_MAX
#define PATH_MAX 1024 /* Maximum length of full path name */
#endif

#ifdef __cplusplus
extern "C" {
#endif

EIF_INTEGER eif_system(EIF_NATIVE_CHAR* s)
{
#ifdef EIF_WINDOWS
	return (EIF_INTEGER)_wsystem(s);
#else
	return (EIF_INTEGER)system(s);
#endif
}

void eif_system_asynchronous(EIF_NATIVE_CHAR* cmd)
{
#ifdef EIF_WINDOWS
	STARTUPINFOW siStartInfo;
	PROCESS_INFORMATION procinfo;
	wchar_t* current_dir;
	EIF_INTEGER result;

	current_dir = (wchar_t*)_wgetcwd(NULL, PATH_MAX);
	memset(&siStartInfo, 0, sizeof(siStartInfo));
	siStartInfo.cb = sizeof(STARTUPINFO);
	siStartInfo.lpTitle = NULL;
	siStartInfo.lpReserved = NULL;
	siStartInfo.lpReserved2 = NULL;
	siStartInfo.cbReserved2 = 0;
	siStartInfo.lpDesktop = NULL;
	siStartInfo.dwFlags = STARTF_FORCEONFEEDBACK;
	result = CreateProcessW(
		NULL,
		cmd,
		NULL,
		NULL,
		TRUE,
		CREATE_NEW_CONSOLE,
		NULL,
		current_dir,
		&siStartInfo,
		&procinfo);
	if (result) {
		CloseHandle(procinfo.hProcess);
		CloseHandle(procinfo.hThread);
	}
	_wchdir(current_dir);
	free(current_dir);
#else
	int status;

	switch (fork()) {
	case -1:
		return;
	case 0:
		break;
	default:
		return;
	}
	status = system(cmd);
	exit(0);
#endif
}

void eif_sleep(EIF_INTEGER_64 nanoseconds)
{
	/*
	 * Suspend thread execution for interval specified by `nanoseconds'.
	 * Use the most precise sleep function if possible.
	 */

#ifdef HAS_NANOSLEEP
	struct timespec req;
	struct timespec rem;
	req.tv_sec = nanoseconds / 1000000000;
	req.tv_nsec = nanoseconds % 1000000000;
	while ((nanosleep (&req, &rem) == -1) && (errno == EINTR)) {
			/* Function is interrupted by a signal.   */
			/* Let's call it again to complete pause. */
		req = rem;
	}
#else
#	ifdef HAS_USLEEP
#		define EIF_SLEEP_PRECISION 1000
#		define EIF_SLEEP_TYPE      unsigned long
#		define EIF_SLEEP_FUNCTION  usleep
#	elif defined EIF_WINDOWS
#		define EIF_SLEEP_PRECISION 1000000
#		define EIF_SLEEP_TYPE      DWORD
#		define EIF_SLEEP_FUNCTION  Sleep
#	else
#		define EIF_SLEEP_PRECISION 1000000000
#		define EIF_SLEEP_TYPE      unsigned int
#		define EIF_SLEEP_FUNCTION  sleep
#	endif
		/* Set total delay time */
	EIF_INTEGER_64 total_time = nanoseconds / EIF_SLEEP_PRECISION;
		/* Set maximum timeout that can be handled by one API call */
	EIF_SLEEP_TYPE timeout = ~((~ (EIF_SLEEP_TYPE) 0) << (sizeof timeout * 8 - 1));
	if ((nanoseconds % EIF_SLEEP_PRECISION) > 0) {
			/* Increase delay to handle underflow */
		total_time++;
	}
	while (total_time > 0) {
			/* Sleep for maximum timeout not exceeding time left */
		if (timeout > total_time) {
			timeout = (EIF_SLEEP_TYPE)total_time;
		}
		EIF_SLEEP_FUNCTION(timeout);
		total_time -= timeout;
	}
#  undef EIF_SLEEP_PRECISION
#  undef EIF_SLEEP_TYPE
#  undef EIF_SLEEP_FUNCTION
#endif
}

#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif
/*
	description:

		"C functions used to implement class COM_FAILURE"

	system: "Gobo Eiffel Compiler"
	copyright: "Copyright (c) 2016-2018, Eric Bezault and others"
	license: "MIT License"
*/

#ifndef GE_COM_FAILURE_C
#define GE_COM_FAILURE_C
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef GE_COM_FAILURE_H
#include "ge_com_failure.h"
#endif

#ifdef EIF_WINDOWS
#include <winerror.h>
#include <tchar.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

EIF_INTEGER GE_ccom_hresult(EIF_POINTER an_exception_code)
{
#ifdef EIF_WINDOWS
	char* stopstring = NULL;
	long result = 0, high_bits = 0, low_bits = 0;
	char high_str[7];
	char* exception_code = (char*)(an_exception_code);

	if (exception_code != NULL) {
		strncpy(high_str, exception_code, 6);
		high_str[6] = '\0';

		high_bits = strtol(high_str, &stopstring, 16);
		low_bits = strtol(exception_code + 6, &stopstring, 16);
		result = (high_bits << 16) + low_bits;
	}
	return (EIF_INTEGER)result;
#else
	return 0;
#endif
}

EIF_INTEGER GE_ccom_hresult_code(EIF_INTEGER an_hresult)
{
#ifdef EIF_WINDOWS
	return HRESULT_CODE(an_hresult);
#else
	return 0;
#endif
}

EIF_INTEGER GE_ccom_hresult_facility(EIF_INTEGER an_hresult)
{
#ifdef EIF_WINDOWS
	return HRESULT_FACILITY(an_hresult);
#else
	return 0;
#endif
}

/*
 * Get text from error `a_code'. It is up to the caller to free
 * the returned buffer using `GE_ccom_local_free'.
 */
EIF_POINTER GE_ccom_error_text(EIF_INTEGER a_code)
{
#ifdef EIF_WINDOWS
	LPVOID result;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		a_code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR)&result,
		0,
		NULL
		);
	return result;
#else
	return 0;
#endif
}

/*
 * Number of characters in `ptr'.
 */
EIF_INTEGER_32 GE_ccom_strlen(EIF_POINTER ptr)
{
#ifdef EIF_WINDOWS
	return (EIF_INTEGER_32)_tcslen((TCHAR*)ptr);
#else
	return 0;
#endif
}

/*
 * Number of bytes occupied by a TCHAR.
 */
EIF_INTEGER_32 GE_ccom_character_size(void)
{
#ifdef EIF_WINDOWS
	return sizeof(TCHAR);
#else
	return 0;
#endif
}

/*
 * Free `a_ptr' using LocalFree.
 */
void GE_ccom_local_free(EIF_POINTER a_ptr)
{
#ifdef EIF_WINDOWS
	LocalFree((HLOCAL)a_ptr);
#endif
}

#ifdef __cplusplus
}
#endif

#endif
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif