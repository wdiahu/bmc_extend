#ifndef _UTILS_H_
#define _UTILS_H_

#include <windows.h>
#include <vector>

#define PAT_LEN 256
#define P_SIZE	3200000
#define PAT_NUM 226
#define Uncompress false
#define Compress true
#define defaultValue -1

extern unsigned long  g_nMatch;
extern unsigned long long g_scanned;

typedef struct _MemBuffer
{
	char *pBuffer;
	size_t nSize;
}MemBuffer;

typedef struct _PatternInfo
{
	char pat[PAT_LEN];
	short len;
}PatternInfo;


typedef struct
{
	unsigned short token;
 	union
 	{
		unsigned short dist;
		unsigned short length;
 	};
}TokenInfo;

void testKMP();

void PrintResult(int loop,int count);

int gettimeofday(struct timeval* tp, void* tzp);

void KMPCompressedMatching(std::vector<PatternInfo>& patList, TokenInfo** infoArray, int* sizeArray, int nextTable[][PAT_LEN],int count, int nLoop);

void GetNext(const char* pat, int* next);

int KMPScanByte(const char* pat, const int plen, short &curState, char token,int *next);

int MethodKMP(const char* pat, const int plen, short curState, short* stateArray, TokenInfo* tokenList, int dist, int length,int *next);

int KmpNaive(const char* pat, const int plen, short curState, short* stateArray, TokenInfo* tokenList, int dist, int length,int *next);

int BMScanByte(const char* pat, const int plen, short &curState, char token);

unsigned int BMScanByte(const char* pat, const int plen, unsigned int& b_idx, unsigned char token, int* skip,int * shift);

unsigned int BMNaive(const char* pat, const int plen, unsigned int b_idx, short* stateArray, TokenInfo* tokenList, int dist, int length, int* skip, int* shift);

void UncompressedMatching(std::vector<PatternInfo>& patList, char** infoArray, int* sizeArray, int count, int nLoop, int skipTable[][PAT_LEN], int shiftTable[][PAT_LEN]);

void BMCompressedMatching(std::vector<PatternInfo>& patList,TokenInfo** infoArray, int* sizeArray, int count, int nLoop, int skipTable[][PAT_LEN], int shiftTable[][PAT_LEN]);


int BMScanByte_bak(const char* pat, const int plen, unsigned int& b_idx, unsigned char token, int* skip, int* shift);

void readPattern(std::vector<PatternInfo>& patList,char *dir);

int uncompressTest(char* dir, char* patFile, int loop);

int compressTestByNaive(char *dir,char *patFile,int loop);

int processPatternBm(std::vector<PatternInfo>& patList);

int processPatternKmp(std::vector<PatternInfo>& patList);

int readText(char *dir,bool option, std::vector<MemBuffer>& m_vecBuf);//处理文本 option 1 代表 压缩数据 0 代表普通数据

int processText(std::vector<MemBuffer>& m_vecBuf,char** infoArray,int * sizeArray,int count);

int processText(std::vector<MemBuffer>& m_vecBuf, TokenInfo** infoArray, int* sizeArray, int count);

int clearArray(std::vector<MemBuffer>& m_vecBuf, char** infoArray, int* sizeArray,int count);

int clearArray(std::vector<MemBuffer>& m_vecBuf, TokenInfo** infoArray, int* sizeArray, int count);

int compressByKmp(char* szDir, char* patFile, int loop);

#endif	// _UTILS_H_
