/*
	Simple DFA recognition program.
	Implementing DFA by state transition table.

	DFA -- (1+01)*00(0+1)*
*/

#include <stdio.h>

#define STATES	3	/* number of states */
#define SYMBOLS	2	/* number of symbols */

/*
	State Tranisition Table in p102.
*/
int ST_TAB[STATES][SYMBOLS] = {
	/* state p */	{ 1, 0 },
	/* state q */	{ 2, 0 },
	/* state r */	{ 2, 2 }
};

void put_table(int tab[][SYMBOLS], int m, int n) {
	int i;

	puts("State Transition Table");
	puts("\t0\t1");
	for (i = 0; i < m; i++)
		printf("%c\t%c\t%c\n",
			'A'+i, 'A'+tab[i][0], 'A'+tab[i][1]);
	puts("Final states = { C }");
}

int main() {
	int ch;	/* input symbol */
	int cur_state=0;	/* start state */

	put_table(ST_TAB, STATES, SYMBOLS);
	printf("Input string (1+01)*00(0+1)* = ");

	ch = getchar();
	while (ch != '\n') {
		ch = ch - '0';
		if (ch < 0 || ch > 1) { puts("FAIL"); return 0; }
		cur_state = ST_TAB[cur_state][ch];
		if (cur_state < 0) { puts("FAIL"); return 0; }
		ch = getchar();
	}

	if (cur_state == 2) puts("OK");	/* final states = { 2 } */
	else puts("FAIL");

	return 0;
}

// EOF
