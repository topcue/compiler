/*
	NFA --> DFA conversion program
*/
#include <stdio.h>
#include <string.h>

#define STATES	256
#define SYMBOLS	20

int N_symbols;	/* number of input symbols */
int NFA_states;	/* number of NFA states */
char *NFAtab[STATES][SYMBOLS];

int DFA_states;	/* number of DFA states */
int DFAtab[STATES][SYMBOLS];

/*
	Initialize NFA table.
*/
void init_NFA_table()
{
/*
	NFA table for ex.21

	NFAtab[0][0] = "01";
	NFAtab[0][1] = "0";
	NFAtab[1][0] = "";
	NFAtab[1][1] = "01";

	NFA_states = 2;
	DFA_states = 0;
	N_symbols = 2;
*/
/*
	NFA table for ex.17
*/
/*	NFAtab[0][0] = "12";
	NFAtab[0][1] = "13";
	NFAtab[1][0] = "12";
	NFAtab[1][1] = "13";
	NFAtab[2][0] = "4";
	NFAtab[2][1] = "";
	NFAtab[3][0] = "";
	NFAtab[3][1] = "4";
	NFAtab[4][0] = "4";
	NFAtab[4][1] = "4";

	NFA_states = 5;
	DFA_states = 0;
	N_symbols = 2;
*/
//	NFA table for mid-term exam 2014

	NFAtab[0][0] = "0";
	NFAtab[0][1] = "12";
	NFAtab[1][0] = "13";
	NFAtab[1][1] = "1";
	NFAtab[2][0] = "23";
	NFAtab[2][1] = "2";
	NFAtab[3][0] = "1";
	NFAtab[3][1] = "13";

	NFA_states = 4;
	DFA_states = 0;
	N_symbols = 2;
}

/*
	Print state-transition table.
	State names: 'A', 'B', 'C', ...
*/
void print_nfa_table(
	char *tab[][SYMBOLS],	/* DFA table */
	int nstates,	/* number of states */
	int nsymbols)	/* number of input symbols */
{
	int i, j;

	puts("\nInput e-NFA:\nSTATE TRANSITION TABLE");

	/* input symbols: '0', '1', ... */
	printf("     | ");
	for (i = 0; i < nsymbols; i++)
		printf("  %-6c", '0'+i);
	printf("\n");

	printf("-----+--");
	for (i = 0; i < nsymbols; i++) printf("-------");
	printf("\n");

	for (i = 0; i < nstates; i++) {
		printf("  %c  | ", '0'+i);	/* state */
		for (j = 0; j < nsymbols; j++)
			printf("  %-6s", tab[i][j]);
		printf("\n");
	}
}

/*
	Print state-transition table.
	State names: 'A', 'B', 'C', ...
*/
void print_dfa_table(
	int tab[][SYMBOLS],	/* DFA table */
	int nstates,	/* number of states */
	int nsymbols)	/* number of input symbols */
{
	int i, j;

	puts("\nOutput DFA:\nSTATE TRANSITION TABLE");

	/* input symbols: '0', '1', ... */
	printf("     | ");
	for (i = 0; i < nsymbols; i++) printf("  %c  ", '0'+i);

	printf("\n-----+--");
	for (i = 0; i < nsymbols; i++) printf("-----");
	printf("\n");

	for (i = 0; i < nstates; i++) {
		printf("  %c  | ", 'A'+i);	/* state */
		for (j = 0; j < nsymbols; j++)
			printf("  %c  ", 'A'+tab[i][j]);
		printf("\n");
	}
}

/*
	String 't' is merged into 's' in an alphabetical order.
*/
void string_merge(char *s, char *t)
{
	char temp[STATES], *r=temp, *p=s;

	while (*p && *t) {
		if (*p == *t) {
			*r++ = *p++; t++;
		} else if (*p < *t) {
			*r++ = *p++;
		} else
			*r++ = *t++;
	}
	*r = '\0';

	if (*p) strcat(r, p);
	else if (*t) strcat(r, t);

	strcpy(s, temp);
}

/*
	Get next-state string for current-state string.
	(state ﾽﾺￆﾮﾸﾵ￀ￌﾹￇﾷￎ ﾰﾢ state﾿ﾡ ﾴ￫ￇ￘ nextstateﾸﾦ merge)
*/
void get_next_state(char *nextstates, char *cur_states,
	char *nfa[STATES][SYMBOLS], int n_nfa, int symbol)
{
	int i;
	char temp[STATES];

	temp[0] = '\0';
	for (i = 0; i < strlen(cur_states); i++)
		string_merge(temp, nfa[cur_states[i]-'0'][symbol]);
	strcpy(nextstates, temp);
}

/*
	statename ￅￗ￀ￌﾺ￭﾿ﾡﾼﾭ 'state'ﾸﾦ ￃﾣﾾￆ indexﾸﾦ return.
	'state'ﾰﾡ ￅￗ￀ￌﾺ￭﾿ﾡ ﾾ￸￀ﾸﾸ￩ ﾳﾡ﾿ﾡ ￃ￟ﾰﾡￇￏﾰ￭ indexﾸﾦ return.
*/
int state_index(char *state, char statename[][STATES], int *pn)
{
	int i;

	if (!*state) return -1;	/* no next state */

	for (i = 0; i < *pn; i++)
		if (!strcmp(state, statename[i])) return i;

	strcpy(statename[i], state);	/* new state-name */
	return (*pn)++;
}

/*
	Convert NFA table to DFA table.
	Method:
		0. state-name￀ￌ ﾽﾺￆﾮﾸﾵ￀ￌﾹￇﾷￎ statename ￅￗ￀ￌﾺ￭ ￀ￌ﾿￫
		   'n' -- statename[]﾿ﾡ ﾵ￮ﾷￏﾵ￈ state ﾰﾳﾼ￶
		1. DFA table￀ￇ entry ﾰﾳﾼ￶ﾸﾦ 1ﾷￎ ￃￊﾱ￢￈ﾭ ﾹￗ statename﾿ﾡ ￃ￟ﾰﾡ
		2. statename[i]￀ￇ ﾰﾢ symbolﾵ￩﾿ﾡ ﾴ￫ￇ￘ nextstate ﾰ￨ﾻ￪
		3. nextstateﾰﾡ ﾽﾺￆﾮﾸﾵ￀ￌﾹￇﾷￎ statename￀ￇ indexﾸﾦ DFA﾿ﾡ ﾳￖ￀ﾽ
	Return value: number of DFA states.
*/
int nfa_to_dfa(char *nfa[STATES][SYMBOLS], int n_nfa,
	int n_sym, int dfa[][SYMBOLS])
{
	char statename[STATES][STATES];
	int i = 0;	/* current index of DFA */
	int n = 1;	/* number of DFA states */

	char nextstate[STATES];
	int j;

	strcpy(statename[0], "0");	/* start state */

	for (i = 0; i < n; i++) {	/* for each DFA state */
		for (j = 0; j < n_sym; j++) {	/* for each input symbol */
			get_next_state(nextstate, statename[i], nfa, n_nfa, j);
			dfa[i][j] = state_index(nextstate, statename, &n);
		}
	}

	return n;	/* number of DFA states */
}

int main()
{
	init_NFA_table();
	print_nfa_table(NFAtab, NFA_states, N_symbols);

	DFA_states = nfa_to_dfa(NFAtab, NFA_states, N_symbols, DFAtab);
	print_dfa_table(DFAtab, DFA_states, N_symbols);
	puts("Final states = { ... }");

	return 0;
}
