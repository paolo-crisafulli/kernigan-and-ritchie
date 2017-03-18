#include <stdio.h>
#include <stdbool.h>
#include "lines.h"

bool getline_chunk(FILE *stream, char result[], size_t maxlen,
		int* presult_len, bool *preached_eol, bool *preached_eof) {
	int c, len = 0;
	while (len < maxlen - 1 && (c = fgetc(stream)) != EOF && c != '\n') {
		result[len] = c;
		len++;
	}

	*preached_eof = (c == EOF);

	*preached_eol = (c == '\n') || *preached_eof;

	*presult_len = len;

	result[len] = '\0';

	if (c == EOF && len == 0)
		return false;
	else
		return true;
}
