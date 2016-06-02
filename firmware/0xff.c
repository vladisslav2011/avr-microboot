#include <stdio.h>

int main()
{
	unsigned char b=0xff;
	while(1)
		fwrite(&b,1,1,stdout);
}

