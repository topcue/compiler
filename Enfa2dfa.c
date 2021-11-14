/*
	Epsilon-NFA --> DFA conversion program
*/
#include <stdio.h>
#include <string.h>

#define STATES	99
#define SYMBOLS	20

int N_symbols;	/* number of input symbols */
int N_NFA_states;	/* number of NFA states */
char *NFAtab[STATES][SYMBOLS];
char *NFA_finals;	/* NFA final states */

int N_DFA_states;	/* number of DFA states */
int DFAtab[STATES][SYMBOLS];
char DFA_finals[STATES+1];	/* NFA final states */

char StateName[STATES][STATES+1];	/* state name table */
char Eclosure[STATES][STATES+1];	/* epsilon closure for each state */

/*
	Initialize NFA table.
*/
void load_NFA_table()
{
/*
	epsilon-NFA table for ex.24
	Last input symbol is an epsilon.
	Input symbols : 0(a), 1(b), 2(epsilon)

	NFAtab[0][0] = "0";
	NFAtab[0][1] = "";
	NFAtab[0][2] = "13";
	NFAtab[1][0] = "2";
	NFAtab[1][1] = "";
	NFAtab[1][2] = "";
	NFAtab[2][0] = "";
	NFAtab[2][1] = "2";
	NFAtab[2][2] = "3";
	NFAtab[3][0] = "3";
	NFAtab[3][1] = "";
	NFAtab[3][2] = "";

	N_symbols = 2;
	N_NFA_states = 4;
	NFA_finals = "3";
	N_DFA_states = 0;
*/


	// epsilon-NFA table for ex.25
	// Last input symbol is an epsilon.
	// Input symbols : 0(a), 1(b), 2(epsilon)

	NFAtab[0][0] = "1";
	NFAtab[0][1] = "";
	NFAtab[0][2] = "";
	NFAtab[0][3] = "2";
	NFAtab[1][0] = "";
	NFAtab[1][1] = "3";
	NFAtab[1][2] = "";
	NFAtab[1][3] = "";
	NFAtab[2][0] = "";
	NFAtab[2][1] = "";
	NFAtab[2][2] = "2";
	NFAtab[2][3] = "3";
	NFAtab[3][0] = "";
	NFAtab[3][1] = "";
	NFAtab[3][2] = "";
	NFAtab[3][3] = "";

	N_symbols = 3;
	N_NFA_states = 4;
	NFA_finals = "23";
	N_DFA_states = 0;


/*
	Epsilon-NFA table for TEST.
	Last input symbol is an epsilon.
	Input symbols : 0(a), 1(b), 2(epsilon)

	NFAtab[0][0] = "01";
	NFAtab[0][1] = "0";
	NFAtab[0][2] = "";
	NFAtab[1][0] = "13";
	NFAtab[1][1] = "2";
	NFAtab[1][2] = "3";
	NFAtab[2][0] = "2";
	NFAtab[2][1] = "1";
	NFAtab[2][2] = "2";
	NFAtab[3][0] = "";
	NFAtab[3][1] = "1";
	NFAtab[3][2] = "";

	N_symbols = 2;
	N_NFA_states = 4;
	NFA_finals = "3";
	N_DFA_states = 0;
*/

//      |   0       1       e
// -----+-----------------------
//   0  |   1
//   1  |                   5
//   2  |           3
//   3  |                   5
//   4  |                   02
//   5  |
// Start state  = 4
// Final states = { 5 }

	// NFAtab[0][0] = "1";
	// NFAtab[0][1] = "";
	// NFAtab[0][2] = "";

	// NFAtab[1][0] = "";
	// NFAtab[1][1] = "";
	// NFAtab[1][2] = "5";
	
	// NFAtab[2][0] = "";
	// NFAtab[2][1] = "3";
	// NFAtab[2][2] = "";
	
	// NFAtab[3][0] = "";
	// NFAtab[3][1] = "";
	// NFAtab[3][2] = "5";

	// NFAtab[4][0] = "";
	// NFAtab[4][1] = "";
	// NFAtab[4][2] = "02";

	// NFAtab[5][0] = "";
	// NFAtab[5][1] = "";
	// NFAtab[5][2] = "";

	// N_symbols = 2;
	// N_NFA_states = 5;
	// NFA_finals = "5";
	// N_DFA_states = 0;

}

/*
	Print state-transition table.
	State names: 'A', 'B', 'C', ...
*/
void print_nfa_table(
	char *tab[][SYMBOLS],	/* DFA table */
	int nstates,	/* number of states */
	int nsymbols,	/* number of input symbols */
	char *finals)	/* final states */
{
	int i, j;

	puts("\ne-NFA: STATE TRANSITION TABLE");

	/* input symbols: '0', '1', ... */
	printf("     | ");
	for (i = 0; i < nsymbols; i++) printf("  %-6c", '0'+i);
	printf("  e\n");	/* epsilon */

	printf("-----+--");
	for (i = 0; i < nsymbols+1; i++) printf("-------");
	printf("\n");

	for (i = 0; i < nstates; i++) {
		printf("  %c  | ", '0'+i);	/* state */
		for (j = 0; j < nsymbols+1; j++)
			printf("  %-6s", tab[i][j]);
		printf("\n");
	}
	printf("Final states = %s\n", finals);
}

/*
	Print state-transition table.
	State names: 'A', 'B', 'C', ...
*/
void print_dfa_table(
	int tab[][SYMBOLS],	/* DFA table */
	int nstates,	/* number of states */
	int nsymbols,	/* number of input symbols */
	char *finals)	/* final states */
{
	int i, j;

	puts("\nDFA: STATE TRANSITION TABLE");

	/* input symbols: '0', '1', ... */
	printf("     | ");
	for (i = 0; i < nsymbols; i++) printf("  %c  ", '0'+i);

	printf("\n-----+--");
	for (i = 0; i < nsymbols; i++) printf("-----");
	printf("\n");

	for (i = 0; i < nstates; i++) {
		printf("  %c  | ", 'A'+i);	/* state */
		for (j = 0; j < nsymbols; j++)
			printf("  %c  ", tab[i][j]);
		printf("\n");
	}

	printf("Final states = {");
	for (i=0; i < strlen(finals); i++) printf(" %c", finals[i]);
	printf(" }\n");
}

/*
	String 't' is merged into 's' in an alphabetical order.

	Return value: number of items that are added to 's'.
*/
int string_merge(char *s, char *t)
{
	int n=0;
	char temp[STATES+1], *r=temp, *p=s;

	while (*p && *t) {
		if (*p == *t) {
			*r++ = *p++; t++;
		} else if (*p < *t) {
			*r++ = *p++;
		} else {
			*r++ = *t++;
			n++;	/* an item is added to 's' */
		}
	}
	*r = '\0';

	if (*t) {
		strcat(r, t);
		n += strlen(t);
	} else if (*p) strcat(r, p);

	strcpy(s, temp);

	return n;
}

/*
	Get next-state string for current-state string.
	(state ��Ʈ���̹Ƿ� �� state�� ���� nextstate�� merge)
*/
void get_next_state_NFA(char *nextstates, char *cur_states,
	char *nfa[STATES][SYMBOLS], int symbol)
{
	int i;
	char temp[STATES+1];

	temp[0] = '\0';
	for (i = 0; i < strlen(cur_states); i++)
		string_merge(temp, nfa[cur_states[i]-'0'][symbol]);
	strcpy(nextstates, temp);
}

/*
	StateName ���̺� 'stnt'���� 'state'�� ã�� index�� return.
	'state'�� ���̺� ������ ���� �߰��ϰ� index�� return.
*/
int state_index(char *state, char stnt[][STATES+1], int *pn)
{
	int i;

	if (!*state) return -1;	/* no next state */

	for (i = 0; i < *pn; i++)
		if (!strcmp(state, stnt[i])) return i;

	strcpy(stnt[i], state);	/* new state-name */
	return (*pn)++;
}

/*
	'epstates': epsilon-accessible states for 'state'.
*/
void get_ep_states(int state, char *epstates,
	char *nfa[][SYMBOLS], int n_sym)
{
	int i, n=0;	/* 'n': number of added states */

	/* directly epsilon-accessible states */
	strcpy(epstates, nfa[state][n_sym]);

	do {	/* get all indirectly epsilon-accessible states */
		for (i = 0; i < strlen(epstates); i++)
			n = string_merge(epstates, nfa[epstates[i]-'0'][n_sym]);
	} while (n);	/* �� �̻� �߰��Ǵ� state�� ���� ������ */
}

/*
	Initialize 'Eclosure[]' for each NFA state.
	<����> 'Eclosure[i]'�� �ڱ� state�� �����Ͽ���!
*/
void init_Eclosure(char eclosure[][STATES+1],
	char *nfa[][SYMBOLS], int n_nfa, int n_sym)
{
	int i;

	printf("\nEpsilon-accessible states:\n");
	for (i = 0; i < n_nfa; i++) {
		get_ep_states(i, eclosure[i], nfa, n_sym);
		printf("    state %d : [%s]\n", i, eclosure[i]);
	} printf("\n");
}

/*
	Epsilon closure of 'states' is 'epstates'.
*/
void e_closure(char *epstates, char *states, char eclosure[][STATES+1])
{
	int i;

	strcpy(epstates, states);
	for (i = 0; i < strlen(states); i++)
		string_merge(epstates, eclosure[states[i]-'0']);
}

/*
	Convert NFA table to DFA table.
	Method:
		0. state-name�� ��Ʈ���̹Ƿ� StateName ���̺� �̿�
		   'n' -- StateName[]�� ��ϵ� state ����
		1. DFA table�� entry ������ 1�� �ʱ�ȭ �� StateName�� �߰�
		2. StateName[i]�� �� symbol�鿡 ���� nextstate ���
		3. nextstate�� ��Ʈ���̹Ƿ� StateName�� index�� DFA�� ����
	Return value: number of DFA states.
*/
int nfa_to_dfa(char *nfa[][SYMBOLS], int n_nfa,
	int n_sym, int dfa[][SYMBOLS])
{
	int i = 0;	/* current index of DFA */
	int n = 1;	/* number of DFA states */
	
	// my
	n = n_sym - 1;

	char nextstate[STATES+1];
	char temp[STATES+1];	/* epsilon closure */
	int j;

	init_Eclosure(Eclosure, nfa, n_nfa, n_sym);

	e_closure(temp, "0", Eclosure);
	strcpy(StateName[0], temp);	/* initialize start state */

	printf("Epsilon-NFA to DFA conversion\n");
	for (i = 0; i < n; i++) {	/* for each DFA state */
		for (j = 0; j < n_sym; j++) {	/* for each input symbol */
			get_next_state_NFA(nextstate, StateName[i], nfa, j);
			e_closure(temp, nextstate, Eclosure);
			dfa[i][j] = state_index(temp, StateName, &n);
			printf("    state %d(%4s) : %d --> state %2d(%4s)\n",
				i, StateName[i], j, dfa[i][j], temp);
			dfa[i][j] += 'A';	/* 0/1/2/... --> 'A/B/C/...' */
		}
	}

	return n;	/* number of DFA states */
}

/*
	NFA�� final state�� �ϳ��� ���Ե� ��� state�� DFA�� final state��.
*/
void get_DFA_finals(
	char *dfinals,	/* DFA final states */
	char *nfinals,	/* NFA final states */
	char stnt[][STATES+1],	/* state-name table */
	int n_dfa)	/* number of DFA states */
{
	int i, j, k=0, n=strlen(nfinals);

	for (i = 0; i < n_dfa; i++) {
		for (j = 0; j < n; j++) {
			if (strchr(stnt[i], nfinals[j])) {
				dfinals[k++] = i+'A';
				break;
			}
		}
	}
	dfinals[k] = '\0';
}

int main()
{
	load_NFA_table();
	print_nfa_table(NFAtab, N_NFA_states, N_symbols, NFA_finals);

	N_DFA_states = nfa_to_dfa(NFAtab, N_NFA_states, N_symbols, DFAtab);
	get_DFA_finals(DFA_finals, NFA_finals, StateName, N_DFA_states);

	print_dfa_table(DFAtab, N_DFA_states, N_symbols, DFA_finals);
	return 0;
}
