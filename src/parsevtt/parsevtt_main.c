#include <webvtt/readers/filereader.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

typedef struct
state_t
{
	const char *input_file;
	webvtt_reader reader;
	webvtt_parser parser;
} state_t, *state;

int
main( int argc, char **argv )
{
	state_t s;
	int status = 0;
	webvtt_status result;
	int i;
	memset( &s, 0, sizeof s );
	for( i = 0; i < argc; ++i )
	{
		const char *a = argv[i];
		if( *a == '-' )
		{
			switch( a[1] )
			{
				case 'f':
				{
					const char *p = a+2;
					while( isspace(*p) ) ++p;
					if( *p )
					{
						s.input_file = p;
					}
					else if( i+1 < argc )
					{
						s.input_file = argv[i+1];
						++i;
					}
					else
					{
						fprintf( stderr, "error: missing parameter for switch `-f'\n" );
					}
				} break;
				
				case '?':
				{
					fprintf( stdout, "Usage: parsevtt -f <vttfile>\n" );
					return 0;
				} break;
			}
		}
	}
	if( !s.input_file )
	{
		fprintf( stderr, "error: missing input file.\n\nUsage: parsevtt -f <vttfile>\n" );
		return 1;
	}
	
	if( ( result = webvtt_filereader_open( &s.reader, s.input_file ) ) != WEBVTT_SUCCESS )
	{
		const char *reason = "(reason unknown)";
		switch( result )
		{
			case WEBVTT_OUT_OF_MEMORY: reason = "(out of memory)"; break;
			case WEBVTT_INVALID_PARAM: reason = "(invalid parameter in call to webvtt_filereader_open())"; break;
			case WEBVTT_NOT_SUPPORTED: reason = "(filereader not supported)"; break;
		}
		fprintf( stderr, "error: failed to open VTT stream `%s' %s.\n", s.input_file, reason );
		return 1;
	}
	
	if( ( result = webvtt_parser_new( WEBVTT_NOWAIT, s.reader, &s.parser ) ) != WEBVTT_SUCCESS )
	{
		fprintf( stderr, "error: failed to create VTT parser.\n" );
		webvtt_reader_release( s.reader );
		return 1;
	}
	
	/**
	 * Try to parse the file.
	 */
	do
	{
		webvtt_cue cue;
		result =
			webvtt_parser_get_cue( s.parser, &cue );
			
		switch( result )
		{
			case WEBVTT_PARSE_ERROR:
				/**
				 * TODO:
				 * Acquire some detailed information from the parser (Line number in input file,
				 * column number, specific error
				 */
				fprintf( stderr, "error: parse error\n" );
				status = 1;
			break;
		}
		
	} while( result == WEBVTT_SUCCESS );
	webvtt_parser_release( &s.parser );
	webvtt_reader_release( s.reader );
	return status;
}