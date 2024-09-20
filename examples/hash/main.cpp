#include <stdio.h>
#include "hash.h"

int main() {
    const char* vrijednost = "Test";
    printf("Vrijednost : Hash\n");
    printf("Rezulatat : \"%s\" : %ld\n",vrijednost, hash(vrijednost));
    return 0;
}