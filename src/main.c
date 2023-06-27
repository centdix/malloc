#include "include.h"

int main() {
    int *array = my_malloc(sizeof(int) * 5000);
	print_heap_state(HEAD);
	int *array2 = my_malloc(sizeof(int) * 10000);
	int *array3 = my_malloc(sizeof(int) * 20000);
	int *array4 = my_malloc(sizeof(int) * 15);
	int *array5 = my_malloc(sizeof(int) * 15);
	int *array6 = my_malloc(sizeof(int) * 15);
	array6 = my_realloc(array6, sizeof(int) * 30);
	print_heap_state(HEAD);
	// my_free(array);
	// my_free(array);
	my_free(array3);
	print_heap_state(HEAD);
	int *array7 = my_malloc(sizeof(int) * 3);
	my_free(array5);
	print_heap_state(HEAD);
	my_free(array4);
	print_heap_state(HEAD);
}