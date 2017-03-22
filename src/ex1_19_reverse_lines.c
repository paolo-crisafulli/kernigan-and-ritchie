#include <stdio.h>
#include <stdbool.h>

#include "lines.h"

size_t reverse_lines_buffer_size = 100;

static void reverse(char string[], int str_len) {
	int i;
	char c;
	for (i = 0; i < str_len / 2; i++) {
		c = string[i];
		string[i] = string[str_len - 1 - i];
		string[str_len - 1 - i] = c;
	}
}

static void recursively_print_reversed_line_tail(FILE *stream_in,
		FILE *stream_out,
bool *preached_eof) {
	char buffer[reverse_lines_buffer_size];
	int result_len;
	bool reached_eol;
	get_line_chunk(stream_in, buffer, reverse_lines_buffer_size, &result_len,
			&reached_eol,
			preached_eof);
	if (!reached_eol) {
		recursively_print_reversed_line_tail(stream_in, stream_out,
				preached_eof);
	}
	reverse(buffer, result_len);
	fprintf(stream_out, "%s", buffer);
}

static bool get_line_head(char buffer[], size_t buffer_size,
		FILE* stream_in, int* presult_len, bool* preached_eol,
		bool* preached_eof) {
	return get_line_chunk(stream_in, buffer, buffer_size, presult_len,
			preached_eol, preached_eof);
}

void reverse_lines(FILE *stream_in, FILE *stream_out) {
	char buffer[reverse_lines_buffer_size];
	int result_len;
	bool reached_eol;
	bool reached_eof;
	while (get_line_head(buffer, reverse_lines_buffer_size, stream_in, &result_len,
			&reached_eol, &reached_eof)) {
		if (!reached_eol) {
			recursively_print_reversed_line_tail(stream_in, stream_out,
					&reached_eof);
		}
		reverse(buffer, result_len);
		fprintf(stream_out, "%s%s", buffer, reached_eof ? "" : "\n");
	}
}
