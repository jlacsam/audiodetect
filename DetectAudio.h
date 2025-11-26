#define DA_DEFSAMPLING		8000	// Sampling Rate
#define DA_DEFHISTORY		7	// Seconds
#define DA_DEFEPOCH		32	// Segments Per Second
#define DA_DEFCONSEC		3	// Consecutive Segments
#define DA_DEFCHANNEL		16	// Default number of channels
#define DA_DEFFILENAME		128	// Filename length for output
#define DA_DEFDISTANCE		7	// Minimum distance between gaps, seconds
#define DA_DEFGAPTOL		0.025	// Statistically determined of maximum amplitude
#define DA_DEFAMPTOL		819	// 0.025 of SHRT_MAX
#define DA_DEFLOCALMAX		8192	// 0.25 of SHRT_MAX
#define DA_DEFGLOBALMAX		57344	// History * LocalMax

#define DA_IDXSAMPLING		0
#define DA_IDXHISTORY		1
#define DA_IDXEPOCH		2
#define DA_IDXCONSEC		3
#define DA_IDXCHANNEL		4
#define DA_IDXDISTANCE		5
#define DA_IDXGAPTOL		6
#define DA_IDXUSEPORTS		7


// Data Structures
// LocalMax
// <---- Channel 1 -----><---- Channel 2 -----><---- Channel 3 ----->
// <H1 H2 H3 H4 H5 H6 H7><H1 H2 H3 H4 H5 H6 H7><H1 H2 H3 H4 H5 H6 H7>

// Buffer
// <---- Samples --------------------------------------------------->
// <CH1 CH2 CH3 CH4 CH5 CH6 CH7 CH8><CH1 CH2 CH3 CH4 CH5 CH6 CH7 CH8>


// This class is exported from the GapWatch.dll
class CDetectAudio {
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
	CDetectAudio(void);
	~CDetectAudio(void);

	// Methods
	int Channel;
	Update(void);
	Find(void);
	Process(short *b);
	Modify(int m, int Value);
	SetPath(int c, char *f);

	int Print(void);
};


