libwebvtt - The library for interpreting and authoring conformant WebVTT content

TODO:
- *** IMPORTANT *** Add additional API functions for allocating and cleaning up
  Cue objects
- Reference counting objects (may be a hassle, but would help to prevent leaking
  dropped cue objects)  
- Improvements to lexer/parser state machines, in terms of speed and conformance
- Default error handlers when not provided by user
- Internationalized error messages
- Shared-library compilation
