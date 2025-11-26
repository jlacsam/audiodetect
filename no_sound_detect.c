/*
	gcc -o no_sound_detect no_sound_detect.c -I/data/Projects/SOLO/rdk6000v2/include
*/

#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <limits.h>
#include "solo/g723.h"

int get_file_size(char *pathfile)
{
        struct stat f;
        if (!stat(pathfile,&f)) {
                return f.st_size;
        } else {
                return 0;
        }
}

int no_sound_detect (G723_DATA *g723)
{
	int i, count=0;
	for (i=0; i<12; i++)
		if (g723->data[i]==UINT_MAX)
			count++;
	return (count==12);
}

int main (int argc, char *argv[])
{
	if (argc < 2) {
		printf("USAGE: no_sound_detect <g723 file>\n");
		return -1;
	}

	FILE *in;
	if ((in = fopen(argv[1],"r")) == NULL) {
		printf("Unable to open g723 file.\n");
		return -2;
	}

	G723_DATA g723;
	int s,t=0;
	s = get_file_size(argv[1]);
	while (t<s) {
		t += fread(&g723,1,sizeof(G723_DATA),in);
		if (no_sound_detect(&g723))
			printf("No sound detected.\n");
		else
			printf("Sound detected.\n");
	}
	fclose(in);

	return 0;
}
