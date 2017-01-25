#include <stdio.h>
#define MAXLINE 1000 /* maximum input line length */

int getoneline(char line[], int maxline);
void copy(char to[], char from[]);

/* print the longest input line */
int main() {
	int len; /* current line length */
	int max; /* maximum length seen so far */
	char line[MAXLINE]; /* current input line */
	char longest[MAXLINE]; /* longest line saved here */
	max = 0;
	while ((len = getoneline(line, MAXLINE)) > 0)
		if (len > max) {
			max = len;
			copy(longest, line);
		}
	if (max > 0) /* there was a line */
		printf("%d (\"%s\")\n", max, longest);
	return 0;
}

/* getoneline: read a line into s, return length */
int getoneline(char s[], int lim) {
	int c, len = 0;
	while ((c = getchar()) != EOF && c != '\n') {
		if (len < lim - 1)
			s[len] = c;
		len++;
	}
	if (len < lim) {
		s[len] = '\0';
	} else {
		s[lim - 1] = '\0';
		if (lim > 4) /* save leading char (at least); finish with "..." */
			s[lim - 2] = s[lim - 3] = s[lim - 4] = '.';
	}
	return len;
}

/* copy: copy 'from' into 'to'; assume 'to' is big enough */
void copy(char to[], char from[]) {
	int i;
	i = 0;
	while ((to[i] = from[i]) != '\0')
		++i;
}
