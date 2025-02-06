#include "helpers.h"

const char* basename(const char* path)
{
#ifdef _WIN32
	char separator = '\\';
#else
	char separator = '/';
#endif

	const char* result = strrchr(path, separator);

	if (result)
		return ++result;

	return "";
}

List list_create(size_t capacity) {
	if (capacity <= 0)
	{
		ERROR("Capacity must to be greater than 0.");
		return;
	}

	List list = {
		.length = 0,
		.capacity = capacity,
		.items = malloc((sizeof *list.items) * capacity),
	};

	return list;
}

void list_append(List *list, int item) {
	if (list->length == list->capacity) {
		list->capacity *= 2;

		void *resized = realloc(list->items, (sizeof *list->items) * list->capacity);

		if (resized != NULL)
			list->items = resized;
		else {
			ERROR("Failed to resize the list.");
			return;
		}
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

void list_destroy(List *list) {
	free(list->items);
}
