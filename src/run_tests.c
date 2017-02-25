/*
 * run_tests.c
 *
 *  Created on: 16 Feb 2017
 *      Author: paolo
 */

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "exercises.h"
#include "../unity-src/unity.h"

#define LF "\n"
#define EMPTY_LINE ""
#define LINE "line"
#define LONG_LINE "long line"
#define LONGER_LINE "longer line"
#define VERY_LONG_LINE "very long line"
#define VERY_VERY_LONG_LINE "very very long line"

#define MAX_LONGEST_LINE_OUTPUT_LEN 5000

FILE* create_input_stream(char input[]) {
	FILE* stream_in;
	int input_len = strlen(input);
	if (input_len > 0)
		stream_in = fmemopen(input, input_len, "r");
	else
		stream_in = fopen("/dev/null", "r");
	// open /dev/null
	return stream_in;
}

FILE* create_output_stream(char output[MAX_LONGEST_LINE_OUTPUT_LEN]) {
	return fmemopen(output, MAX_LONGEST_LINE_OUTPUT_LEN, "w");
}

void assert_print_longest_line(char input[], char expected_longest_line[]) {
	FILE* stream_in = create_input_stream(input);

	char output[MAX_LONGEST_LINE_OUTPUT_LEN];
	FILE* stream_out = create_output_stream(output);

	print_longest_line_maxlen(stream_in, stream_out, 12);

	fclose(stream_out); /* flush to output char buffer */
	printf("print_longest_line output: %s\n", output);

	static const size_t max_len = 100;
	char expected_len[max_len];
	snprintf(expected_len, max_len, "%zd", strlen(expected_longest_line));
	char* found_string = strstr(output, expected_len);
	bool found_expected_len = (found_string != NULL);

	static const size_t max_message_len = 1000;
	char message[max_message_len];
	snprintf(message, max_message_len,
			"Expected longest line: \"%s\" (of len %s), in input: \"%s\"",
			expected_longest_line, expected_len, input);

	TEST_ASSERT_TRUE_MESSAGE(found_expected_len, message);
}

void test_print_longest_line() {
	assert_print_longest_line(
	/**/
	EMPTY_LINE /**/
	,/**/
	EMPTY_LINE /**/
	);

	assert_print_longest_line(
	/**/
	LINE LF /**/
	LONG_LINE /**/
	,/**/
	LONG_LINE /**/
	/**/);

	assert_print_longest_line(
	/**/
	LONG_LINE LF /**/
	LINE /**/
	,/**/
	LONG_LINE /**/
	/**/);

	assert_print_longest_line(
	/**/
	LINE LF /**/
	LINE LF /**/
	LONG_LINE LF /**/
	LINE LF /**/
	LINE LF /**/
	,/**/
	LONG_LINE /**/
	);

	assert_print_longest_line(
	/**/
	EMPTY_LINE LF /**/
	LINE LF /**/
	LONG_LINE LF /**/
	LONGER_LINE LF /**/
	VERY_LONG_LINE LF /**/
	VERY_VERY_LONG_LINE /**/
	EMPTY_LINE LF /**/
	,/**/
	VERY_VERY_LONG_LINE /**/
	);
}

int main(int argc, char **argv) {
	UNITY_BEGIN();
	RUN_TEST(test_print_longest_line);
	return UNITY_END();
}
