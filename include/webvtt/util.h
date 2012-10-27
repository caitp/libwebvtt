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
typedef webvtt_int webvtt_bool;
typedef webvtt_uint32 webvtt_length;

/**
 * Memory allocation callbacks, which allow overriding the allocation strategy.
 */
typedef void * (WEBVTT_CALLBACK *webvtt_alloc_fn_ptr)( void *userdata, webvtt_length nbytes );
typedef void (WEBVTT_CALLBACK *webvtt_free_fn_ptr)( void *userdata, void *pmem );

/**
 * Allocation functions. webvtt_set_allocator() should really be the first function called.
 * However, it will do nothing (and not report error) if objects have already been allocated
 * and not freed. Therefore, it is NOT safe to assume that it worked and use the supplied
 * function pointers directly.
 *
 * Currently, set_allocator (and the other allocation functions) do not use any locking mechanism,
 * so the library cannot be considered to be thread-safe at this time if changing the allocator is
 * used.
 *
 * I don't believe there is much of a reason to worry about the overhead of using function pointers
 * for allocation, as it is negligible compared to the act of allocating memory itself, and having
 * a configurable allocation strategy could be very useful.
 */
void *webvtt_alloc( webvtt_length nb );
void *webvtt_alloc0( webvtt_length nb );
void webvtt_free( void *data );
void webvtt_set_allocator( webvtt_alloc_fn_ptr alloc, webvtt_free_fn_ptr free, void *userdata );

typedef enum webvtt_status_t webvtt_status;

enum
webvtt_status_t
{
	WEBVTT_SUCCESS = 0,
	WEBVTT_UNFINISHED = -1,
	WEBVTT_PARSE_ERROR = -2,
	WEBVTT_OUT_OF_MEMORY = -3,
	WEBVTT_INVALID_PARAM = -4,
	WEBVTT_NOT_SUPPORTED = -5,
};

#endif