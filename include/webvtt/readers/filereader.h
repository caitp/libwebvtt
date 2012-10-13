#ifndef __WEBVTT_CFILEREADER_H__
#	define __WEBVTT_CFILEREADER_H__
#	include "../parser.h"
#	include "../reader.h"
webvtt_status webvtt_filereader_open( webvtt_reader *ppreader, const char *path );
#	if WEBVTT_OS_WIN32 || WEBVTT_OS_WINCE
webvtt_status webvtt_filereader_wopen( webvtt_reader *ppreader, const wchar_t *path );
#	endif

#endif