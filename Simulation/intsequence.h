#ifndef INT_SEQ_H_INCLUDED
#define INT_SEQ_H_INCLUDED

struct int_sequence {
	int count;
	int capacity;
	int* buffer;
	int realloc_amt;
};

#endif
