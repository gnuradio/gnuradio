// Altera requires configuration bytes to be sent LSB first but the
// SD Card reads bytes MSB first
// This code reverses the bits of the altera bitstream so
// it will come out correct when read from the SD card
// $Log: bit_reverse.c,v $
// Revision 1.1  2006/01/06 14:44:17  mbl
// initial version
//



#include "stdio.h"
#include "string.h"

FILE*   fileOut;
FILE*   fileIn;

void    outIOerror(char* pfn);
void    inIOerror(char* pfn);

int main(int argc, char* arg[])
{
	unsigned char input, output;
	unsigned char in_mask, out_mask;
	int i;

	fileOut = fopen(arg[2],"wb");
	if (fileOut == NULL)
	{
	    outIOerror(arg[2]);
	    exit(-1);
	}

	printf("Opening input file %s\n", arg[1]);
        fileIn = fopen(arg[1],"rb");
        if (fileIn == NULL)
        {
            inIOerror(arg[1]);
            exit(-1);
        }

        while (!feof(fileIn) && fgets((char*)&input, 2 ,fileIn) != NULL)
        {
    	    in_mask = 1;
    	    out_mask = 0x80;
	    output = 0;

    	    for ( i=0; i < 8; ++i )
     	    {
       		if (input & in_mask) 
       		{
        	    output |= out_mask;
       		}
       		out_mask = out_mask >> 1;
       		in_mask = in_mask << 1;
     	    }
	    fwrite((void*)&output,sizeof(char),1,fileOut);
	}

	fclose(fileIn);
	fclose(fileOut);
	printf("\n%s has been created\n", arg[2]);
	exit(0);
}

void outIOerror(char *pfn)
{
	printf("I/O Error while writing to file=%s\n",pfn);
}

void inIOerror(char *pfn)
{
	printf("I/O Error while reading file=%s\n",pfn);
}
