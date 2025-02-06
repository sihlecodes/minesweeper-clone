#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

const char* basename(const char* path);

#define ERROR(message) fprintf(stderr, "%s:%d: %s\n", basename(__FILE__), __LINE__, message)

typedef struct List {
	int *items;
	size_t length;
	size_t capacity;
} List;

List list_create(size_t capacity);

void list_append(List* list, int item);

bool list_contains(List* list, int item);

void list_destroy(List *list);
