#ifndef LINES_H_
#define LINES_H_

bool get_line_chunk(FILE *stream, char result[], size_t maxlen,
		int *result_len, bool *reached_eol, bool *reached_eof);

#endif /* LINES_H_ */
