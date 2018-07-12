#include <stdio.h>
#include <stdlib.h>
#include "Decodeoutput.h"

Decodeoutput::Decodeoutput()
{
	pointer_file = NULL;
}

void Decodeoutput::SetValue(char* datapath, int channel)
{
	this->channel = channel;
	sprintf(filename, "%sch_%d.txt", datapath, channel);
	printf("Output file: %s\n", filename);
	this->pointer_file = fopen(filename, "w");
	if (pointer_file == NULL)
	{
		printf("open file error\n");
        return;
	}
}
