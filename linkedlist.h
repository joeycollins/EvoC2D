#ifndef LINKEDLIST_H_INCLUDED
#define LINKEDLIST_H_INCLUDED

#include <stdbool.h>

struct link {
	void* data;
	struct link* next;
};

struct linked_list {
	int count;
	struct link* head;
	struct link* tail;
};

struct linked_list new_list();

void list_add(struct linked_list* list, void* element);

void* list_pop_at(struct linked_list* list, int index);

bool list_contains(struct linked_list* list, void* element);

void free_list(struct linked_list* list);

void free_list_completely(struct linked_list* list);

#endif