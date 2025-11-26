#define GW_DEFSAMPLING		8000
#define GW_DEFHISTORY		7
#define GW_DEFEPOCH			32
#define GW_DEFCONSEC		3
#define GW_DEFCHANNEL		8			// Default number of channels
#define GW_DEFFILENAME		128
#define GW_DEFDISTANCE		7			// Minimum distance between gaps
#define GW_DEFGAPTOL		0.025		// Statistically determined
#define GW_DEFAMPTOL		819			// 0.025 of SHRT_MAX
#define GW_DEFLOCALMAX		8192		// 0.25 of SHRT_MAX
#define GW_DEFGLOBALMAX		57344		// History * LocalMax

#define GW_IDXSAMPLING		0
#define GW_IDXHISTORY		1
#define GW_IDXEPOCH			2
#define GW_IDXCONSEC		3
#define GW_IDXCHANNEL		4
#define GW_IDXDISTANCE		5
#define GW_IDXGAPTOL		6
#define GW_IDXUSEPORTS		7


// Data Structures
// LocalMax
// <---- Channel 1 -----><---- Channel 2 -----><---- Channel 3 ----->
// <H1 H2 H3 H4 H5 H6 H7><H1 H2 H3 H4 H5 H6 H7><H1 H2 H3 H4 H5 H6 H7>

// Buffer
// <---- Samples --------------------------------------------------->
// <CH1 CH2 CH3 CH4 CH5 CH6 CH7 CH8><CH1 CH2 CH3 CH4 CH5 CH6 CH7 CH8>


// Function used to initialize the class
int GWInitialize( char f[] );
void GWProcess( short *b );

// This class is exported from the GapWatch.dll
class CGapWatch {
private:
	int Sampling;
	int History;
	int Epoch;
	int EpochSize;
	float GapTol;
	int Consec;
	int ActiveChannel;
	int Distance;
	int *Counter;
	int *GlobalMax;
	float *Position;
	float *Previous;
	float *Last;
	short *Buffer;
	short *LocalMax;
	short *EpochMax;
	short *AmpTol;
	short *NumConsec;
	char **Path;
	int UsePorts;

public:
	CGapWatch(void);
	~CGapWatch(void);

	// TODO: add your methods here.
	int Channel;
	Update(void);
	Find(void);
	Process(short *b);
	Modify(int m, int Value);
	SetPath(int c, char *f);

	int Print(void);
};


