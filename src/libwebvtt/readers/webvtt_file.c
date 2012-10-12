#include <webvtt/readers/filereader.h>
#include <stdio.h>

static void WEBVTT_CALLBACK file_close( webvtt_reader self );
static webvtt_uint32 WEBVTT_CALLBACK file_read( webvtt_reader self, webvtt_byte *buffer, webvtt_uint32 length );

static struct webvtt_reader_t webvtt_reader_file =
{
	&file_close,
	&file_read
};

typedef struct
webvtt_filereader_t
{
	webvtt_reader_t base;
	FILE *fh;
} *webvtt_filereader;

webvtt_status
webvtt_filereader_open( webvtt_reader *ppreader, const char *path )
{
	FILE *fh = fopen( path, "rb" );
}

#	if WEBVTT_OS_WIN32 || WEBVTT_OS_WINCE
webvtt_status webvtt_filereader_wopen( webvtt_reader *ppreader, const wchar_t *path )
{
}
#	endif

static void WEBVTT_CALLBACK
file_close( webvtt_reader self )
{
}

static webvtt_uint32 WEBVTT_CALLBACK
file_read( webvtt_reader self,
			webvtt_byte *buffer,
			webvtt_uint32 length )
{
	webvtt_uint32 nread = 0;
	webvtt_filereader me = (webvtt_filereader)self;
	if( me && me->fh )
	{
		nread += fread( fh, buffer, length );
		if( nread == 0 )
		{
		}
	}
	return nread;
}