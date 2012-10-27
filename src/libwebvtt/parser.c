#include "parser.h"
#include <string.h>
#define FULLSTOP (0x2E)

/**
 * parser modes
 */
enum
{
	M_BUFFER_TOKENS = 0,
	M_LINE_TOKENS,
	M_READ_LINE
};


/**
 * states
 */
enum parse_state_t
{
	T_INITIAL = 0, T_HEADER, T_CUES, T_TAG, T_TAGCOMMENT, T_TAGEOL, T_TAGEOL2, T_STARTTIME, T_PAYLOAD,
	T_CUEEOL, T_SEP1, T_SEP2, T_SEP3, T_PRESETTING, T_ENDTIME, T_CUEID, T_SETTING, T_VERTICAL, T_POSITION,
	T_LINE, T_SIZE, T_ALIGN, T_PREPAYLOAD
};


#define ASCII_0 (0x30)
#define ASCII_9 (0x39)
#define ASCII_ISDIGIT(c) ( ((c) >= ASCII_0) && ((c) <= ASCII_9) )
#define HYPHEN_MINUS (0x2D)
#define COLON  (0x3A)

#define SECS_PER_HOUR (3600)
#define SECS_PER_MINUTE (60)
#define SECS_PER_MILLI (0.001)
#define BUFFER (self->buffer + self->position)
#define MALFORMED_TIME ((webvtt_timestamp_t)-1.0)

static webvtt_int64 parse_int( const webvtt_byte **pb, int *pdigits );
static int parse_timestamp( webvtt_parser self, const webvtt_byte *b, webvtt_timestamp *result );

webvtt_status
webvtt_create_parser( webvtt_cue_fn_ptr on_read,
						webvtt_error_fn_ptr on_error, void *
						userdata,
						webvtt_parser *ppout )
{
	webvtt_parser p;
	if( !on_read || !on_error || !ppout )
	{
		return WEBVTT_INVALID_PARAM;
	}
	
	if( !(p = (webvtt_parser)webvtt_alloc0( sizeof *p ) ) )
	{
		return WEBVTT_OUT_OF_MEMORY;
	}
	
	p->read = on_read;
	p->error = on_error;
	p->line = 1;
	p->userdata = userdata;
	*ppout = p;
	
	return WEBVTT_SUCCESS;
}

void
webvtt_delete_parser( webvtt_parser self )
{
}

#define BEGIN_STATE(State) case State: {
#define END_STATE } break;
#define IF_TOKEN(Token,Actions) case Token: { Actions } break;
#define BEGIN_DFA switch(self->state) {
#define END_DFA }
#define BEGIN_TOKEN switch(token) {
#define END_TOKEN }
#define IF_TRANSITION(Token,State) if( token == Token ) { self->state = State;
#define ELIF_TRANSITION(Token,State) } else IF_TRANSITION(Token,State)
#define ENDIF }
#define ELSE } else {

static int find_newline( const webvtt_byte *buffer, webvtt_uint *pos, webvtt_uint len )
{
	while( *pos < len )
	{
		if( buffer[ *pos ] == CR || buffer[ *pos ] == LF )
		{
			return 1;
		} 
		else
		{ 
			(*pos)++;
		}
	}
	return -1;
}

/**
 * basic strnstr-ish routine
 */
int
find_bytes(const webvtt_byte *buffer, webvtt_uint len, const webvtt_byte *sbytes, webvtt_uint slen)
{
	webvtt_uint slen2 = slen - 1;
	do
	{
		if( *buffer == *sbytes && memcmp( buffer + 1, sbytes + 1, slen2) == 0 )
		{
			return 1;
		}
	} while( len-- >= slen && *buffer++ );
	return 0;
}

webvtt_status
webvtt_parse_chunk( webvtt_parser self, const void *buffer, webvtt_uint len )
{
	webvtt_token token;
	webvtt_uint pos = 0;
	while( pos < len )
	{
		switch( self->mode )
		{
#if 1 /* M_BUFFER_TOKENS */
			case M_BUFFER_TOKENS:
			{
				token = webvtt_lex( self, (webvtt_byte *)buffer, &pos, len );
				if( token == UNFINISHED )
				{
					return WEBVTT_UNFINISHED;
				}
				if( token == BADTOKEN )
				{
					return WEBVTT_PARSE_ERROR;
				}
				else
				{
					BEGIN_DFA
						BEGIN_STATE(T_INITIAL)
							/* Transition from T_INITIAL -> T_TAG if we find 'WEBVTT'. Otherwise, there's been an error. */
							IF_TRANSITION( WEBVTT, T_TAG )
								self->mode = M_READ_LINE;
							ELSE
								/* indicate that a valid tag has not been provided and this is not a valid WebVTT
									document */
								ERROR(WEBVTT_MALFORMED_TAG);
							ENDIF
						END_STATE

						BEGIN_STATE(T_TAGEOL)
							IF_TRANSITION(NEWLINE,T_TAGEOL2)
							ELSE
								ERROR(WEBVTT_EXPECTED_EOL);
								pos -= (self->token_pos-1);
								continue;
							ENDIF
						END_STATE

						BEGIN_STATE(T_TAGEOL2)
							IF_TRANSITION(NEWLINE,T_TAGEOL2)
							ELSE
								self->state = T_CUEID;
								self->mode = M_READ_LINE;
								pos -= (self->token_pos);
								continue;
							ENDIF
						END_STATE

						BEGIN_STATE(T_CUEID)
							IF_TRANSITION(NEWLINE,T_STARTTIME)
								self->mode = M_READ_LINE;
							ELSE
							ENDIF
						END_STATE

						BEGIN_STATE(T_TAG)
							IF_TRANSITION(NEWLINE,T_TAGEOL)
							ELSE
							ENDIF
						END_STATE

						BEGIN_STATE(T_PREPAYLOAD)
							IF_TRANSITION(NEWLINE,T_PAYLOAD)
								self->mode = M_READ_LINE;
							ELSE
								ERROR(WEBVTT_EXPECTED_EOL);
							ENDIF
						END_STATE

						BEGIN_STATE(T_PAYLOAD)
							IF_TRANSITION(NEWLINE,T_CUEEOL)
							ELSE
								ERROR(WEBVTT_EXPECTED_EOL);
							ENDIF
						END_STATE

						BEGIN_STATE(T_CUEEOL)
							IF_TRANSITION(NEWLINE,T_CUEEOL)
							ELSE
								self->mode = M_READ_LINE;
								self->mode = T_CUEID;
							ENDIF
						END_STATE

					END_DFA
					self->token[ self->token_pos = 0 ] = 0;
				}
			}
			break;
#endif
#if 1 /* M_LINE_TOKENS */
			case M_LINE_TOKENS:
			{
				webvtt_uint old_pos = self->line_pos;
				token = webvtt_lex( self, (webvtt_byte *)self->line_buffer->text, &self->line_pos, len );
				
				if( token == UNFINISHED || token == BADTOKEN )
				{
					if( self->line_pos == old_pos && (self->state == T_PRESETTING || self->state == T_SETTING ) )
					{
						self->state = T_PREPAYLOAD;
						self->mode = M_BUFFER_TOKENS;
						webvtt_bytearray_delete(&self->line_buffer);
						self->line_pos = 0;
						continue;
					}

					/**
					 * TODO: Notify application
					 */
					return WEBVTT_PARSE_ERROR;
				}
				else
				{
					BEGIN_DFA
						BEGIN_STATE(T_STARTTIME)
							if( token == TIMESTAMP )
							{
								if( !parse_timestamp(self, self->token, &self->cue->from ) )
								{
									ERROR(WEBVTT_MALFORMED_TIMESTAMP);
								}
								self->state = T_SEP1;
							}
							else
							{
								ERROR(WEBVTT_EXPECTED_TIMESTAMP);
							}
						END_STATE

						BEGIN_STATE(T_SEP1)
							IF_TRANSITION(WHITESPACE,T_SEP2)
							ELSE
								ERROR(WEBVTT_EXPECTED_WHITESPACE);
								IF_TRANSITION(SEPARATOR,T_SEP3)
								ENDIF
							ENDIF
						END_STATE

						BEGIN_STATE(T_SEP2)
							IF_TRANSITION(SEPARATOR,T_SEP3)
							ELSE
								ERROR(WEBVTT_MISSING_CUETIME_SEPARATOR);
								if( token == TIMESTAMP )
								{
									if( !parse_timestamp( self, self->token, &self->cue->until ) )
									{
										ERROR(WEBVTT_MALFORMED_TIMESTAMP);
									}
									self->state = T_PRESETTING;
								}
							ENDIF
						END_STATE

						BEGIN_STATE(T_SEP3)
							IF_TRANSITION(WHITESPACE,T_ENDTIME)
							ELSE
								ERROR(WEBVTT_EXPECTED_WHITESPACE);
								if( token == TIMESTAMP )
								{
									if( !parse_timestamp(self, self->token, &self->cue->until ) )
									{
										ERROR(WEBVTT_MALFORMED_TIMESTAMP);
									}
									self->state = T_PRESETTING;
								}
							ENDIF
						END_STATE

						BEGIN_STATE(T_ENDTIME)
							if( token == TIMESTAMP )
							{
								if( !parse_timestamp( self, self->token, &self->cue->until ) )
								{
									ERROR(WEBVTT_MALFORMED_TIMESTAMP);
								}
								self->state = T_PRESETTING;
							}
							else
							{
								ERROR(WEBVTT_EXPECTED_TIMESTAMP);
							}
						END_STATE

						BEGIN_STATE(T_PRESETTING)
							IF_TRANSITION(WHITESPACE,T_SETTING)
							ELSE
								ERROR(WEBVTT_EXPECTED_WHITESPACE);
							ENDIF
						END_STATE

						BEGIN_STATE(T_SETTING)
							IF_TRANSITION(VERTICAL,T_VERTICAL)
							ELIF_TRANSITION(POSITION,T_POSITION)
							ELIF_TRANSITION(LINE,T_LINE)
							ELIF_TRANSITION(SIZE,T_SIZE)
							ELIF_TRANSITION(ALIGN,T_ALIGN)
							ELIF_TRANSITION(NEWLINE,T_PAYLOAD)
								webvtt_bytearray_delete(&self->line_buffer);
								self->mode = M_READ_LINE;
							ELSE
								ERROR(WEBVTT_INVALID_CUESETTING);
							ENDIF
						END_STATE

						BEGIN_STATE(T_VERTICAL)
							if( token == RL || token == LR )
							{
								self->cue->settings = (webvtt_cue_settings)&self->cue->_settings;
								if( self->cue->settings->vertical )
								{
									ERROR(WEBVTT_VERTICAL_ALREADY_SET);
								}
								self->cue->settings->vertical = &self->cue->_settings._vertical;
								self->cue->settings->vertical->value = token == RL ? WEBVTT_VERTICAL_RL : WEBVTT_VERTICAL_LR;
								self->state = T_PRESETTING;
							}
							else
							{
								ERROR(WEBVTT_VERTICAL_BAD_VALUE);
							}
						END_STATE

						BEGIN_STATE(T_POSITION)
							if( token == INTEGER )
							{
								const webvtt_byte *b = self->token;
								webvtt_int64 value;
								int digits;
								self->cue->settings = (webvtt_cue_settings)&self->cue->_settings;
								if( self->cue->settings->position )
								{
									ERROR(WEBVTT_POSITION_ALREADY_SET);
								}
								self->cue->settings->position = &self->cue->_settings._position;
								
								value = parse_int( &b, &digits );
								if( digits == 0 )
								{
									ERROR(WEBVTT_POSITION_BAD_VALUE);
								}
								self->cue->settings->position->value = (webvtt_uint)value;
								self->state = T_PRESETTING;
							}
							else
							{
								ERROR(WEBVTT_POSITION_BAD_VALUE);
							}
						END_STATE

						BEGIN_STATE(T_SIZE)
							if( token == PERCENTAGE )
							{
								const webvtt_byte *b = self->token;
								webvtt_int64 value;
								int digits;
								self->cue->settings = (webvtt_cue_settings)&self->cue->_settings;
								if( self->cue->settings->size )
								{
									ERROR(WEBVTT_SIZE_ALREADY_SET);
								}
								self->cue->settings->size = &self->cue->_settings._size;
								
								value = parse_int( &b, &digits );
								if( digits == 0 )
								{
									ERROR(WEBVTT_SIZE_BAD_VALUE);
								}
								self->cue->settings->size->value = (webvtt_uint)value;
								self->state = T_PRESETTING;
							}
							else
							{
								ERROR(WEBVTT_SIZE_BAD_VALUE);
							}
						END_STATE

						BEGIN_STATE(T_LINE)
							if( token == INTEGER || token == PERCENTAGE )
							{
								const webvtt_byte *b = self->token;
								webvtt_int64 value;
								int digits;
								self->cue->settings = (webvtt_cue_settings)&self->cue->_settings;
								if( self->cue->settings->line )
								{
									ERROR(WEBVTT_LINE_ALREADY_SET);
								}
								self->cue->settings->line = &self->cue->_settings._line;
								
								value = parse_int( &b, &digits );
								if( digits == 0 )
								{
									ERROR(WEBVTT_LINE_BAD_VALUE);
								}
								if( token == PERCENTAGE )
								{
									if( value < 0 )
									{
										ERROR(WEBVTT_LINE_BAD_VALUE);
									}
									else
									{
										self->cue->settings->line->is_percentage = 1;
										self->cue->settings->line->value.percentage = (webvtt_uint)value;
									}
								}
								else
								{
									self->cue->settings->line->is_percentage = 0;
									self->cue->settings->line->value.line = (webvtt_int)value;
								}
								self->state = T_PRESETTING;
							}
							else
							{
								ERROR(WEBVTT_LINE_BAD_VALUE);
							}
						END_STATE

						BEGIN_STATE(T_ALIGN)
							if( token == START || token == MIDDLE || token == END )
							{
								self->cue->settings = (webvtt_cue_settings)&self->cue->_settings;
								if( self->cue->settings->align )
								{
									ERROR(WEBVTT_ALIGN_ALREADY_SET);
								}
								self->cue->settings->align = &self->cue->_settings._align;
								if( token == START )
								{
									self->cue->settings->align->value = WEBVTT_ALIGN_START;
								}
								else if( token == MIDDLE )
								{
									self->cue->settings->align->value = WEBVTT_ALIGN_MIDDLE;
								}
								else
								{
									self->cue->settings->align->value = WEBVTT_ALIGN_END;
								}
								self->mode = T_PRESETTING;
							}
							else
							{
								ERROR(WEBVTT_ALIGN_BAD_VALUE);
							}
						END_STATE

					END_DFA
					self->token[ self->token_pos = 0 ] = 0;
				}
			}
			break;
#endif
#if 1 /* M_READ_LINE */
			case M_READ_LINE:
			{
				BEGIN_DFA
					BEGIN_STATE(T_TAG)
						if( !find_newline( (const webvtt_byte*)buffer, &pos, len ) )
						{
							ERROR(WEBVTT_LONG_COMMENT);
						}
						else
						{	/* We found the new line, so return to buffer parsing mode */
							self->mode = M_BUFFER_TOKENS;
						}
					END_STATE

					BEGIN_STATE(T_CUEID)
						int stat;
						if( !self->cue )
						{
							self->cue = (cue *)webvtt_alloc0( sizeof *(self->cue) );
							if( !self->cue )
							{
								ERROR(WEBVTT_ALLOCATION_FAILED);
							}
						}
						if( !self->line_buffer )
						{
							if( webvtt_bytearray_new( 0x80, &self->line_buffer ) != WEBVTT_SUCCESS )
							{
								ERROR(WEBVTT_ALLOCATION_FAILED);
							}
							self->truncate = 0;
							self->line_pos = 0;
						}
						if( (stat = webvtt_bytearray_getline( &self->line_buffer, (const webvtt_byte*)buffer,
							&pos, len, self->state == T_CUEID ? &self->truncate : 0 )) < 0 )
						{
							/**
							 * Allocation failed, so...
							 */
							ERROR(WEBVTT_ALLOCATION_FAILED);
						}
						if( self->truncate )
						{
							/**
							 * Let the application decide if we should give up now.
							 */
							ERROR(WEBVTT_ID_TRUNCATED);
						}
						if( stat )
						{
							if(  find_bytes( (const webvtt_byte *)self->line_buffer->text, self->line_buffer->length, (const webvtt_byte *)"-->", 3 ) )
							{
								self->mode = M_LINE_TOKENS;
								self->state = T_STARTTIME;
							}
							else
							{
								webvtt_uint pos = 0;
								webvtt_string_new( self->line_buffer->length, &self->cue->id );
								if( webvtt_string_append_utf8( &self->cue->id, self->line_buffer->text, &pos, 
									self->line_buffer->length, 0 ) == WEBVTT_OUT_OF_MEMORY )
								{
									ERROR(WEBVTT_ALLOCATION_FAILED);
								}
								webvtt_bytearray_delete(&self->line_buffer);
								self->mode = M_BUFFER_TOKENS;
							}
						}
					END_STATE
					/**
					 * Save the payload
					 */
					BEGIN_STATE(T_PAYLOAD)
						int stat;
						if( !self->line_buffer )
						{
							if( webvtt_bytearray_new( 0x80, &self->line_buffer ) == WEBVTT_OUT_OF_MEMORY )
							{
								ERROR(WEBVTT_ALLOCATION_FAILED);
							}
							self->truncate = 0;
						}
						if( (stat = webvtt_bytearray_getline( &self->line_buffer, (const webvtt_byte*)buffer,
							&pos, len, self->state == T_CUEID ? &self->truncate : 0 )) < 0 )
						{
							/**
							 * Allocation failed, so...
							 */
							ERROR(WEBVTT_ALLOCATION_FAILED);
						}
						if( stat )
						{
							webvtt_uint pos = 0;
							webvtt_string_new( self->line_buffer->length, &self->cue->payload );
							if( webvtt_string_append_utf8( &self->cue->payload, self->line_buffer->text, &pos, 
								self->line_buffer->length, 0 ) == WEBVTT_OUT_OF_MEMORY )
							{
								ERROR(WEBVTT_ALLOCATION_FAILED);
							}
							webvtt_bytearray_delete(&self->line_buffer);

							/**
							 * Inform the user that we've read a cue
							 */
							self->read( self->userdata, (webvtt_cue)self->cue );
							self->cue = 0;

							self->mode = M_BUFFER_TOKENS;
							self->line_pos = 0;
						}
					END_STATE
				END_DFA
				self->token[ self->token_pos = 0 ] = 0;
			}
			break;
#endif
		}
	}

	if( self->cue )
	{
		return WEBVTT_UNFINISHED;
	}

	if( pos == len || token == UNFINISHED )
	{
		return WEBVTT_SUCCESS;
	}
	return WEBVTT_PARSE_ERROR;
}

/**
 * Get an integer value from a series of digits.
 */
static webvtt_int64
parse_int( const webvtt_byte **pb, int *pdigits )
{
	int digits = 0;
	webvtt_int64 result = 0;
	webvtt_int64 mul = 1;
	const webvtt_byte *b = *pb;
	while( *b )
	{
		webvtt_byte ch = *b;
		if( ASCII_ISDIGIT(ch) )
		{
			/**
			 * Digit character, carry on
			 */
			result = result * 10 + ( ch - ASCII_0 );
			++digits;
		}
		else if( mul == 1 && digits == 0 && ch == HYPHEN_MINUS )
		{
			mul = -1;
		}
		else
		{
			break;
		}
		++b;
	}
	*pb = b;
	if( pdigits )
	{
		*pdigits = digits;
	}
	return result * mul;
}

/**
 * Turn the token of a TIMESTAMP tag into something useful, and returns non-zero
 * returns 0 if it fails
 */
static int
parse_timestamp( webvtt_parser self, const webvtt_byte *b, webvtt_timestamp *result )
{
	int have_hours = 0;
	int digits;
	webvtt_int64 v[4];
    if (!ASCII_ISDIGIT(*b))
	{
		goto _malformed;
	}
	
	/* get sequence of digits */
	v[0] = parse_int( &b, &digits );

	/* assume v[0] contains hours if more or less than 2 digits, or value is greater than 59 */
	if (digits != 2 || v[0] > 59)
	{
		have_hours = 1;
	}
	
	/* fail if missing colon ':' character */
	if ( !*b || *b++ != COLON)
	{
		goto _malformed;
	}
	
	/* fail if end of data reached, or byte is not an ASCII digit */
	if (!ASCII_ISDIGIT(*b))
	{
		goto _malformed;
	}
	
	/* get another integer value, fail if digits is not equal to 2 */
    v[1] = parse_int( &b, &digits );
	if( digits != 2 )
	{
		goto _malformed;
	}
	
	/* if we already know there's an hour component, or if the next byte is a colon ':',
	   read the next value */
	if (have_hours || (*b == COLON))
	{
		if (!*b++ != COLON)
		{
			goto _malformed;
		}
        if (!*b || !ASCII_ISDIGIT(*b))
		{
			goto _malformed;
		}
		v[2] = parse_int( &b, &digits );
		if( digits != 2 )
		{
			goto _malformed;
		}
    }
	else
	{
		/* Otherwise, if there is no hour component, shift everything over */
		v[2] = v[1];
		v[1] = v[0];
		v[0] = 0;
    }

	/* collect the manditory seconds-frac component. fail if there is no FULL_STOP '.'
	   or if there is no ascii digit following it */
	if( *b++ != FULLSTOP || !ASCII_ISDIGIT(*b))
	{
		goto _malformed;
	}
	v[3] = parse_int( &b, &digits );
	if( digits != 3 )
	{
		goto _malformed;
	}
	
	/* Ensure that minutes and seconds are acceptable values */
    if( v[1] > 59 || v[2] > 59 )
	{
		goto _malformed;
	}
	
	*result = (webvtt_timestamp)( v[0] * SECS_PER_HOUR )
			+ ( v[1] * SECS_PER_MINUTE )
			+ v[2] 
			+ ( v[3] * SECS_PER_MILLI );
	
	return 1;
_malformed:
	ERROR(WEBVTT_MALFORMED_TIMESTAMP);
	return 0;
}