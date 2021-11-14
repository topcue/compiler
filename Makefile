CC     = gcc
TARGET = regex-eNFA-DFA-opt dfa-file dfa_deprecated dfa-goto dfa-opt nfa2dfa Enfa2dfa re2nfa

all: $(TARGET)

T: T.c
	$(CC) -o $@ $^ -g

clean:
	rm $(TARGET)

# EOF
