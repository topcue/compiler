/*
	정규표현으로부터 ε-NFA를 구성하고, DFA로 변환하여, 상태수 최소화(reduced DFA)
	- 입력: 정규표현
	- 출력: DFA를 DFA-table.txt 와 같은 내용으로 출력
	- 방법: 예제소스 re2nfa.c, Enfa2dfa.c, dfa-opt.c를 1개의 .c 소스로 통합
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE  100
#define STATES      100

/* number string length : number(max 99) -> string(length 3) */
#define MAX_LENGTH  3
#define SYMBOLS     20

typedef struct stack_node {
	int ss;  /* start state */
	int es;  /* final state */
} SN;

/* ==================== global variables for re2nfa ==================== */

int N_symbols    = 0; /* number of input symbols */
int N_NFA_states = 0; /* number of NFA states */
int start_symbol = 0;
int final_symbol = 1;

char* NFAtab[STATES][SYMBOLS];

SN* stack[STACK_SIZE];
int cur_ss = 0;
int cur_es = 1;
int cur_symbol = 0;
int E_symbol;
int top = 0;

/* ==================== global variables for Enfa2dfa ==================== */

char* NFA_finals;  /* NFA final states */

int N_DFA_states;  /* number of DFA states */
int DFAtab[STATES][SYMBOLS];
char DFA_finals[STATES+1];  /* NFA final states */

char StateName[STATES][STATES+1];  /* state name table */
char Eclosure[STATES][STATES+1];   /* epsilon closure for each state */

// ==================== global variables for dfa-opt ====================

int N_optDFA_states;          /* number of optimized DFA states */
int OptDFA[STATES][SYMBOLS];  /* reduced DFA */
char NEW_finals[STATES+1];    /* final states of reduced DFA */

// ==================== Functions for re2nfa ====================

/* Print e-NFA table */
void put_eNFA_table(char *tab[][SYMBOLS], int nstates, int nsymbols) {
	int i, j;

	puts("\nNFA: STATE TRANSITION TABLE");

	/* input symbols: '0', '1', ... */
	printf("     | ");
	for (i = 0; i < nsymbols; i++) printf("  %-6c", '0'+i);
	printf("  e\n");	/* epsilon */

	printf("-----+--");
	for (i = 0; i <= nsymbols; i++) printf("-------");
	printf("\n");

	for (i = 0; i <= nstates; i++) {
		printf(" %2d  | ", i);	/* state */
		for (j = 0; j < nsymbols+1; j++) {
			if (tab[i][j]==NULL) printf("  %-6s","");
			else printf("  %-6s", tab[i][j]);
		}
		printf("\n");
	}
	printf("Start state  = %d\n", start_symbol);
	printf("Final states = { %d }\n", final_symbol);
}

/* Push (Stack) */
void push(SN* node) {
	if (top >= STACK_SIZE-1) {
		puts("STACK OVERFLOW!");
		return;
	}
	stack[top++] = node;
}

/* Pop (Stack) */
SN* pop() {
	return stack[--top];
}

/* Check is stack empty (Stack) */
int is_stack_empty() {
	return (top <= 0);
}

/* swap character */
void swap(char* str, int a, int b) {
	char tmp = str[a]; str[a] = str[b]; str[b] = tmp;
}

/* A utility function to reverse a string */
void reverse(char str[], int length) {
	int start = 0;
	int end = length -1;
	while (start < end) {
		swap(&str, start, end);
		start++;
		end--;
	}
}

/* Implementation of itoa() */
char* itoa(int num, char* str, int base) {
	int i = 0;
	int isNegative = 0;
  
	/* Handle 0 explicitely, otherwise empty string is printed for 0 */
	if (num == 0) {
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}
  
	// In standard itoa(), negative numbers are handled only with 
	// base 10. Otherwise numbers are considered unsigned.
	if (num < 0 && base == 10) {
		isNegative = 1;
		num = -num;
	}
  
	// Process individual digits
	while (num != 0) {
		int rem = num % base;
		str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
		num = num/base;
	}
  
	// If number is negative, append '-'
	if (isNegative)
		str[i++] = '-';
  
	str[i] = '\0'; // Append string terminator
  
	// Reverse the string
	reverse(str, i);
  
	return str;
}

/* Convert number to string */
char* create_number_to_string(int num) {
	char *string;
	string = (char *)malloc(sizeof(char)*MAX_LENGTH+1);
	itoa(num, string, 10);
	return string;
}

/* Works for concatenation operator of regex */
void operator_concatenation() {
	SN *state1, *state2;
	SN *new_state = (SN *)malloc(sizeof(SN));
	char* tmp;

	state2 = pop();
	state1 = pop();

	new_state->ss = state1->ss;
	new_state->es = state2->es;
	
	tmp = create_number_to_string(state2->ss);
	NFAtab[state1->es][E_symbol] = tmp;

	push(new_state);
}

/* Works for closure operator of regex */
void operator_closure() {
	SN* state;
	SN* new_state = (SN *)malloc(sizeof(SN));
	char* tmp1;
	char* tmp2;

	new_state->ss = cur_ss;
	new_state->es = cur_es;
	cur_ss+=2;
	cur_es+=2;

	state = pop();

	tmp1 = create_number_to_string(state->ss);
	strcat(tmp1, create_number_to_string(new_state->es));
	NFAtab[new_state->ss][E_symbol] = tmp1;

	tmp2 = create_number_to_string(state->ss);
	strcat(tmp2, create_number_to_string(new_state->es));
	NFAtab[state->es][E_symbol] = tmp2;
	
	push(new_state);
}

/* Works for union operator of regex */
void operator_union() {
	SN *state1, *state2;
	SN *new_state = (SN *)malloc(sizeof(SN));
	char *temp;

	new_state->ss = cur_ss;
	new_state->es = cur_es;
	cur_ss+=2;
	cur_es+=2;

	state2 = pop();
	state1 = pop();

	temp = create_number_to_string(state1->ss);
	strcat(temp, create_number_to_string(state2->ss));

	NFAtab[new_state->ss][E_symbol] = temp;
	NFAtab[state1->es][E_symbol] = create_number_to_string(new_state->es);
	NFAtab[state2->es][E_symbol] = create_number_to_string(new_state->es);

	push(new_state);
}

/*
	Create states w/ symbol

	         symbol_x
	state_n ---------> state_n+1
*/
void create_symbol_state(char c) {
	SN* state = (SN *)malloc(sizeof(SN));
	state->ss = cur_ss;
	state->es = cur_es;

	NFAtab[cur_ss][cur_symbol] = create_number_to_string(cur_es);
	push(state);

	printf("create symbol [%c] = %d\n", c, cur_symbol);

	cur_ss += 2;
	cur_es += 2;
	cur_symbol++;
}

/* Check is operator */
int is_operator(char ch) {
	if (ch=='+' || ch=='*' || ch=='.' || ch=='(' || ch==')') return 1;
	else return 0;
}

/* Count numbers of symbol */
void count_symbols(char *str) {
	while (*str) {
		if(!is_operator(*str)){ //is not operator
			N_symbols++;
		}
		str++;
	}
	E_symbol = N_symbols; // if N_symbols : 0~3, epsilon_simbol : 4
	printf("symbol count : %d\n", N_symbols);
}

/*
	regular_expression -> eNFA
*/

/* Core function to convert regex to e-NFA  */
void RE_to_eNFA(char *re) {
	SN* state;
	printf("\n[RE to eNFA]\n");

	count_symbols(re);

	while(*re) {
		switch(*re) {
		case '+': 
			operator_union();
			break;
		case '*':
			operator_closure();
			break;
		case '.':
			operator_concatenation();
			break;
		default : //create_symbol
			create_symbol_state(*re);
		}
		re++;
	}

	state = pop();
	start_symbol = state->ss;
	final_symbol = N_NFA_states = state->es;
}

/* Give an order (used at infix_to_postfix()) */
int order(char ch) {
	/* opder : '+' < '.' < '*' < '(' or ')' < char */
	if ('0'<=ch && ch<='9') return 9;
	else if('a'<=ch && ch<='z') return 9;
	else if('A'<=ch && ch<='Z') return 9;
	else if(ch=='+') return 1;
	else if(ch=='*') return 3;
	else if(ch=='.') return 2;
	else if(ch=='(') return 4;
	else if(ch==')') return 4;
	else return 0;
}

/* Convert infix expression to postfix expression */
char* infix_to_postfix(char *str) {
	char *p = str;
	char stack[999];
	int top=0;
	int i=0;
	char *postfix = (char *)malloc(strlen(str)+1);
	
	while(*p) {
		if (top == 0) { //stack empty
			if (is_operator(*p)) {
				stack[++top] = *p;
			} else
				postfix[i++] = *p;
			p++;
		} else {
			if (*p == '(') {
				stack[++top] = *p;
				p++;
			} else if (*p == ')') {
				while (stack[top] != '(') {
					postfix[i++] = stack[top--];
				}
				top--;
				p++;
			} else if (order(*p) >= order(stack[top])) {
				postfix[i++] = *p;
				p++;
			} else {
				if (stack[top]=='(') {
					stack[++top] = *p;
					p++;
				} else {
					postfix[i++] = stack[top--];
				}
			}
		}
	}

	// stack all pop
	while (top > 0) {
		postfix[i++] = stack[top--];
	}
	postfix[i] = '\0';
	return postfix;
}

/* Insert concatination operator */
char* insert_concatenation(char *str) {
	int len = strlen(str);
	char *result = (char *)malloc(len*2);
	int i = 0, j = 0;

	result[i++] = str[j++];
	for(; j<len;){
		if (!is_operator(str[j-1]) && !is_operator(str[j])) {
			result[i++] = '.';
		} else if (str[j-1]=='*' && !is_operator(str[j])) {
			result[i++] = '.';
		} else if (str[j-1]==')' && !is_operator(str[j])) {
			result[i++] = '.';
		}
		result[i++] = str[j++];
	}
	result[i] = '\0';

	return result;
}

// ==================== Functions for Enfa2dfa ====================

/* Load e-NFA table */
void load_NFA_table() {
	for (int i = 0; i < N_NFA_states+1; i++) {
		for (int j = 0; j < N_symbols+1; j++) {
			if (NFAtab[i][j] == NULL) {
				NFAtab[i][j] = "";
			}
		}
	}

	NFA_finals = malloc(2 * sizeof(char));
	NFA_finals[0] = '0' + final_symbol;
	NFA_finals[1] = '\0';
	N_DFA_states = 0;
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

	for (i = 0; i < nstates+1; i++) {
		printf(" %2d  | ", i);	/* state */
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
		for (j = 0; j < nsymbols; j++) {
			if(tab[i][j] == '@') tab[i][j] = ' ';
			printf("  %c  ", tab[i][j]);
		}
		printf("\n");
	}

	printf("Final states = {");
	for (i=0; i < strlen(finals); i++) printf(" %c", finals[i]);
	printf(" }\n");
}


/*
	Merge two strings.
	String 't' is merged into 's' in an alphabetical order.
	Return value: number of items that are added to 's'.
*/
int string_merge(char *s, char *t) {
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
	(state 스트링이므로 각 state에 대해 nextstate를 merge)
*/
void get_next_state_NFA(char *nextstates, char *cur_states, char *nfa[STATES][SYMBOLS], int symbol) {
	int i;
	char temp[STATES+1];

	temp[0] = '\0';
	for (i = 0; i < strlen(cur_states); i++)
		string_merge(temp, nfa[cur_states[i]-'0'][symbol]);
	strcpy(nextstates, temp);
}

/*
	StateName 테이블 'stnt'에서 'state'를 찾아 index를 return.
	'state'가 테이블에 없으면 끝에 추가하고 index를 return.
*/
int state_index(char *state, char stnt[][STATES+1], int *pn) {
	int i;

	if (!*state) return -1;	/* no next state */

	for (i = 0; i < *pn; i++)
		if (!strcmp(state, stnt[i])) return i;

	strcpy(stnt[i], state);	/* new state-name */
	return (*pn)++;
}

/* 'epstates': epsilon-accessible states for 'state'. */
void get_ep_states(int state, char *epstates, char *nfa[][SYMBOLS], int n_sym) {
	int i, n = 0;  /* 'n': number of added states */

	/* directly epsilon-accessible states */
	strcpy(epstates, nfa[state][n_sym]);
	
	do {  /* get all indirectly epsilon-accessible states */
		for (i = 0; i < strlen(epstates); i++) {
			n = string_merge(epstates, nfa[epstates[i]-'0'][n_sym]);
		}
	} while (n);  /* 더 이상 추가되는 state가 없을 때까지 */
}

/*
	Initialize 'Eclosure[]' for each NFA state.
	<주의> 'Eclosure[i]'에 자기 state는 제외하였음!
*/
void init_Eclosure(char eclosure[][STATES+1], char *nfa[][SYMBOLS], int n_nfa, int n_sym) {
	int i;

	printf("\nEpsilon-accessible states:\n");
	for (i = 0; i < n_nfa; i++) {
		get_ep_states(i, eclosure[i], nfa, n_sym);
		printf("    state %d : [%s]\n", i, eclosure[i]);
	} printf("\n");
}

/* Epsilon closure of 'states' is 'epstates'. */
void e_closure(char *epstates, char *states, char eclosure[][STATES+1]) {
	int i;

	strcpy(epstates, states);
	for (i = 0; i < strlen(states); i++)
		string_merge(epstates, eclosure[states[i]-'0']);
}

/*
	Convert NFA table to DFA table.
	Method:
		0. state-name이 스트링이므로 StateName 테이블 이용
		   'n' -- StateName[]에 등록된 state 개수
		1. DFA table의 entry 개수를 1로 초기화 및 StateName에 추가
		2. StateName[i]의 각 symbol들에 대해 nextstate 계산
		3. nextstate가 스트링이므로 StateName의 index를 DFA에 넣음
	Return value: number of DFA states.
*/
int nfa_to_dfa(char *nfa[][SYMBOLS], int n_nfa, int n_sym, int dfa[][SYMBOLS]) {
	int i = 0;	/* current index of DFA */
	int n = 1;	/* number of DFA states */

	n = n_sym-1;

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

/* NFA의 final state가 하나라도 포함된 모든 state가 DFA의 final state임. */
void get_DFA_finals(
	char *dfinals,	/* DFA final states */
	char *nfinals,	/* NFA final states */
	char stnt[][STATES+1],	/* state-name table */
	int n_dfa)	/* number of DFA states */
{
	int i, j, k=0, n=strlen(nfinals);

	// printf("%s\n", nfinals);

	for (i = 0; i < n_dfa; i++) {
		for (j = 0; j < n; j++) {
			// printf("%s\n", stnt[i]);
			if (strchr(stnt[i], nfinals[j])) {
				dfinals[k++] = i+'A';
				break;
			}
		}
	}
	dfinals[k] = '\0';
}


// ==================== Functions for dfa-opt ====================

/*
	Initialize NFA table.
	Not needed.
*/
void load_DFA_table() {
	/* Not needed. */
}

/*
	Get next-state string for current-state string.
	(state 스트링이므로 각 state에 대해 nextstate를 merge)
*/
void get_next_state(char *nextstates, char *cur_states, int dfa[STATES][SYMBOLS], int symbol) {
	int i, ch;

	for (i = 0; i < strlen(cur_states); i++)
		*nextstates++ = dfa[cur_states[i]-'A'][symbol];
	*nextstates = '\0';
}

/*
	Get index of the equivalence states for state 'ch'.
	Equiv. class id's are '0', '1', '2', ...
*/
char equiv_class_ndx(char ch, char stnt[][STATES+1], int n) {
	int i;

	for (i = 0; i < n; i++)
		if (strchr(stnt[i], ch)) return i+'0';
	return -1;	/* next state is NOT defined */
}

/*
	Check if all the next states belongs to same equivalence class.
	Return value:
		If next state is NOT unique, return 0.
		If next state is unique, return next state --> 'A/B/C/...'
	's' is a '0/1' string: state-id's
*/
char is_one_nextstate(char *s) {
	char equiv_class;	/* first equiv. class */

	while (*s == '@') s++;
	equiv_class = *s++;	/* index of equiv. class */

	while (*s) {
		if (*s != '@' && *s != equiv_class) return 0;
		s++;
	}

	return equiv_class;	/* next state: char type */
}

/*
	statename 테이블에서 'state'를 찾아 index를 return.
	'state'가 테이블에 없으면 끝에 추가하고 index를 return.
*/
int state_index2(char *state, char stnt[][STATES+1], int n, int *pn,
	int cur)	/* 'cur' is added only for 'printf()' */
{
	int i;
	char state_flags[STATES+1];	/* next state info. */

	if (!*state) return -1;	/* no next state */

	for (i = 0; i < strlen(state); i++)
		state_flags[i] = equiv_class_ndx(state[i], stnt, n);
	state_flags[i] = '\0';

	if(state_flags[0] == -1) {
		printf("   %d:[%s]\t--> [%s] ( )\n", cur, stnt[cur], state);
	} else {
		printf("   %d:[%s]\t--> [%s] (%s)\n", 
			cur, stnt[cur], state, state_flags);
	}
	

	if (i=is_one_nextstate(state_flags))
		return i-'0';	/* deterministic next states */
	else {
		strcpy(stnt[*pn], state_flags);	/* state-division info */
		return (*pn)++;
	}
}

/*
	Divide DFA states into finals and non-finals.
*/
int init_equiv_class(char statename[][STATES+1], int n, char *finals) {
	int i, j;

	if (strlen(finals) == n) {	/* all states are final states */
		strcpy(statename[0], finals);
		return 1;
	}

	strcpy(statename[1], finals);	/* final state group */

	for (i=j=0; i < n; i++) {
		if (i == *finals-'A') {
			finals++;
		} else statename[0][j++] = i+'A';
	}
	statename[0][j] = '\0';

	return 2;
}

/*
	Get optimized DFA 'newdfa' for equiv. class 'stnt'.
*/
int get_optimized_DFA(char stnt[][STATES+1], int n,
	int dfa[][SYMBOLS], int n_sym, int newdfa[][SYMBOLS])
{
	int n2=n;		/* 'n' + <num. of state-division info> */
	int i, j;
	char nextstate[STATES+1];

	for (i = 0; i < n; i++) {	/* for each pseudo-DFA state */
		for (j = 0; j < n_sym; j++) {	/* for each input symbol */
			get_next_state(nextstate, stnt[i], dfa, j);
			newdfa[i][j] = state_index2(nextstate, stnt, n, &n2, i)+'A';
		}
	}

	return n2;
}

/*
	char 'ch' is appended at the end of 's'.
*/
void chr_append(char *s, char ch) {
	int n=strlen(s);

	*(s+n) = ch;
	*(s+n+1) = '\0';
}

void sort(char stnt[][STATES+1], int n) {
	int i, j;
	char temp[STATES+1];

	for (i = 0; i < n-1; i++)
		for (j = i+1; j < n; j++)
			if (stnt[i][0] > stnt[j][0]) {
				strcpy(temp, stnt[i]);
				strcpy(stnt[i], stnt[j]);
				strcpy(stnt[j], temp);
			}
}

/*
	Divide first equivalent class into subclasses.
		stnt[i1] : equiv. class to be segmented
		stnt[i2] : equiv. vector for next state of stnt[i1]
	Algorithm:
		- stnt[i1] is splitted into 2 or more classes 's1/s2/...'
		- old equiv. classes are NOT changed, except stnt[i1]
		- stnt[i1]=s1, stnt[n]=s2, stnt[n+1]=s3, ...
	Return value: number of NEW equiv. classses in 'stnt'.
*/
int split_equiv_class(char stnt[][STATES+1],
	int i1,	/* index of 'i1'-th equiv. class */
	int i2,	/* index of equiv. vector for 'i1'-th class */
	int n,	/* number of entries in 'stnt' */
	int n_dfa)	/* number of source DFA entries */
{
	char *old=stnt[i1], *vec=stnt[i2];
	int i, n2, flag=0;
	char newstates[STATES][STATES+1];	/* max. 'n' subclasses */

	for (i=0; i < STATES; i++) newstates[i][0] = '\0';

	for (i=0; vec[i]; i++)
		chr_append(newstates[vec[i]-'0'], old[i]);

	for (i=0, n2=n; i < n_dfa; i++) {
		if (newstates[i][0]) {
			if (!flag) {	/* stnt[i1] = s1 */
				strcpy(stnt[i1], newstates[i]);
				flag = 1;	/* overwrite parent class */
			} else	/* newstate is appended in 'stnt' */
				strcpy(stnt[n2++], newstates[i]);
		}
	}

	sort(stnt, n2);	/* sort equiv. classes */

	return n2;	/* number of NEW states(equiv. classes) */
}

/*
	Equiv. classes are segmented and get NEW equiv. classes.
*/
int set_new_equiv_class(char stnt[][STATES+1], int n,
	int newdfa[][SYMBOLS], int n_sym, int n_dfa)
{
	int i, j, k;

	for (i = 0; i < n; i++) {
		for (j = 0; j < n_sym; j++) {
			k = newdfa[i][j]-'A';	/* index of equiv. vector */
			if (k >= n)	/* equiv. class 'i' should be segmented */
				return split_equiv_class(stnt, i, k, n, n_dfa);
		}
	}

	return n;
}

/* Print equiv classes */
void print_equiv_classes(char stnt[][STATES+1], int n) {
	int i;

	printf("\nEQUIV. CLASS CANDIDATE ==>");
	for (i = 0; i < n; i++)
		printf(" %d:[%s]", i, stnt[i]);
	printf("\n");
}

/*
	State-minimization of DFA: 'dfa' --> 'newdfa'
	Method:
		0. state-name이 스트링이므로 statename 테이블 이용
		   'n' -- statename[]에 등록된 state 개수
		1. DFA를 final states와 non-final states 2개의 동치류로 분할
		2. 2개의 동치류에 대해 next state가 동일 동치류인지 확인
		3. next state가 동일한 동치류가 아닌 것을 다시 분할
	Keypoint:
		현재 동치류에 대한 nextstate가 동치류에 없으면,
		statename 테이블에 그 정보를 추가하고 'n2'에 statename 테이블의
		entry 개수를 저장.
		즉, n == n2이면, 분할이 끝나고 optimized DFA가 생성됨!
	Return value: number of DFA states.
*/
int optimize_DFA(
	int dfa[][SYMBOLS],	/* DFA state-transition table */
	int n_dfa,	/* number of DFA states */
	int n_sym,	/* number of input symbols */
	char *finals,	/* final states of DFA */
	char stnt[][STATES+1],	/* state name table */
	int newdfa[][SYMBOLS])	/* reduced DFA table */
{
	char nextstate[STATES+1];
	int n;	/* number of new DFA states */
	int n2;	/* 'n' + <num. of state-dividing info> */

	n = init_equiv_class(stnt, n_dfa, finals);

	while (1) {
		print_equiv_classes(stnt, n);
		n2 = get_optimized_DFA(stnt, n, dfa, n_sym, newdfa);
		if (n != n2)
			n = set_new_equiv_class(stnt, n, newdfa, n_sym, n_dfa);
		else break;	/* equiv. class segmentation ended!!! */
	}

	return n;	/* number of DFA states */
}

/* Check if 't' is a subset of 's'. */
int is_subset(char *s, char *t) {
	int i;

	for (i = 0; *t; i++)
		if (!strchr(s, *t++)) return 0;
	return 1;
}

/* New finals states of reduced DFA. */
void get_NEW_finals(
	char *newfinals,	/* new DFA finals */
	char *oldfinals,	/* source DFA finals */
	char stnt[][STATES+1],	/* state name table */
	int n)	/* number of states in 'stnt' */ 
{
	int i;

	for (i = 0; i < n; i++)
		if (is_subset(oldfinals, stnt[i])) *newfinals++ = i+'A';
	*newfinals++ = '\0';
}


int main() {
	printf("\n\n ================ re2nfa ================\n");
	
	char* regular_expression_infix;
	char* regular_expression_postfix;
	char regular_expression[100] = "ab*";
	
	printf("Regular Expression : %s\n", regular_expression);

	regular_expression_infix = insert_concatenation(regular_expression);
	printf("insert concatenation : %s\n", regular_expression_infix);

	regular_expression_postfix = infix_to_postfix(regular_expression_infix);
	printf("infix to postfix : %s -> %s\n", regular_expression_infix, regular_expression_postfix);

	RE_to_eNFA(regular_expression_postfix);
	put_eNFA_table(NFAtab, N_NFA_states, N_symbols);
	

	printf("\n\n ================ Enfa2dfa ================\n");

	load_NFA_table();
	N_DFA_states = nfa_to_dfa(NFAtab, N_NFA_states, N_symbols, DFAtab);
	get_DFA_finals(DFA_finals, NFA_finals, StateName, N_DFA_states);
	
	print_dfa_table(DFAtab, N_DFA_states, N_symbols, DFA_finals);


	printf("\n\n ================ dfa-opt ================\n");
	
	N_optDFA_states = optimize_DFA(DFAtab, N_DFA_states, N_symbols, DFA_finals, StateName, OptDFA);
	get_NEW_finals(NEW_finals, DFA_finals, StateName, N_optDFA_states);

	print_dfa_table(OptDFA, N_optDFA_states, N_symbols, NEW_finals);

	return 0;
}

// EOF
