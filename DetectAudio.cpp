#include <malloc.h>
#include <stdio.h>
#include <limits.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include "DetectAudio.h"

// Initialize GapWatch Object
int GWInitialize (char f[])
{
	int i;
	char keyname[100], tmppath[GW_DEFFILENAME];

	// Initialize parameters for GapWatch
	long gwparam;
	gwparam = GetPrivateProfileInt("GAPWATCH", "History", GW_DEFHISTORY, f);
	GWAdServM3.Modify(GW_IDXHISTORY, gwparam);
	gwparam = GetPrivateProfileInt("DCU", "SampleRate", GW_DEFSAMPLING, f);
	GWAdServM3.Modify(GW_IDXSAMPLING, gwparam);
	gwparam = GetPrivateProfileInt("GAPWATCH", "Epoch", GW_DEFEPOCH, f);
	GWAdServM3.Modify(GW_IDXEPOCH, gwparam);
	gwparam = GetPrivateProfileInt("GAPWATCH", "Consec", GW_DEFCONSEC, f);
	GWAdServM3.Modify(GW_IDXCONSEC, gwparam);
	gwparam = GetPrivateProfileInt("GAPWATCH", "Distance", GW_DEFDISTANCE, f);
	GWAdServM3.Modify(GW_IDXDISTANCE, gwparam);
	gwparam = GetPrivateProfileInt("GAPWATCH", "GapTol", GW_DEFAMPTOL, f);
	GWAdServM3.Modify(GW_IDXGAPTOL, gwparam);

	// determine if port numbers will be used for extension names
	gwparam = GetPrivateProfileInt("DCU", "UsePortNo", 0, f);
	GWAdServM3.Modify(GW_IDXUSEPORTS, gwparam);

	// Initialize directories for GapWatch
	for ( i = 1; i <= GWAdServM3.Channel; ++i )
	{
		sprintf( keyname, "DCUDirectory%d", i );
		GetPrivateProfileString("DCU", keyname, "", tmppath, GW_DEFFILENAME-1, f);
		if ( tmppath[ strlen( tmppath )-1 ] != '\\' ) strcat( tmppath, "\\" );
		GWAdServM3.SetPath( i-1, tmppath );
	}

	return 0;
}

// Process Buffer, Buffer is interleaved data.
void GWProcess (short *b, short i = 1)
{
	short *b1, j;
	for ( j = 0; j < 1; j++ )
	{
		b1 = b + j * GWAdServM3.Sampling;
		GWAdServM3.Process(b1);
	}
}

// This is the constructor of a class that has been exported.
// see GapWatch.h for the class definition
CDetectAudio::CDetectAudio()
{ 
	int i, j;

	// Initialize scalars
	Sampling 	= DA_DEFSAMPLING;
	History 	= DA_DEFHISTORY;
	Epoch 		= DA_DEFEPOCH;
	GapTol 		= (float) DA_DEFGAPTOL;
	Consec 		= DA_DEFCONSEC;
	Channel 	= DA_DEFCHANNEL;
	Distance 	= DA_DEFDISTANCE;
	EpochSize 	= Sampling / Epoch;

	// Initialize arrays
	Counter		= (int *) calloc( Channel, sizeof(int) );
	GlobalMax	= (int *) calloc( Channel, sizeof(int) );
	Position	= (float *) calloc( Channel, sizeof(float) );
	Previous	= (float *) calloc( Channel, sizeof(float) );
	Last		= (float *) calloc( Channel, sizeof(float) );
	EpochMax	= (short *) calloc( Channel, sizeof(short) );
	AmpTol		= (short *) calloc( Channel, sizeof(short) );
	NumConsec	= (short *) calloc( Channel, sizeof(short) );
	LocalMax	= (short *) calloc( Channel * History, sizeof(short) );

	// Initialize values
	for ( i = 0; i < Channel; ++i ) 
	{
		AmpTol[ i ] = DA_DEFAMPTOL;
		GlobalMax[ i ] = DA_DEFGLOBALMAX;
		for ( j = 0; j < History; ++j ) 
		{
			LocalMax[ i * History + j ] = DA_DEFLOCALMAX;
		}
	}

	// Initialize character arrays
	Path = (char **)	malloc( Channel * sizeof(char *) );
	for ( i = 0; i < Channel; ++i ) 
		Path[ i ] = (char *) malloc( DA_DEFFILENAME * sizeof(char) );

	return; 
}

CDetectAudio::~CDetectAudio()
{
	int i;

	// Free memory
	free(Counter);
	free(Position);
	free(Previous);
	free(Last);
	free(EpochMax);
	free(AmpTol);
	free(NumConsec);
	free(GlobalMax);
	free(LocalMax);
	
	// Free filenames
	for ( i = 0; i < Channel; ++i ) free( Path[ i ] );
	free(Path);

	return; 
}

CDetectAudio::Process(short *b)
{
	Buffer = b;
	Update();
	Find();
}


CDetectAudio::SetPath(int c, char *f)
{
	ActiveChannel = c;
	strcpy(Path[ ActiveChannel ],f);
}


CDetectAudio::Update()
{	
	int i, j, *ga;
	short *a, *la;

	// Find amplitude tolerances for each channel
	for ( a = AmpTol, ga = GlobalMax, la = LocalMax, i = 0; i < Channel; ++i, ++ga, ++a ) 
	{
		for ( *ga = 0, j = 0; j < History; ++j, ++la ) 
		{
			*ga += *la; 
		}
		*a = (short) (GapTol * (float) *ga / (float) History);
	}
}


CDetectAudio::Find()
{
	int i, j, k, h, f, g;
	short *l, *b, *e, *a, *n;
	float *p, *q, *r;
	struct tm *t;
	time_t now, hh, mm, ss;

	// For each Channel
	for ( i = 0; i < Channel; ++i ) {
		
		// Determine position in History
		h = Counter[ i ]  % History;

		// Initialize pointers
		l = LocalMax + (i * History + h);
		b = Buffer + i;
		e = EpochMax + i;
		a = AmpTol + i;
		n = NumConsec + i;
		p = Position + i;
		q = Previous + i;
		r = Last + i;

		// For each epoch
		for ( *l = 0, j = 0; j < Epoch; ++j ) {

			// Find maximum (g) and minimum (f) amplitude for each epoch
			for ( g = SHRT_MIN, f = SHRT_MAX, k = 0; k < EpochSize; ++k, b += Channel ) 
			{
				if ( *b > g ) g = *b;
				if ( *b < f ) f = *b;
			}

			// Half the distance between maximum and minimum is the adjusted epoch maximum.
			*e = abs((g - f) / 2);

			// If the maximum epoch is below amplitude tolerance
			if ( *e < *a ) {
				(*n)++;
				if ( *n == Consec ) {

					// Get current time
					now = time(NULL);
					t = localtime(&now);
					hh = t->tm_hour;
					mm = t->tm_min;
					ss = t->tm_sec;

					// Seconds after midnight
					*p = (float) 3600 * (float) hh + (float) 60 * (float) mm + (float) ss + 
						(float) j / (float) Epoch;
					//*p = (float) Counter[ i ] + (float) j / (float) Epoch;
					
					// Check if crossed another day
					if ( *q > *p ) {
						*q = 0;
						*r = 0;
					}

					// Check distance between gaps
					if ( *p - *q > (float) Distance ) { 
						ActiveChannel = i;
						Print();
						*r = *p;
					}
					
					// Remember last gap
					*q = *p;

				} // If the gap is long enough
			} // if there is a gap

			else {
				*n = 0;
			}

			// Update local maximum
			if ( *e > *l ) *l = *e;

		} // Each epoch

		// Increment or reset counter
		if ( Counter[ i ] == LONG_MAX ) Counter[ i ] = 0;
		else ++Counter[ i ];

	} // Each channel


}


int CDetectAudio::Print()
{
	FILE *out;
	char filename[20], pathname[128], ch[10];
	struct tm *t;
	time_t now;

	// Get current time
	now = time(NULL);
	t = localtime(&now);

	// Get filename
	if ( UsePorts == 1 )
	{
		strftime(filename,20,"%Y%m%d",t);
		sprintf(ch,"%d",ActiveChannel+1);
		strcat(filename,".");
		strcat(filename,ch);
		strcat(filename,"gp");
	}
	else
	{
		strftime(filename,20,"%Y%m%d",t);
		strcat(filename,".gap");
	}

	// Get pathname
	sprintf( pathname,"%s%s", Path[ ActiveChannel ], filename );

	// Open file and append/create
	if ( ( out = fopen(pathname,"a+t") ) == NULL ) return 1;
	//fwrite(&Position[ ActiveChannel ],sizeof(float),1,out);

	// Append previous if not yet written
	if ( Previous[ ActiveChannel ] != Last[ ActiveChannel ] )
		fprintf(out,"%8.2f~~", Previous[ ActiveChannel ]);

	// Write current gap
	fprintf(out,"%8.2f~~", Position[ ActiveChannel ]);

	// Close file
	fclose(out);

	return 0;
}


CDetectAudio::Modify(int m, int Value)
{
	int i;

	if ( m == GW_IDXCHANNEL ) {

		// Free filenames
		for ( i = 0; i < Channel; ++i ) free( Path[ i ] );
		free(Path);

		// Update value
		Channel = Value;
		
		// Reset all arrays;
		Counter		= (int *)	calloc( Channel, sizeof(int) );
		GlobalMax	= (int *)	calloc( Channel, sizeof(int) );
		Position	= (float *)	calloc( Channel, sizeof(float) );
		Previous	= (float *)	calloc( Channel, sizeof(float) );
		Last		= (float *) calloc( Channel, sizeof(float) );
		EpochMax	= (short *) calloc( Channel, sizeof(short) );
		AmpTol		= (short *) calloc( Channel, sizeof(short) );
		NumConsec	= (short *) calloc( Channel, sizeof(short) );
		LocalMax	= (short *) calloc( Channel * History, sizeof(short) );

		// Initialize character arrays
		Path = (char **) malloc( Channel * sizeof(char *) );
		for ( i = 0; i < Channel; ++i ) 
			Path[ i ] = (char *) malloc( GW_DEFFILENAME * sizeof(char) );

	}

	else {
		switch ( m ) 
		{
		case DA_IDXHISTORY	: 
			History	= Value;
			LocalMax = (short *) realloc( LocalMax, Channel * History * sizeof(short) );

		case DA_IDXSAMPLING	: Sampling	= Value;
		case DA_IDXEPOCH	: Epoch		= Value;
		case DA_IDXCONSEC	: Consec	= Value;
		case DA_IDXDISTANCE	: Distance	= Value;
		case DA_IDXGAPTOL	: GapTol	= (float) Value / (float) SHRT_MAX;
		case DA_IDXUSEPORTS	: UsePorts	= Value;
		default: Value = Value; // Nothing
		}
	}
}


