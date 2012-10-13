#ifndef __WEBVTT_CREADER_H__
#	define __WEBVTT_CREADER_H__
#	include "util.h"
#	ifndef WEBVTT_NO_STDIO
#		include <stdio.h>
#	endif

typedef struct webvtt_reader_t *webvtt_reader, webvtt_reader_t;

/**
 * webvtt_reader_t
 * Object representing a reader for a single source of data.
 */
struct
webvtt_reader_t
{
	/**
	 * delete(self)
	 * deallocate the reader using the appropriate allocator / heap
	 */
	void (WEBVTT_CALLBACK *release)( webvtt_reader *pself );
	
	/**
	 * close(self)
	 * closes a stream, releasing all associated data.
	 */
	void (WEBVTT_CALLBACK *close)( webvtt_reader self );
	
	/**
	 * read(self, buffer, length)
	 * reads 'length' bytes to 'buffer' from 'self'
	 * returns the amount of bytes actually read.
	 */
	webvtt_uint32 (WEBVTT_CALLBACK *read)( webvtt_reader self, webvtt_byte *buffer, webvtt_uint32 length );
	
	/**
	 * is_end(self)
	 * returns non-zero if the end of the stream has been reached,
	 * otherwise zero.
	 */
	 webvtt_bool (WEBVTT_CALLBACK *is_end)( webvtt_reader self );
};


/**
 * Helpers
 */
#	define webvtt_reader_release( self )\
do {\
	webvtt_reader p = (webvtt_reader)(self);\
	if( p ) \
		p->release( &(self) );\
} while( 0 )

#	define webvtt_reader_close( self )\
do {\
	webvtt_reader p = (webvtt_reader)(self);\
	if( p ) \
		p->close( (self) );\
} while( 0 )

#endif