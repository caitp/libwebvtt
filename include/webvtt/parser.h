#ifndef __WEBVTT_CPARSER_H__
#	define __WEBVTT_CPARSER_H__
#	include "string.h"
#	include "reader.h"

typedef struct webvtt_parser_t *webvtt_parser;
typedef struct webvtt_cue_t *webvtt_cue;
typedef webvtt_uint64 webvtt_timestamp_t;

typedef enum
webvtt_flags_t
{
	/**
	 * WEBVTT_NOWAIT
	 * will not wait for any signal before continuing to attempt
	 * to read from the webvtt_reader. The default mode. For normal files,
	 * this will not cause any problems. However, when reading from a socket or STDIN
	 * or similar, this could be problematic.
	 */
	WEBVTT_NOWAIT = 0,
	
	/** 
	 * WEBVTT_WAIT_SIGNAL
	 * will not try to read from the reader only if signalled
	 */
	WEBVTT_WAIT_SIGNAL = 1,
} webvtt_flags;

typedef enum
webvtt_status_t
{
	WEBVTT_SUCCESS = 0,
	WEBVTT_OUT_OF_MEMORY = -1,
	WEBVTT_PARSE_ERROR = -2,
	WEBVTT_READ_ERROR = -3,
	WEBVTT_INVALID_PARAM = -4,
	WEBVTT_NOT_SUPPORTED = -5,
} webvtt_status;

webvtt_status webvtt_parser_new( webvtt_flags flags, webvtt_reader preader, webvtt_parser *pparser );
void webvtt_parser_delete( webvtt_parser *pparser );
webvtt_status webvtt_parser_get_cue( webvtt_parser self, webvtt_cue *pcue );
void webvtt_parser_get_error( webvtt_parser parser );

webvtt_uint64 webvtt_timestamp_get_hours( webvtt_timestamp_t ms );
webvtt_uint32 webvtt_timestamp_get_minutes( webvtt_timestamp_t ms );
webvtt_uint32 webvtt_timestamp_get_seconds( webvtt_timestamp_t ms );
webvtt_uint32 webvtt_timestamp_get_milliseconds( webvtt_timestamp_t ms );

struct
webvtt_cue_t
{
	webvtt_timestamp_t from;
	webvtt_timestamp_t until;
	webvtt_string id;
	webvtt_string payload;
};

#endif