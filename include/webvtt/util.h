#ifndef __WEBVTT_CUTIL_H__
#	define __WEBVTT_CUTIL_H__

#	if defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
#		define WEBVTT_OS_WIN32 1
#		if defined(_WIN64)
#			define WEBVTT_OS_WIN64 1
#		endif
#	endif

#	if defined(_MSC_VER)
#		define WEBVTT_CC_MSVC 1
#		define WEBVTT_CALLBACK __cdecl
#		if _MSC_VER >= 1600
#			define WEBVTT_HAVE_STDINT 1
#		endif
#	elif defined(__GNUC__)
#		define WEBVTT_CC_GCC 1
#		define WEBVTT_HAVE_STDINT 1
#		define WEBVTT_CALLBACK __attribute__((cdecl))
#	else
#		define WEBVTT_CC_UNKNOWN 1
#		define WEBVTT_CALLBACK /* default calling convention */
#	endif

#	if WEBVTT_HAVE_STDINT
#		include <stdint.h>
		typedef int8_t webvtt_int8;
		typedef int16_t webvtt_int16;
		typedef int32_t webvtt_int32;
		typedef int64_t webvtt_int64;
		typedef uint8_t webvtt_uint8;
		typedef uint16_t webvtt_uint16;
		typedef uint32_t webvtt_uint32;
		typedef uint64_t webvtt_uint64;
#	elif defined(_MSC_VER)
		typ4edef signed __int8 webvtt_int8;
		typedef signed __int16 webvtt_int16;
		typedef signed __int32 webvtt_int32;
		typedef signed __int64 webvtt_int64;
		typedef unsigned __int8 webvtt_uint8;
		typedef unsigned __int16 webvtt_uint16;
		typedef unsigned __int32 webvtt_uint32;
		typedef unsigned __int64 webvtt_uint64;
#	elif WEBVTT_CC_UNKNOWN
#		warn "Unknown compiler. Compiler specific int-types probably broken!"
		typedef signed char webvtt_int8;
		typedef signed short webvtt_int16;
		typedef signed long webvtt_int32;
		typedef signed long long webvtt_int64;
		typedef unsigned char webvtt_uint8;
		typedef unsigned short webvtt_uint16;
		typedef unsigned long webvtt_uint32;
		typedef unsigned long long webvtt_uint64;
#	endif

typedef signed int webvtt_int;
typedef signed char webvtt_char;
typedef signed short webvtt_short;
typedef signed long webvtt_long;
typedef signed long long webvtt_longlong;
typedef unsigned int webvtt_uint;
typedef unsigned char webvtt_uchar;
typedef unsigned short webvtt_ushort;
typedef unsigned long webvtt_ulong;
typedef unsigned long long webvtt_ulonglong;
typedef webvtt_uint8 webvtt_byte;
#endif