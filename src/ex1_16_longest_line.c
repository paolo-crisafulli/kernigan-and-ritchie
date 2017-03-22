#include <stdio.h>
#include <stdbool.h>
#include "lines.h"

#define MAXLINE 1000 /* maximum input line length */
const int NUM_LEADING_CHARS_WITH_ELLIPSIS = 6;

size_t longest_line_buffer_size = 100;

int consume_end_of_line(FILE *stream_in, size_t buffer_size, bool *preached_eol) {
	char garbage[buffer_size];
	bool reached_eof;
	int len;
	get_line_chunk(stream_in, garbage, buffer_size, &len, preached_eol,
			&reached_eof);
	return len;
}

void replace_string_end_with_ellipsis(char string[], int str_len) {
	if (NUM_LEADING_CHARS_WITH_ELLIPSIS >= 4 /* save leading char (at least) */
	&& str_len > NUM_LEADING_CHARS_WITH_ELLIPSIS) {
		/* finish with "..." */
		string[str_len - 2] = string[str_len - 3] = string[str_len - 4] = '.';
	}
}

bool get_first_line_chunk(FILE *stream_in, char line[], size_t maxlen,
		int* plen, bool *preached_eol) {
	bool reached_eof;
	return get_line_chunk(stream_in, line, maxlen, plen, preached_eol,
			&reached_eof);
}

bool get_line(FILE *stream_in, char line[], size_t maxlen, int *plen) {
	int len;
	bool reached_eol;
	if (!get_first_line_chunk(stream_in, line, maxlen, &len, &reached_eol))
		return false;

	int total_len = len; /* current line length */

	while (!reached_eol) {
		total_len += consume_end_of_line(stream_in, maxlen, &reached_eol);
	}

	*plen = total_len;

	return true;
}

/* copy: copy 'from' into 'to'; assume 'to' is big enough */
void copy(char to[], char from[]) {
	int i;
	i = 0;
	while ((to[i] = from[i]) != '\0')
		++i;
}

/* print the longest input line */
void print_longest_line(FILE *stream_in, FILE *stream_out) {
	int len; /* current line length */
	int max; /* maximum length seen so far */
	char line[longest_line_buffer_size]; /* current input line */
	char longest[longest_line_buffer_size]; /* longest line saved here */
	longest[0] = '\0';
	max = 0;
	while (get_line(stream_in, line, longest_line_buffer_size, &len)) {
		if (len > max) {
			max = len;
			copy(longest, line);
		}
	}
	if (max >= longest_line_buffer_size) {
		replace_string_end_with_ellipsis(longest, longest_line_buffer_size);
	}

	/* works also for empty input */
	fprintf(stream_out, "%d (\"%s\")\n", max, longest);
}
