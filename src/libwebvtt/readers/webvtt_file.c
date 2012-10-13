#include <webvtt/readers/filereader.h>
#include <stdio.h>
#include <malloc.h>

static void WEBVTT_CALLBACK file_close( webvtt_reader *pself );
static webvtt_uint32 WEBVTT_CALLBACK file_read( webvtt_reader self, webvtt_byte *buffer, webvtt_uint32 length );
static webvtt_bool WEBVTT_CALLBACK file_is_end( webvtt_reader self );
static struct webvtt_reader_t webvtt_reader_file =
{
	&file_close,
	&file_read,
	&file_is_end
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
	webvtt_filereader reader;
	if( !fh )
	{
		return WEBVTT_READ_ERROR;
	}
	if ( (reader = calloc( sizeof(*reader) ) ) == NULL )
	{
		fclose( fh );
		return WEBVTT_OUT_OF_MEMORY;
	}
	reader->base = webvtt_reader_file;
	reader->fh = fh;
}

webvtt_status webvtt_filereader_wopen( webvtt_reader *ppreader, const wchar_t *path )
{
#	if WEBVTT_OS_WIN32 || WEBVTT_OS_WINCE
	FILE *fh = _wfopen( path, "rb" );
	webvtt_filereader reader;
	if( !fh )
	{
		return WEBVTT_READ_ERROR;
	}
	if ( (reader = calloc( sizeof(*reader) ) ) == NULL )
	{
		fclose( fh );
		return WEBVTT_OUT_OF_MEMORY;
	}
	reader->base = webvtt_reader_file;
	reader->fh = fh;
#	else
	return WEBVTT_NOT_SUPPORTED;
#	endif
}

static void WEBVTT_CALLBACK
file_close( webvtt_reader *pself )
{
	if( pself && *pself )
	{
	  webvtt_filereader me = *(webvtt_filereader*)pself;
	  if( me->fh )
	  {
		fclose( me->fh );
	  }
	  free( me );
	  *pself = 0;
	}
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
	}
	return nread;
}

static webvtt_bool WEBVTT_CALLBACK
file_is_end( webvtt_reader self )
{
	webvtt_filereader me = (webvtt_filereader)self;
	return !me || !me->fh || feof( me->fh ) != 0;
}