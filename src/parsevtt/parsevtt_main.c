#include <webvtt/parser.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

static int WEBVTT_CALLBACK
error( void *userdata, webvtt_uint line, webvtt_uint col, webvtt_error errcode )
{
	fprintf(stderr, "`%s' at %u:%u -- error: %s\n", (const char *)userdata, line, col, webvtt_strerror( errcode ) );
	return 0; /* Die on all errors */
}

static void WEBVTT_CALLBACK 
cue( void *userdata, webvtt_cue cue )
{
	/**
	 * Do nothing
	 */
	webvtt_delete_cue( &cue );
}

int
main( int argc, char **argv )
{
	const char *input_file = 0;
	int status = 0;
	webvtt_status result;
	webvtt_parser vtt;
	FILE *fh;
	int i;
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
						input_file = p;
					}
					else if( i+1 < argc )
					{
						input_file = argv[i+1];
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
	if( !input_file )
	{
		fprintf( stderr, "error: missing input file.\n\nUsage: parsevtt -f <vttfile>\n" );
		return 1;
	}
	
	fh = fopen(input_file,"rb");
	if( !fh )
	{
		fprintf( stderr, "error: failed to open `%s': %s\n", input_file, strerror(errno) );
		return 1;
	}
	
	if( ( result = webvtt_create_parser( &cue, &error, input_file, &vtt ) ) != WEBVTT_SUCCESS )
	{
		fprintf( stderr, "error: failed to create VTT parser.\n" );
		fclose( fh );
		return 1;
	}
	
	/**
	 * Try to parse the file.
	 */
	do
	{
		char buffer[0x1000];
		webvtt_uint n_read = (webvtt_uint)fread( buffer, 1, sizeof(buffer), fh );
		if( !n_read && feof( fh ) )
			break; /* Read the file successfully */
		result = webvtt_parse_chunk( vtt, buffer, n_read );
			
		if( result == WEBVTT_PARSE_ERROR )
		{
			/**
			 * TODO:
			 * Acquire some detailed information from the parser (Line number in input file,
			 * column number, specific error
			 */
			fprintf( stderr, "error: parse error\n" );
			return 1;
		}
	} while( result == WEBVTT_SUCCESS );
	webvtt_delete_parser( vtt );
	fclose( fh );
	return status;
}