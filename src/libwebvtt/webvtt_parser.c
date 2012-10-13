#include <webvtt/parser.h>
#include <malloc.h>

#ifdef WEBVTT_MIN_BUFFER_SIZE
#	undef WEBVTT_MIN_BUFFER_SIZE
#endif
#define WEBVTT_MIN_BUFFER_SIZE 0x80
#if !defined(WEBVTT_BUFFER_SIZE) || WEBVTT_BUFFER_SIZE < WEBVTT_MIN_BUFFER_SIZE
#	define WEBVTT_BUFFER_SIZE (0x200)
#endif

typedef enum
webvtt_state_t
{
	INITIAL = 0,
} webvtt_state;

struct
webvtt_parser_t
{
	/**
	 * TODO: use atomic operations where available
	 */
	webvtt_uint refs;
	
	webvtt_reader reader;
	webvtt_flags flags;
	webvtt_state state;
	
	/**
	 * UTF-8 parsing
	 */
	webvtt_uint32 uc;
	webvtt_uint32 need;

	/**
	 * character buffer
	 */
	webvtt_byte buffer[WEBVTT_BUFFER_SIZE];
	webvtt_uint32 position;
	webvtt_uint32 filled; /* bytes in the buffer that have been filled. */
	webvtt_uint32 signal;

	/**
	 * working cue
	 */
	webvtt_cue wc;
	
	/**
	 * finished cue
	 * reset to NULL after retrieved.
	 */
	webvtt_cue finished_cue;
} webvtt_parser_t;

static webvtt_status webvtt_parse_header( webvtt_parser self );

webvtt_status
webvtt_parser_new( webvtt_flags flags, webvtt_reader preader, webvtt_parser *pparser )
{
	webvtt_parser me;
	
	if( !preader || !pparser )
	{
		return WEBVTT_INVALID_PARAM;
	}
	
	me = calloc( 1, sizeof *me );
	if( !me )
	{
		return WEBVTT_OUT_OF_MEMORY;
	}
	
	me->refs = 1;
	
	/**
	 * TODO:
	 * Sanity checks on flags
	 */
	me->flags = flags;
	me->reader = preader;
	*pparser = me;
	return WEBVTT_SUCCESS;
}

void
webvtt_parser_ref( webvtt_parser self )
{
	if( self )
	{
		self->refs++;
	}
}

void
webvtt_parser_release( webvtt_parser *pparser )
{
	if( pparser && *pparser )
	{
		webvtt_parser me = *pparser;
		*pparser = 0;
		if( --( me->refs ) == 0 )
		{
			/**
			 * close reader
			 */
			webvtt_reader_close( me->reader );
		
			/**
			 * delete finished_cue if present
			 */
		
			/**
			 * delete working cue
			 */
		 
			/**
			 * delete parser
			 */
			free( me );
		}
	}
}

webvtt_status
webvtt_parser_get_cue( webvtt_parser self, webvtt_cue *pcue )
{
	webvtt_status status = WEBVTT_INVALID_PARAM;
	if( self && pcue )
	{
		webvtt_status s = WEBVTT_PARSE_ERROR;
		if( self->finished_cue )
		{
			*pcue = self->finished_cue;
			self->finished_cue = 0;
			status = WEBVTT_SUCCESS;
		}
		
		do
		{
			if( self->position >= self->filled )
			{
				if( ( self->flags & WEBVTT_WAIT_SIGNAL ) && !self->signal )
				{
					s = WEBVTT_NEED_MORE_DATA;
					break;
				}
				else
				{
					if( self->signal > 0 )
					{
						--self->signal;
					}
					self->position = 0;
					self->filled = self->reader->read( self->reader, self->buffer, 
						sizeof(self->buffer) );
					if( self->filled == 0 && self->reader->is_end( self->reader ) )
					{
						return WEBVTT_END_OF_STREAM;
					}
				}
			}
			
			switch( self->state )
			{
				case INITIAL:
				{
					s = webvtt_parse_header( self );
				}
				break;
			}
		} while( self->finished_cue == NULL );
		
		if( status != WEBVTT_SUCCESS )
		{
			/**
			 * return the parsing status
			 */
			status = s;
		}
	}
	return status;
}

void
webvtt_parser_get_error( webvtt_parser parser )
{
}

/**
 * Parsing implementation
 */
 static webvtt_status
 webvtt_parse_header( webvtt_parser self )
 {
	
	return WEBVTT_PARSE_ERROR;
 }