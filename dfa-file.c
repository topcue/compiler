/*
	Simple DFA recognition program.
	Implementing DFA by state transition table.
	Specifically, read data from a file 'DFA-table.txt'

	DFA -- a+b+c+d
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Maximum size of a line to read from a file */
#define LINE_SIZE 256
#define TRUE      1

/* Initialize global variable */
int** ST_TAB = NULL;

int STATES  = -1;
int SYMBOLS = -1;

int start_state = 0;
int final_states = -1;

int states_num = 0;

/* ================ Util Functions ================ */

/* Print error message */
void print_error(char* msg) {
	puts(msg);
	exit(1);
}

/* Check is prefix of string */
int is_prefix(const char* pre, const char* str) {
	return !strncmp(pre, str, strlen(pre));
}

/* Check is space or tab */
int is_space(char ch) {
	return ((ch == ' ') || (ch == '\t'));
}

/* string shift left */
void strshl(char* str, int offset) {
	int len;

	len = strlen(str);
	if(offset >= len){
		memset(str,'\0',len);
		return;
	}

	for (int i = 0; i < len - offset; i++){
		str[i] = str[i + offset];
		str[i + offset] = '\0';
	}
}

/* Remove blank prefix */
void remove_prefix_space(char* str) {
	while(is_space(str[0])) {
		strshl(str, 1);
	}
}

/* Parsing value of prefix from string */
int prefix_value(char* pre, char* str) {
	strshl(str, strlen(pre)+1);
	remove_prefix_space(str);
	
	return atoi(str);
}

/* Get offset of substring in string */
int substr_offset(char* str, char* sub) {
	int offset = 0;

	while(TRUE) {
		if(strlen(str + (++offset)) == 1) {
			return -1;
		} else if(!strncmp(str + offset, sub, 1)) {
			return offset;
		}
	}

	/* Unreachable code */
	print_error("[-] substr_offset(): unreachable code");
}

/* Free states table */
void free_table() {
	for (int i = 0; i < STATES; i++) {
		free(ST_TAB[i]);
	}
	free(ST_TAB);
}


/* ================ Core Functions ================ */

/* Fill states table with state */
void fill_st_table(char* str) {
	int inputs[LINE_SIZE] = {0x00, };
	int off = 0;
	int idx = 0;

	off = substr_offset(str, ":");
	strshl(str, off+1);

	while (off > 0) {
		remove_prefix_space(str);
		off = substr_offset(str, " ");
		inputs[idx++] = atoi(str);
		strshl(str, off);
	}

	/* Assert: parsed data isn't wrong! */
	assert(idx == SYMBOLS);

	/* Set table */
	ST_TAB[states_num] = (int*)malloc((SYMBOLS) * sizeof(int));
	for (int i = 0; i < SYMBOLS; i++) {
		ST_TAB[states_num][i] = inputs[i];
	}

	states_num++;
}

/* Read data from DFA-table */
void read_DFA_table() {
	FILE *fp;
	char line[LINE_SIZE];

	/* Open file */
	fp = fopen("DFA-table.txt", "r");
	if(fp == NULL){
		print_error("[-] File not found!");
	}

	/* Parsing from file */
	while (fgets(line, LINE_SIZE-1, fp)) {
		line[strcspn(line, "\n")] = 0;

		/* Skip if comment */
		if(line[0] == ';') {
			continue;
		}

		/* States info */
		if(strstr(line, ":")) {
			fill_st_table(line);
			continue;
		}

		/* Other info */
		if(is_prefix("n_states", line)) {
			STATES = prefix_value("n_states", line);
			ST_TAB = (int**)malloc(STATES * sizeof(int*));
		} else if(is_prefix("n_symbols", line)) {
			SYMBOLS = prefix_value("n_symbols", line);
		} else if(is_prefix("start_state", line)) {
			start_state = prefix_value("start_state", line);
		} else if(is_prefix("final_states", line)) {
			final_states = prefix_value("final_states", line);
		}
	}

	fclose(fp);
}

/* Print information of DFA-table */
void print_table(int** tab, int m, int n, int final_states) {
	printf("\n============ Info ============\n");
	printf("STATES: %d\n", STATES);
	printf("SYMBOLS: %d\n", SYMBOLS);
	printf("start_state: %c\n", 'A' + start_state);
	printf("final_states: %c\n", 'A' + final_states);

	puts("\n==== State Transition Table ====");
	for (int i = 0; i < n; i++) {
		printf("\t%c", i+'a');
	}
	putchar('\n');

	for (int i = 0; i < m; i++) {
		putchar('A'+i);
		for (int j = 0; j < n; j++) {
			if (tab[i][j] == -1) {
				printf("\t%c", ' ');
			} else {
				printf("\t%c", 'A'+tab[i][j]);	
			}
		}
		putchar('\n');
	}
	printf("Final states = { %c }\n", 'A'+final_states);
}

/* Check validity of string */
void oracle() {
	int ch;           /* input symbol */
	int cur_state=0;  /* start state */

	printf("\n============ Oracle ============\n");
	printf("Input string a+b+c+d = ");

	ch = getchar();
	while (ch != '\n') {
		ch = ch - 'a';
		if (ch < start_state || ch > SYMBOLS) {
			puts("FAIL");
			return;
		}
		cur_state = ST_TAB[cur_state][ch];
		if (cur_state < start_state) {
			puts("FAIL");
			return;
		}
		ch = getchar();
	}

	/* Final states */
	if (final_states == cur_state) {
		puts("OK");
	}
	else {
		puts("FAIL");
	}
}

int main() {
	/* Read data from DFA-table */
	read_DFA_table();

	/* Print information of DFA-table */
	print_table(ST_TAB, STATES, SYMBOLS, final_states);

	/* Check validity of string */
	oracle();

	/* Ready to return */
	free_table();
	
	return 0;
}

// EOF