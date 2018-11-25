#include <stdlib.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{

	int a = 5;
	int b = 7;

	printf("a: %d, b: %d\n", a, b);

	a = b;

	printf("a: %d, b: %d\n", a, b);

	b = 10;

	printf("a: %d, b: %d\n", a, b);

	return 0;
}