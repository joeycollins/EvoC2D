#include "linkedlist.h"
#include <stdlib.h>

struct linked_list new_list() {
	struct linked_list list = {
		.count = 0,
		.head = NULL,
		.tail = NULL
	};
	return list;
}

void list_add(struct linked_list* list, void* element) {
	struct link new_link = {
		.data = element,
		.next = NULL
	};

	struct link* addr = malloc(sizeof(struct link));
	*addr = new_link;

	if (list->head == NULL) {
		list->head = addr;
	}
	else {
		list->tail->next = addr;
	}
	list->tail = addr;
	list->count++;
}

void* list_pop_at(struct linked_list* list, int index) {
	if (list->count == 0 || index >= list->count) {
		return NULL;
	}

	struct link* previous = NULL;
	struct link* to_remove = list->head;
	for (int i = 0; i < index; i++) {
		previous = to_remove;
		to_remove = to_remove->next;
	}
	void* res = to_remove->data;
	if (to_remove->next == NULL) {
		list->tail = previous;
	}
	if (previous == NULL) {
		list->head = to_remove->next;
	}
	else {
		previous->next = to_remove->next;
	}
	free(to_remove);
	list->count--;
	return res;
}


void free_list(struct linked_list* list) {
	if (list->head == NULL) {
		return;
	}
	struct link* current = list->head;
	for (int i = 0; i < list->count; i++) {
		struct link* next = current->next;
		free(current);
		current = next;
	}
}



void free_list_completely(struct linked_list* list) {
	if (list->head == NULL) {
		return;
	}
	struct link* current = list->head;
	for (int i = 0; i < list->count; i++) {
		struct link* next = current->next;
		free(current->data);
		free(current);
		current = next;
	}
}

bool list_contains(struct linked_list* list, void* element) {
	if (list->count == 0) {
		return false;
	}

	struct link* current = list->head;
	for (int i = 0; i < list->count; i++) {
		if (current->data == element) {
			return true;
		}
		current = current->next;
	}
	return false;
}