#include <stdio.h>
#include <limits.h>

typedef unsigned char BYTE;
typedef unsigned short WORD;

int DetectAudio (BYTE *buffer, int size, float tolerance, int format)
{
	int i, j, tol, min, max = 0, avg;
	BYTE *p;
	WORD *q;

	switch (format)
	{
		case 8: // 8-bit audio

			p = buffer;
			tol = (int) (tolerance * (float) UCHAR_MAX);
			min = (int) UCHAR_MAX;

			// Find min and max
			for (i=0; i<size/8; i++)
			{
				// Get rolling average of eight sample points
				for (j=0, avg=0; j<8; j++) 
					avg += (int) *p++;
				avg /= 8;

				min = min < avg ? min : avg;
				max = max > avg ? max : avg;
			}

			// Determine if there is significant audio
			if ((max - min) > tol)
				return 1;
			else
				return 0;

			break;

		case 16: // 16-bit audio

			q = (WORD *) buffer;
			tol = (int) (tolerance * (float) USHRT_MAX);
			min = (int) USHRT_MAX;

			// Find min and max
			for (i=0; i<size/8; i++)
			{
				// Get rolling average of eight sample points
				for (j=0, avg=0; j<8; j++)
					avg += (int) *q++;
				avg /= 8;

				min = min < avg ? min : avg;
				max = max > avg ? max : avg;
			}

			// Determine if there is significant audio
			if ((max - min) > tol)
				return 1;
			else
				return 0;

			break;

		default:
			return -1;
			break;
	}
}

int main (int argc, char* argv[])
{
	BYTE buffer[8000];
	fprintf(stdout,"Hello world!\n");

	memset(buffer,0,8000);
	buffer[4000] = atoi(argv[1]);
	int ret;
	ret = DetectAudio(buffer, 8000, 0.10, 8);
	if (ret == 1)
		fprintf(stdout,"Audio has been detected.\n");

	return 0;
}
