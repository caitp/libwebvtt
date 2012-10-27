#include "internal.h"

webvtt_status
webvtt_create_cue( webvtt_cue *pcue )
{
	if( !pcue )
	{
		return WEBVTT_INVALID_PARAM;
	}
	*pcue = webvtt_alloc0( sizeof(cue) );
	if( !*pcue )
	{
		return WEBVTT_OUT_OF_MEMORY;
	}
	return WEBVTT_SUCCESS;
}

void
webvtt_delete_cue( webvtt_cue *pcue )
{
	if( pcue && *pcue )
	{
		struct cue *cue = (struct cue *)*pcue;
		*pcue = 0;
		webvtt_string_delete( cue->id );
		webvtt_string_delete( cue->payload );
		webvtt_free( cue );
	}
}

webvtt_cs_align
webvtt_create_cue_align( webvtt_cue cue )
{
	if( cue )
	{
		if( !cue->settings )
		{
			cue->settings = (webvtt_cue_settings)&((struct cue *)cue)->_settings;
		}
		if( !cue->settings->align )
		{
			cue->settings->align = &((struct cue *)cue)->_settings._align;
		}
		return cue->settings->align;
	}
	return 0;
}

webvtt_cs_vertical
webvtt_create_cue_vertical( webvtt_cue cue )
{
	if( cue )
	{
		if( !cue->settings )
		{
			cue->settings = (webvtt_cue_settings)&((struct cue *)cue)->_settings;
		}
		if( !cue->settings->vertical )
		{
			cue->settings->vertical = &((struct cue *)cue)->_settings._vertical;
		}
		return cue->settings->vertical;
	}
	return 0;
}

webvtt_cs_line
webvtt_create_cue_line( webvtt_cue cue )
{
	if( cue )
	{
		if( !cue->settings )
		{
			cue->settings = (webvtt_cue_settings)&((struct cue *)cue)->_settings;
		}
		if( !cue->settings->line )
		{
			cue->settings->line = &((struct cue *)cue)->_settings._line;
		}
		return cue->settings->line;
	}
	return 0;
}

webvtt_cs_size
webvtt_create_cue_size( webvtt_cue cue )
{
	if( cue )
	{
		if( !cue->settings )
		{
			cue->settings = (webvtt_cue_settings)&((struct cue *)cue)->_settings;
		}
		if( !cue->settings->size )
		{
			cue->settings->size = &((struct cue *)cue)->_settings._size;
		}
		return cue->settings->size;
	}
	return 0;
}

webvtt_cs_position
webvtt_create_cue_position( webvtt_cue cue )
{
	if( cue )
	{
		if( !cue->settings )
		{
			cue->settings = (webvtt_cue_settings)&((struct cue *)cue)->_settings;
		}
		if( !cue->settings->position )
		{
			cue->settings->position = &((struct cue *)cue)->_settings._position;
		}
		return cue->settings->position;
	}
	return 0;
}