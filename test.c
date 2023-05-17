#include <stdlib.h>
#include <stdio.h>

int main() {
    int *array = malloc(sizeof(int) * 10);
    array[0] = 3;
    array[9] =  8;
    for (int i = 0; i < 10; i++) {
        printf("%d\n", array[i]);
    }
    free(array);
}