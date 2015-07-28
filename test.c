
#include <stdio.h>
#include "memModel.h"
#include "memoryAllocator.h"

int main(void)
{
	memInit();
	meminit();

	char *mem1 = (char*)memmalloc(100);
	char *mem2 = (char*)memmalloc(500);
	char *mem3 = (char*)memmalloc(1000);
	char *mem4 = (char*)memmalloc(2000);
	char *mem5 = (char*)memmalloc(5000);

	memfree(mem1);
	memfree(mem4);

	mem1 = (char*)memmalloc(200);

	memfree(mem1);
	memfree(mem2);
	memfree(mem3);
	memfree(mem5);


	getchar();
	return 0;
}