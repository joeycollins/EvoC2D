#ifndef INT_SEQ_H
#define INT_SEQ_H

struct int_sequence {
	int count;
	int capacity;
	int* buffer;
	int realloc_amt;
};

#endif
