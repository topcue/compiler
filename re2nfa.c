/*
	regular expression -> epsilon NFA
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct stack_node {
	int ss;  // start state
	int es;  // final state
} SN;

#define MAX	999
#define STATES	256
#define MAX_LENGTH	3 //number string length : number(max 256) -> string(length 3)
#define SYMBOLS	20

int N_symbols=0; //number of input symbols
int N_NFA_states=0; //number of NFA states
int start_symbol=0;
int final_symbol=1;

char *NFAtab[STATES][SYMBOLS];

SN *stack[MAX];
int cur_ss=0;
int cur_es=1;
int cur_symbol=0;
int E_symbol;
int top=0;

void put_eNFA_table(char *tab[][SYMBOLS], int nstates, int nsymbols)
{
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
		printf("  %c  | ", '0'+i);	/* state */
		for (j = 0; j < nsymbols+1; j++) {
			if (tab[i][j]==NULL) printf("  %-6s","");
			else printf("  %-6s", tab[i][j]);
		}
		printf("\n");
	}
	printf("Start state  = %d\n", start_symbol);
	printf("Final states = { %d }\n", final_symbol);
}

void push(SN *node)
{
	if (top >= MAX-1) {
		puts("STACK OVERFLOW!");
		return;
	}
	stack[top++] = node;
}

SN* pop()
{
	return stack[--top];
}

int is_stack_empty()
{
	return (top <= 0);
}

void swap(char* str, int a, int b) {
	char tmp = str[a]; str[a] = str[b]; str[b] = tmp;
}

// A utility function to reverse a string
void reverse(char str[], int length) {
	int start = 0;
	int end = length -1;
	while (start < end) {
		swap(&str, start, end);
		start++;
		end--;
	}
}

// Implementation of itoa()
char* itoa(int num, char* str, int base) {
	int i = 0;
	int isNegative = 0;
  
	/* Handle 0 explicitely, otherwise empty string is printed for 0 */
	if (num == 0)
	{
		str[i++] = '0';
		str[i] = '\0';
		return str;
	}
  
	// In standard itoa(), negative numbers are handled only with 
	// base 10. Otherwise numbers are considered unsigned.
	if (num < 0 && base == 10)
	{
		isNegative = 1;
		num = -num;
	}
  
	// Process individual digits
	while (num != 0)
	{
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

char* create_number_to_string(int num)
{
	char *string;
	string = (char *)malloc(sizeof(char)*MAX_LENGTH+1);
	itoa(num, string, 10);
	return string;
}

// operater .
void operator_concatenation() { }

// operator *
void operator_closure() { }

// operator + 
void operator_union()
{
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
	create symbol state
		symbol_x
	state_n ---------> state_n+1
*/
void create_symbol_state(char c)
{

	SN *state = (SN *)malloc(sizeof(SN));
	state->ss = cur_ss;
	state->es = cur_es;

	NFAtab[cur_ss][cur_symbol] = create_number_to_string(cur_es);
	push(state);

	printf("create symbol [%c] = %d\n", c, cur_symbol);

	cur_ss += 2;
	cur_es += 2;
	cur_symbol++;
}

int is_operator(char ch)
{
	if (ch=='+' || ch=='*' || ch=='.' || ch=='(' || ch==')') return 1;
	else return 0;
}

void count_symbols(char *str)
{
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
void RE_to_eNFA(char *re)
{ 
	SN *state;
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

// infix_to_postfix.c
int order(char ch)
{
	/*
		opder : '+' < '.' < '*' < '(' or ')' < char
	*/
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

char* infix_to_postfix(char *str)
{
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

	//stack all pop
	while (top > 0) {
		postfix[i++] = stack[top--];
	}
	postfix[i] = '\0';
	return postfix;
}

char* insert_concatenation(char *str)
{
	int len = strlen(str);
	char *result = (char *)malloc(len*2);
	int i=0,j=0;

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

int main()
{
	char regular_expression[100];
	char *regular_expression_infix;
	char *regular_expression_postfix;

	printf("Regular Expression : ");
	scanf("%s", regular_expression);

	regular_expression_infix = insert_concatenation(regular_expression);
	printf("insert concatenation : %s\n", regular_expression_infix);

	regular_expression_postfix = infix_to_postfix(regular_expression_infix);
	printf("infix to postfix : %s -> %s\n", regular_expression_infix, regular_expression_postfix);

	RE_to_eNFA(regular_expression_postfix);

	put_eNFA_table(NFAtab, N_NFA_states, N_symbols);
	return 0;
}
