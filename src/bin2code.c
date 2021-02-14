/*
# _____     ___ ____     ___ ____
#  ____|   |    ____|   |        | |____|
# |     ___|   |____ ___|    ____| |    \    PS2DEV Open Source Project.
#-----------------------------------------------------------------------
# Copyright 2001-2004, ps2dev - http://www.ps2dev.org
# Licenced under Academic Free License version 2.0
# Review ps2sdk README & LICENSE files for further details.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

unsigned char *buffer;

const char * FORMATS[] = {
    "raw",
    "pnach"
};

/* 
 * ARGS:
 *  1. program path
 *  2. input file
 *  3. address
 *  4. output file
 *  5. [optional] output format
 */
int main(int argc, char *argv[])
{
	int fd_size;
	FILE *source,*dest;
	int i;
    int format_count = sizeof(FORMATS)/sizeof(char*);
    int format = 0;
    unsigned int address = 0;

	if(argc != 4 && argc != 5) {
		printf("Usage: bin2code infile address outfile [format]\n\n");
		return 1;
	}

	if((source=fopen( argv[1], "rb")) == NULL) {
		printf("Error opening %s for reading.\n",argv[1]);
		return 1;
	}

	fseek(source,0,SEEK_END);
	fd_size = ftell(source);
	fseek(source,0,SEEK_SET);

	buffer = malloc(fd_size);
	if(buffer == NULL) {
		printf("Failed to allocate memory.\n");
		return 1;
	}

	if(fread(buffer,1,fd_size,source) != fd_size) {
		printf("Failed to read file.\n");
		return 1;
	}
	fclose(source);

	if((dest = fopen(argv[3],"w+")) == NULL) {
		printf("Failed to open/create %s.\n",argv[3]);
		return 1;
	}

    address = (unsigned int)strtol(argv[2], NULL, 16);

    if (argc > 4)
    {
        for (i = 0; i < format_count; ++i)
        {
            if (strcmp(argv[4], FORMATS[i]) == 0) {
                format = i;
                break;
            }
        }
        
        if (i >= format_count) {
            printf("Failed to parse format %s. Please use ", argv[4]);
            for (i = 0; i < format_count; ++i)
                printf("%s, ", FORMATS[i]);
            printf("\n");
            return 1;
        }
    }

	for(i=0;i<fd_size;i += 4) {
        switch (format)
        {
            case 0: // RAW
            {
                fprintf(dest, "%08X %08X\n", 0x20000000 | (address + i), *(unsigned int*)(buffer + i));
                break;
            }
            case 1: // PNACH
            {
                fprintf(dest, "patch=1,EE,%08X,extended,%08X\n", 0x20000000 | (address + i), *(unsigned int*)(buffer + i));
                break;
            }
        }
	}

	fprintf(dest, "\n");

	fclose(dest);

	return 0;
}