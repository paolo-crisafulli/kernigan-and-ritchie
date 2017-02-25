#include <stdio.h>
#include <stdbool.h>
#define MAXLINE 1000 /* maximum input line length */

bool getoneline(FILE *stream, char result[], int *result_len, int maxlen);
void copy(char to[], char from[]);

/* print the longest input line */
void print_longest_line_maxlen(FILE *stream_in, FILE *stream_out, int maxlen) {
	int len; /* current line length */
	int max; /* maximum length seen so far */
	char line[maxlen]; /* current input line */
	char longest[maxlen]; /* longest line saved here */
	longest[0] = '\0';
	max = 0;
	while (getoneline(stream_in, line, &len, maxlen)) {
		if (len > max) {
			max = len;
			copy(longest, line);
		}
	}
	/* works also for empty inpout */
	fprintf(stream_out, "%d (\"%s\")\n", max, longest);
}

void print_longest_line(FILE *stream_in, FILE *stream_out) {
	print_longest_line_maxlen(stream_in, stream_out, MAXLINE);
}

/* getoneline: read a line into "result", return length */
bool getoneline(FILE *stream, char result[], int *result_len, int maxlen) {
	int c, len = 0;
	while ((c = fgetc(stream)) != EOF && c != '\n') {
		if (len < maxlen - 1)
			result[len] = c;
		len++;
	}
	if (c == EOF && len == 0)
		return false;

	*result_len = len;

	if (len < maxlen) {
		result[len] = '\0';
	} else {
		result[maxlen - 1] = '\0';
		if (maxlen > 4) /* save leading char (at least); finish with "..." */
			result[maxlen - 2] = result[maxlen - 3] = result[maxlen - 4] = '.';
	}

	return true;
}

/* copy: copy 'from' into 'to'; assume 'to' is big enough */
void copy(char to[], char from[]) {
	int i;
	i = 0;
	while ((to[i] = from[i]) != '\0')
		++i;
}
