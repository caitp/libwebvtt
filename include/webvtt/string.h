#ifndef __WEBVTT_CSTRING_H__
#	define __WEBVTT_CSTRING_H__
#	include "util.h"
/**
 * webvtt_wchar - A utf16 surrogate/character
 */
typedef webvtt_uint16 webvtt_wchar;

/**
 * webvtt_string - A buffer of utf16 characters
 */
typedef struct webvtt_string_t *webvtt_string;

/**
 * These objects are NOT reference counted, they
 * are not meant to be shared.
 *
 * If this is required by mozilla, it can be added.
 */
struct
webvtt_string_t
{
	webvtt_uint32 alloc;
	webvtt_uint32 length;
	webvtt_wchar *buffer;
	webvtt_wchar array[1];
};

void webvtt_string_new( webvtt_uint32 length, webvtt_string *ppstr );
void webvtt_string_realloc( webvtt_uint32 length, webvtt_string *ppstr );
void webvtt_string_puts( webvtt_string *ppstr, webvtt_char *str, webvtt_uint32 length );

#endif