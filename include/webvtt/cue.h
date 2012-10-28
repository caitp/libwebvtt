#ifndef __WEBVTT_CUE_H__
#	define __WEBVTT_CUE_H__
#	include "util.h"
#	include "string.h"

typedef struct webvtt_cue_t *webvtt_cue;
typedef double webvtt_timestamp_t, webvtt_timestamp;
typedef struct webvtt_cue_settings_t *webvtt_cue_settings;
typedef struct webvtt_cs_vertical_t *webvtt_cs_vertical;
typedef struct webvtt_cs_line_t *webvtt_cs_line;
typedef struct webvtt_cs_position_t *webvtt_cs_position;
typedef struct webvtt_cs_size_t *webvtt_cs_size;
typedef struct webvtt_cs_align_t *webvtt_cs_align;


struct
webvtt_cue_settings_t
{
	webvtt_cs_vertical vertical;
	webvtt_cs_line line;
	webvtt_cs_position position;
	webvtt_cs_size size;
	webvtt_cs_align align;
};

enum webvtt_vertical_type
{
	WEBVTT_VERTICAL_LR = 0,
	WEBVTT_VERTICAL_RL = 1
};
struct
webvtt_cs_vertical_t
{
	webvtt_uint value;
};

struct
webvtt_cs_line_t
{
	webvtt_bool is_percentage;
	union
	{
		webvtt_int line;
		webvtt_uint percentage;
	} value;
};

struct
webvtt_cs_position_t
{
	webvtt_uint value;
};

struct
webvtt_cs_size_t
{
	webvtt_uint value;
};

enum webvtt_align_type
{
	WEBVTT_ALIGN_START = 0,
	WEBVTT_ALIGN_MIDDLE,
	WEBVTT_ALIGN_END
};
struct
webvtt_cs_align_t
{
	webvtt_uint value;
};

struct
webvtt_cue_t
{
	webvtt_timestamp_t from;
	webvtt_timestamp_t until;
	webvtt_cue_settings settings;
	webvtt_string id;
	webvtt_string payload;
};

webvtt_status webvtt_create_cue( webvtt_cue *pcue );
void webvtt_delete_cue( webvtt_cue *pcue );
webvtt_cs_align webvtt_create_cue_align( webvtt_cue cue );
webvtt_cs_vertical webvtt_create_cue_vertical( webvtt_cue cue );
webvtt_cs_line webvtt_get_create_line( webvtt_cue cue );
webvtt_cs_size webvtt_get_create_size( webvtt_cue cue );
webvtt_cs_position webvtt_create_cue_position( webvtt_cue cue );
int webvtt_validate_cue( webvtt_cue cue );

#endif