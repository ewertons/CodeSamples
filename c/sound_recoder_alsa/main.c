#include <stdio.h>
#include <stdlib.h>
#include "wav.h"


int main(int argc, char** argv)
{
	if (argc != 4) 
	{
		printf("Arguments: <samplerate> <bits per sample> <number of channels\r\n");
		return 1;
	}

	WaveHeader* header = genericWAVHeader(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));

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
