#include <webvtt/error.h>

static const char *errstr[] =
{
	/* WEBVTT_ALLOCATION_FAILED */ "error allocating object",
	/* WEBVTT_MALFORMED_TAG */ "malformed 'WEBVTT' tag",
	/* WEBVTT_EXPECTED_EOL */ "expected newline",
	/* WEBVTT_EXPECTED_WHITESPACE */ "expected whitespace",
	/* WEBVTT_LONG_COMMENT */ "very long tag-comment",
	/* WEBVTT_ID_TRUNCATED */ "webvtt-cue-id truncated",
	/* WEBVTT_MALFORMED_TIMESTAMP */ "malformed webvtt-timestamp",
	/* WEBVTT_EXPECTED_TIMESTAMP */ "expected webvtt-timestamp",
	/* WEBVTT_MISSING_CUETIME_SEPARATOR */ "missing webvtt-cuetime-separator `-->'",
	/* WEBVTT_MISSING_CUESETTING_DELIMITER */ "missing whitespace before webvtt-cuesetting",
	/* WEBVTT_INVALID_CUESETTING */ "unrecognized webvtt-cue-setting",
	/* WEBVTT_VERTICAL_ALREADY_SET */ "'vertical' cue-setting already used",
	/* WEBVTT_VERTICAL_BAD_VALUE */ "'vertical' setting must have a value of either 'lr' or 'rl'",
	/* WEBVTT_LINE_ALREADY_SET */ "'line' cue-setting already used",
	/* WEBVTT_LINE_BAD_VALUE */ "'line' cue-setting must have a value that is an integer (signed) line number, or percentage (%) from top of video display",
	/* WEBVTT_POSITION_ALREADY_SET */ "'position' cue-setting already used",
	/* WEBVTT_POSITION_BAD_VALUE */ "'position' cue-setting must have a positive integer value"
	/* WEBVTT_SIZE_ALREADY_SET */ "'size' cue-setting already used",
	/* WEBVTT_SIZE_BAD_VALUE */ "'size' cue-setting must have percentage (%) value",
	/* WEBVTT_ALIGN_ALREADY_SET */ "'align' cue-setting already used",
	/* WEBVTT_ALIGN_BAD_VALUE */ "'align' cue-setting must have a value of either 'start', 'middle', or 'end'",
};

/**
 * TODO:
 * Add i18n localized error strings with support for glibc and msvcrt locale identifiers
 * (This might be too much work!)
 */
const char *
webvtt_strerror( webvtt_error errno )
{
	if( errno < 0 || errno >= (sizeof(errstr) / sizeof(*errstr)) )
	{
		return "";
	}
	return errstr[ errno ];
}