/*
	Simple DFA recognition program.
	Implementing DFA by goto statements.

	DFA -- "a+b+c+d"
*/

#include <stdio.h>

#define FAIL 0;
#define OKAY 1;

int ch;

int dfa() {
	ch = getchar();
	S:
		if (ch == 'a') {
			ch = getchar();
			goto A;
		} else return FAIL;
	A:
		if (ch == 'a') {
			ch = getchar();
			goto A;
		} else if (ch == 'b') {
			ch = getchar();
			goto B;
		} else return FAIL;
	B:
		if (ch == 'b') {
			ch = getchar();
			goto B;
		} else if (ch == 'c') {
			ch = getchar();
			goto C;
		} else return FAIL;
	C:
		if (ch == 'c') {
			ch = getchar();
			goto C;
		} else if (ch == 'd') {
			ch = getchar();
			goto F;
		} else return FAIL;
	F:
		if (ch == '\n') {
			return OKAY;
		} else return FAIL;

}

int main() {
	printf("Input string(a+b+c+d) = ");
	if (dfa())
		puts("OK");
	else puts("FAIL");

	return 0;
}

// EOF
