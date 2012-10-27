libwebvtt - The library for interpreting and authoring conformant WebVTT content
Available from https://github.com/caitp/libwebvtt

Routines available to application:

# Parser object routines
webvtt_status webvtt_create_parser( webvtt_cue_fn_ptr, webvtt_error_fn_ptr,
	void *, webvtt_parser * );
void webvtt_delete_parser( webvtt_parser );
webvtt_status webvtt_parse_chunk( webvtt_parser, const void *, webvtt_uint );

# String object routines
webvtt_status webvtt_string_new( webvtt_uint32, webvtt_string * );
void webvtt_string_delete( webvtt_string );
webvtt_status webvtt_string_append_utf8( webvtt_string *, const webvtt_byte *,
	webvtt_uint *, webvtt_uint, webvtt_utf8_reader );

# Memory allocation routines
void *webvtt_alloc( webvtt_length );
void *webvtt_alloc0( webvtt_length );
void webvtt_free( void * );
void webvtt_set_allocator( webvtt_alloc_fn_ptr, webvtt_free_fn_ptr, void * );

# Error handling
const char *webvtt_strerror( webvtt_error );

TODO:
- *** IMPORTANT *** Add additional API functions for allocating and cleaning up
  Cue objects
- Reference counting objects (may be a hassle, but would help to prevent leaking
  dropped cue objects)  
- Improvements to lexer/parser state machines, in terms of speed and conformance
- Default error handlers when not provided by user
- Internationalized error messages
- Shared-library compilation
- Routines for generating conformant WebVTT text