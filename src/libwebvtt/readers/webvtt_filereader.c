#include <webvtt/readers/filereader.h>
#include <stdio.h>
#include <malloc.h>
#include <errno.h>

static void WEBVTT_CALLBACK file_release( webvtt_reader *pself );
static void WEBVTT_CALLBACK file_close( webvtt_reader self );
static webvtt_uint32 WEBVTT_CALLBACK file_read( webvtt_reader self, webvtt_byte *buffer, webvtt_uint32 length );
static webvtt_bool WEBVTT_CALLBACK file_is_end( webvtt_reader self );
static const struct webvtt_reader_t
webvtt_reader_file =
{
	&file_release,
	&file_close,
	&file_read,
	&file_is_end
};

typedef struct webvtt_filereader_t *webvtt_filereader, webvtt_filereader_t;

struct
webvtt_filereader_t
{
	webvtt_reader_t base;
	FILE *fh;
} ;

webvtt_status
webvtt_filereader_open( webvtt_reader *ppreader, const char *path )
{
	FILE *fh = fopen( path, "rb" );
	webvtt_filereader reader;
	if( !fh )
	{
		const char *error = strerror(errno);
		return WEBVTT_READ_ERROR;
	}
	if ( (reader = calloc( 1, sizeof(*reader) ) ) == NULL )
	{
		fclose( fh );
		return WEBVTT_OUT_OF_MEMORY;
	}
	reader->base = webvtt_reader_file;
	reader->fh = fh;
	*ppreader = reader;
	return WEBVTT_SUCCESS;
}

webvtt_status webvtt_filereader_wopen( webvtt_reader *ppreader, const wchar_t *path )
{
#	if WEBVTT_OS_WIN32 || WEBVTT_OS_WINCE
	FILE *fh = _wfopen( path, L"rb" );
	webvtt_filereader reader;
	if( !fh )
	{
		const char *error = strerror(errno);
		return WEBVTT_READ_ERROR;
	}
	if ( (reader = calloc( 1, sizeof(*reader) ) ) == NULL )
	{
		fclose( fh );
		return WEBVTT_OUT_OF_MEMORY;
	}
	reader->base = webvtt_reader_file;
	reader->fh = fh;
	*ppreader = reader;
	return WEBVTT_SUCCESS;
#	else
	return WEBVTT_NOT_SUPPORTED;
#	endif
}

static void WEBVTT_CALLBACK
file_release( webvtt_reader *pself )
{
	if( pself && *pself )
	{
		webvtt_filereader me = *(webvtt_filereader*)pself;
		*pself = 0;
		
		/**
		 * ensure closed before destroying
		 */
		me->base.close( &me->base );
	
		free( me );
	}
}

static void WEBVTT_CALLBACK
file_close( webvtt_reader self )
{
	if( self )
	{
	  webvtt_filereader me = (webvtt_filereader)self;
	  if( me->fh )
	  {
		FILE *fh = me->fh;
		me->fh = 0;
		fclose( fh );
	  }
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
		nread += fread( buffer, length, 1, me->fh );
	}
	return nread;
}

static webvtt_bool WEBVTT_CALLBACK
file_is_end( webvtt_reader self )
{
	webvtt_filereader me = (webvtt_filereader)self;
	return !me || !me->fh || feof( me->fh ) != 0;
}