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
#include "../unity-src/unity.h"
#include "lines.h"

#define LF "\n"
#define EMPTY_LINE ""
#define LINE "line"
#define LONG_LINE "long line"
#define LONGER_LINE "longer line"
#define VERY_LONG_LINE "very long line"
#define VERY_VERY_LONG_LINE "very very long line"

#define MAX_FUN_OUTPUT_LEN 5000

extern void print_longest_line(FILE* stream_in, FILE* stream_out);
extern void remove_trailing_blanks(FILE* stream_in, FILE* stream_out);
extern void reverse_lines(FILE* stream_in, FILE* stream_out);
extern const int NUM_LEADING_CHARS_WITH_ELLIPSIS;

extern size_t longest_line_buffer_size;
extern size_t reverse_lines_buffer_size;
extern size_t trailing_blanks_buffer_size;

static FILE* create_input_stream(char input[]) {
	FILE* stream_in;
	int input_len = strlen(input);
	if (input_len > 0)
		stream_in = fmemopen(input, input_len, "r");
	else
		stream_in = fopen("/dev/null", "r");
	return stream_in;
}

static FILE* create_output_stream(char output[MAX_FUN_OUTPUT_LEN]) {
	return fmemopen(output, MAX_FUN_OUTPUT_LEN, "w");
}

static int set_value_and_get_initial(size_t *pvar, size_t value) {
	size_t initial = *pvar;
	*pvar = value;
	return initial;
}

static void restore_initial_value(size_t *pvar, size_t value) {
	*pvar = value;
}

static void call_function_under_test(void (*pfun_under_test)(FILE*, FILE*),
		char test_input[], size_t *pbuffer_size, size_t buffer_size,
		char fun_name[], char fun_output[]) {
	FILE* stream_in = create_input_stream(test_input);

	FILE* stream_out = create_output_stream(fun_output);

	size_t initial_value;
	initial_value = set_value_and_get_initial(pbuffer_size, buffer_size);

	(*pfun_under_test)(stream_in, stream_out);

	restore_initial_value(&trailing_blanks_buffer_size, initial_value);

	fclose(stream_out); /* flush to output char buffer */

	printf("Function \"%s\" output:\n%s<<\n", fun_name, fun_output);
}

static void expected_longest_len_as_string(char expected_len[], size_t size_len,
		char expected_longest_line[]) {
	snprintf(expected_len, size_len, "%zd", strlen(expected_longest_line));
}

static void compose_failure_message(char message[], size_t max_message_len,
		char expected_longest_line[], char expected_len[100], char input[]) {
	snprintf(message, max_message_len,
			"Expected longest line: \"%s\" (of len %s), in input: \"%s\"",
			expected_longest_line, expected_len, input);
}

static bool find_string_in_output(char output[MAX_FUN_OUTPUT_LEN],
		char expected_len[100]) {
	char* found_string = strstr(output, expected_len);
	bool found_expected_len = (found_string != NULL);
	return found_expected_len;
}

static void assert_print_longest_line(char input[],
		char expected_longest_line[]) {
	char output[MAX_FUN_OUTPUT_LEN];

	call_function_under_test(&print_longest_line, input,
			&longest_line_buffer_size, 100, "print_longest_line", output);

	char expected_len[100];
	expected_longest_len_as_string(expected_len, -1, expected_longest_line);

	bool found_expected_len = find_string_in_output(output, expected_len);

	static const size_t max_message_len = 1000;
	char message[max_message_len];
	compose_failure_message(message, max_message_len, expected_longest_line,
			expected_len, input);

	TEST_ASSERT_TRUE_MESSAGE(found_expected_len, message);
}

static void assert_print_longest_line_with_ellipsis(char input[],
		size_t buffer_size, bool expectsEllipsis) {
	char output[MAX_FUN_OUTPUT_LEN];

	call_function_under_test(&print_longest_line, input,
			&longest_line_buffer_size, buffer_size, "print_longest_line",
			output);

	bool found = find_string_in_output(output, "...");

	static const size_t max_message_len = 1000;
	char message[max_message_len];
	snprintf(message, max_message_len, "Expected %sellipsis in output: \"%s\"",
			(expectsEllipsis ? "" : "*no* "), output);

	TEST_ASSERT_TRUE_MESSAGE(expectsEllipsis == found, message);
}

static void test_print_longest_line_with_ellipsis() {
	assert_print_longest_line_with_ellipsis(/**/
	EMPTY_LINE,/**/
	2, false);

	/* output array too small to include leading char and ... */
	assert_print_longest_line_with_ellipsis(/**/
	LINE,/**/
	2, false);
	assert_print_longest_line_with_ellipsis(/**/
	LINE,/**/
	4, false);

	/* "line" fits in output array */
	assert_print_longest_line_with_ellipsis(/**/
	LINE,/**/
	5, false);

	/* nominal case: long string should be output with ellipsis */
	assert_print_longest_line_with_ellipsis(/**/
	VERY_VERY_LONG_LINE, /**/
	NUM_LEADING_CHARS_WITH_ELLIPSIS + 1, true);

	/* corner case: longest line is one char longer than the output
	 * size and the threshold above which we replace its end with
	 * an ellipsis */
	size_t longest_line_len = NUM_LEADING_CHARS_WITH_ELLIPSIS + 1;
	char input[longest_line_len + 1];
	int i;
	for (i = 0; i < longest_line_len; i++) {
		input[i] = 'a';
	}
	input[longest_line_len] = '\0';

	assert_print_longest_line_with_ellipsis(/**/
	input,/**/
	longest_line_len, true);
	assert_print_longest_line_with_ellipsis(/**/
	input,/**/
	longest_line_len + 1, false);
}

static void test_print_longest_line() {
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

#define FIVE_BLANKS " \t \t "
#define LINE_WITH_FIVE_BLANKS LINE FIVE_BLANKS

static void assert_remove_trailing_blanks(char input[], int buffer_size,
		char expected_output[]) {
	char output[MAX_FUN_OUTPUT_LEN];

	call_function_under_test(&remove_trailing_blanks, input,
			&trailing_blanks_buffer_size, buffer_size, "remove_trailing_blanks",
			output);

	static const size_t max_message_len = 1000;
	char message[max_message_len];
	snprintf(message, max_message_len,
			"Expected output was: \"%s\", got this instead: \"%s\"",
			expected_output, output);

	TEST_ASSERT_EQUAL_INT_MESSAGE(0, strcmp(expected_output, output), message);
}

static void test_remove_trailing_blanks() {
	char input[] = LINE_WITH_FIVE_BLANKS LF LINE_WITH_FIVE_BLANKS;
	char expected_output[] = LINE LF LINE;
	int buffer_len = 100;

	assert_remove_trailing_blanks(input, buffer_len, expected_output);
}

static void test_remove_blank_lines() {
	int buffer_len = 100;

	/* tests with blank lines at beginning of input */
	assert_remove_trailing_blanks(FIVE_BLANKS LF LINE LF LINE, buffer_len,
	LINE LF LINE);
	assert_remove_trailing_blanks(FIVE_BLANKS LF FIVE_BLANKS LF LINE LF LINE,
			buffer_len, LINE LF LINE);

	/* tests with blank lines in middle of input */
	assert_remove_trailing_blanks(LINE LF FIVE_BLANKS LF LINE, buffer_len,
	LINE LF LINE);
	assert_remove_trailing_blanks(LINE LF FIVE_BLANKS LF FIVE_BLANKS LF LINE,
			buffer_len, LINE LF LINE);

	/* tests with blank lines at end of input */
	assert_remove_trailing_blanks(LINE LF FIVE_BLANKS, buffer_len, LINE LF);
	assert_remove_trailing_blanks(LINE LF FIVE_BLANKS LF, buffer_len, LINE LF);
}

#define SPACE " "
#define FIVE_SPACES "     "
#define FOUR_SPACES "    "

static void test_remove_trailing_blanks_with_small_buffer() {

	assert_remove_trailing_blanks(LINE FIVE_SPACES, 2,
	LINE FIVE_SPACES);
	assert_remove_trailing_blanks(LINE FIVE_SPACES, 3,
	LINE FOUR_SPACES);
	assert_remove_trailing_blanks(LINE FIVE_SPACES, 4,
	LINE FIVE_SPACES);
	assert_remove_trailing_blanks(LINE FIVE_SPACES, 5,
	LINE FOUR_SPACES);
	assert_remove_trailing_blanks(LINE FIVE_SPACES, 6,
	LINE SPACE);
	assert_remove_trailing_blanks(LINE FIVE_SPACES, 7,
	LINE SPACE SPACE);

	/* just enough space in buffer */
	assert_remove_trailing_blanks(LINE FIVE_SPACES,
			strlen(LINE FIVE_SPACES) + 2 /* 1 for the \n, one for the \0 */,
			LINE);
}

static void concatenate_input_strings(char input_strings[][100],
		int num_strings, char input[]) {
	input[0] = 0;
	int i;
	for (i = 0; i < num_strings; i++) {
		strcat(input, input_strings[i]);
	}
}

static void compose_reverse_lines_failure_message(char failure_message[],
		const char format[], char input_string[]) {
	sprintf(failure_message, format, input_string);
}

static void assert_every_string_is_reversed(char input_strings[][100],
		int num_strings, char output[]) {
	int i, offset = 0;
	for (i = 0; i < num_strings; i++) {

		int str_len = strlen(input_strings[i]);

		if (str_len == 0) {
			continue;
		}

		char failure_message[1000];

		compose_reverse_lines_failure_message(failure_message,
				"First char of output line should be same as last of input line \"%s\"",
				input_strings[i]);
		TEST_ASSERT_EQUAL_INT_MESSAGE(input_strings[i][str_len - 1],
				output[offset], failure_message);

		compose_reverse_lines_failure_message(failure_message,
				"First char of output line should be same as last of input line \"%s\"",
				input_strings[i]);
		TEST_ASSERT_EQUAL_INT_MESSAGE(input_strings[i][0],
				output[offset + str_len - 1], failure_message);

		offset += str_len;
	}
}

static void assert_reverse_lines(char input_strings[][100], int num_strings,
		size_t buffer_size) {
	char input[1000];
	concatenate_input_strings(input_strings, num_strings, input);

	char output[MAX_FUN_OUTPUT_LEN];
	call_function_under_test(&reverse_lines, input, &reverse_lines_buffer_size,
			buffer_size, "reverse_lines", output);

	TEST_ASSERT_EQUAL_INT_MESSAGE(strlen(input), strlen(output),
			"Input and output should have same length");

	assert_every_string_is_reversed(input_strings, num_strings, output);
}

static void test_reverse_lines_with_empty_input() {
	char output[MAX_FUN_OUTPUT_LEN];
	call_function_under_test(&reverse_lines, "", &reverse_lines_buffer_size,
			reverse_lines_buffer_size, "reverse_lines", output);

	TEST_ASSERT_EQUAL_INT_MESSAGE(0, strlen(output),
			"Output should be an empty string");
}

static void test_reverse_lines() {
	char input_strings[][100] = /**/
	{ /**/
	LF, /**/
	LINE, LF,/**/
	LF, /**/
	VERY_VERY_LONG_LINE /**/
	};
	assert_reverse_lines(input_strings, 5, reverse_lines_buffer_size);

	char input_strings2[][100] = { LF };
	assert_reverse_lines(input_strings2, 1, reverse_lines_buffer_size);
}

static void test_reverse_lines_with_small_buffer() {
	char input_strings[][100] = { VERY_VERY_LONG_LINE };

	int small_buff_size = 5;
	assert_reverse_lines(input_strings, 1, small_buff_size);
}

void run_suite_for_ex1_16_print_longest_line() {
	RUN_TEST(test_print_longest_line);
	RUN_TEST(test_print_longest_line_with_ellipsis);
}

void run_suite_for_ex1_18_remove_trailing_blanks() {
	RUN_TEST(test_remove_trailing_blanks);
	RUN_TEST(test_remove_trailing_blanks_with_small_buffer);
	RUN_TEST(test_remove_blank_lines);
}

void run_suite_for_ex1_19_reverse_lines() {
	RUN_TEST(test_reverse_lines_with_small_buffer);
	RUN_TEST(test_reverse_lines);
	RUN_TEST(test_reverse_lines_with_empty_input);
}

int main(int argc, char **argv) {
	UNITY_BEGIN();
	run_suite_for_ex1_16_print_longest_line();
	run_suite_for_ex1_18_remove_trailing_blanks();
	run_suite_for_ex1_19_reverse_lines();
	return UNITY_END();
}
