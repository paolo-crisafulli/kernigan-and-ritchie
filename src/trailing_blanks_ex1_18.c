#include <stdio.h>
#include <stdbool.h>

#include "lines.h"

static bool is_blank_char(char c) {
	return c == ' ' || c == '\t';
}

static int trim_trailing_blanks(char line[], int len) {
	int i;
	for (i = len - 1; i >= 0; i--) {
		char c = line[i];
		if (is_blank_char(c)) {
			line[i] = '\0';
		} else {
			break;
		}
	}
	return i + 1;
}

static void print_line_without_trailing_blanks(FILE* stream_in,
		FILE *stream_out,
		size_t maxline, int *pline_len, bool *preached_eof) {
	char line[maxline];
	int total_len = 0;

	bool reached_eol;
	bool reached_eof;
	int len;
	/* detect end of file and exit, or get first block of line */
	if (!getline_chunk(stream_in, line, maxline, &len, &reached_eol,
			&reached_eof)) {
		*pline_len = 0;
		*preached_eof = true;
		return;
	}

	/* print first blocks of line as they are, we can't trim them */
	while (!reached_eol) {
		total_len += len;
		fprintf(stream_out, "%s", line);
		getline_chunk(stream_in, line, maxline, &len, &reached_eol,
				&reached_eof);
	}

	total_len += trim_trailing_blanks(line, len);

	/* print trimmed trailing block */
	fprintf(stream_out, "%s", line);

	*pline_len = total_len;

	*preached_eof = reached_eof;
}

static void print_new_line(FILE* stream_out) {
	fprintf(stream_out, "\n");
}

void remove_trailing_blanks(FILE *stream_in, FILE *stream_out, size_t maxline) {
	while (true) {
		int line_len;
		bool reached_eof;
		print_line_without_trailing_blanks(stream_in, stream_out, maxline,
				&line_len, &reached_eof);
		if (reached_eof)
			break; /* don't add a new line if there wasn't in the input */
		if (line_len == 0)
			continue; /* don't print blank lines */
		print_new_line(stream_out);
	}
}
