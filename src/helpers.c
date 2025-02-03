#include "helpers.h"

void list_init(List *list, int capacity) {
	list->length = 0;
	list->capacity = capacity;
	list->items = malloc((sizeof *list->items) * capacity);
}

void list_append(List *list, int item) {
	if (list->length == list->capacity) {
		list->capacity *= 2;

		void *resized = realloc(list->items, (sizeof *list->items) * list->capacity);

		if (resized != NULL) 
			list->items = resized;
		else
			printf("Something went wrong while resizing list\n");
	}

	list->items[list->length++] = item;
}

bool list_contains(List *list, int item) {
	for (size_t i = 0; i < list->length; i++) {
		if (list->items[i] == item)
			return true;
	}

	return false;
}

void list_deinit(List *list) {
	list->length = 0;
	list->capacity = 0;
	free(list->items);
}
