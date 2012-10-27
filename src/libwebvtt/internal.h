#ifndef __INTERNAL_H__
#	define __INTERNAL_H__
#	include <webvtt/cue.h>

typedef struct cue_settings cue_settings;
typedef struct cue cue;

struct
cue_settings
{
	webvtt_cs_vertical vertical;
	webvtt_cs_line line;
	webvtt_cs_position position;
	webvtt_cs_size size;
	webvtt_cs_align align;
	struct webvtt_cs_vertical_t _vertical;
	struct webvtt_cs_line_t _line;
	struct webvtt_cs_position_t _position;
	struct webvtt_cs_size_t _size;
	struct webvtt_cs_align_t _align;
};

struct
cue
{
	webvtt_timestamp_t from;
	webvtt_timestamp_t until;
	webvtt_cue_settings settings;
	webvtt_string id;
	webvtt_string payload;
	cue_settings _settings;
};

#endif