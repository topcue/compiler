/*
	Simple DFA recognition program.
	Implementing DFA by state transition table.

	DFA -- a+b+c+d
*/
#include <stdio.h>

#define STATES	5	/* number of states */
#define SYMBOLS	4	/* number of symbols */

/*
	State Tranisition Table
*/
int ST_TAB[STATES][SYMBOLS] = {
	/* state S:0 */	{ 1, -1, -1, -1 },
	/* state A:1 */	{ 1, 2, -1, -1 },
	/* state B:2 */	{ -1, 2, 3, -1 },
	/* state C:3 */	{ -1, -1, 3, 4 },
	/* state F:4 */	{ -1, -1, -1, -1 }
};

char *final_states = "E";

void put_table(int tab[][SYMBOLS], int m, int n, char *final_states)
{
	int i, j;

	puts("State Transition Table");
	for (i = 0; i < n; i++)
		printf("\t%c", i+'a');
	putchar('\n');

	for (i = 0; i < m; i++) {
		putchar('A'+i);
		for (j = 0; j < n; j++) {
			printf("\t%c", 'A'+tab[i][j]);
		}
		putchar('\n');
	}
	printf("Final states = { %s }\n", final_states);
}

int main()
{
	int ch;	/* input symbol */
	int cur_state=0;	/* start state */

	put_table(ST_TAB, STATES, SYMBOLS, final_states);
	printf("Input string a+b+c+d = ");

	ch = getchar();
	while (ch != '\n') {
		ch = ch - 'a';
		if (ch < 0 || ch > 4) { puts("FAIL"); return 0; }
		cur_state = ST_TAB[cur_state][ch];
		if (cur_state < 0) { puts("FAIL"); return 0; }
		ch = getchar();
	}

	if (strchr(final_states, cur_state+'A')) puts("OK");	/* final states */
	else puts("FAIL");

	return 0;
}
