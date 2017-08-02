#include <stdio.h>
#include <stdlib.h>
#include "wav.h"


int main(void)
{
	WaveHeader* header = genericWAVHeader(16000, 8, 2);

	if (header == NULL)
	{
		printf("failed creating header\r\n");
	}
	else
	{
		FILE* f = fopen("./out.wav", "w");

		if (writeWAVHeader((int)f, header) != 0)
		{
			printf("failed writting header\r\n");
		}
		else if (recordWAV("./out.wav", header, 1000) != 0)
		{
			printf("failed recording\r\n");
		}

		fclose(f);
		free(header);
	}

	return 0;
}
