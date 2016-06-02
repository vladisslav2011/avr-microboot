#include <stdio.h>


int main()
{
	unsigned char  buffer[16];
	unsigned k,len;
	while(len=fread(&buffer[0],1,sizeof(buffer),stdin))
	{
		for(k=0;k<len;k++)
			fprintf(stdout,"0x%02x , ",buffer[k]);
		fprintf(stdout,"\n");
	}
}

