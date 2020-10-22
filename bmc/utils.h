#ifndef _UTILS_H_
#define _UTILS_H_
#include <vector>
#define PAT_LEN 256
#define P_SIZE	3200000
#define PAT_NUM 1
extern unsigned long g_scanned, g_nMatch;

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
// 	union
// 	{
		unsigned short dist;
		unsigned short length;
// 	};
}TokenInfo;

void PrintResult(int loop);
int gettimeofday(struct timeval* tp, void* tzp);
void KMPCompressedMatching(const char* pat, const int plen, TokenInfo** infoArray, int* sizeArray, int count, int nLoop);

void GetNext(const char* pat, int* next);
int KMPScanByte(const char* pat, const int plen, short &curState, char token);
short MethodKMP(const char* pat, const int plen, short curState, short* stateArray, TokenInfo* tokenList, int dist, int length);
short KmpNaive(const char* pat, const int plen, short curState, short* stateArray, TokenInfo* tokenList, int dist, int length);
//原本就有的
int BMScanByte(const char* pat, const int plen, short &curState, char token);

//试验
int BMScanByte(const char* pat, const int plen, unsigned int& b_idx, unsigned char token, int* skip,int * shift);

void BMCompressedMatching(const char* pat, const int plen, TokenInfo** infoArray, int* sizeArray, int count, int nLoop);

//void BMUnCompressMatching(const char* pat, const int plen, TokenInfo** infoArray, int* sizeArray, int count, int nLoop);
void BMCompressedMatching(char* pat, const int plen, char** infoArray, int* sizeArray, int count, int nLoop);

//void BMCompressedMatching(char* pat, const int plen, char** infoArray, int* sizeArray, int count, int nLoop,int **skipTable,int **shiftTable);

//将模式串和长度更改了
void UncompressedMatching(std::vector<PatternInfo>& patList, char** infoArray, int* sizeArray, int count, int nLoop, int skipTable[][PAT_LEN], int shiftTable[][PAT_LEN]);

void BMCompressedMatching(std::vector<PatternInfo>& patList,TokenInfo** infoArray, int* sizeArray, int count, int nLoop, int skipTable[][PAT_LEN], int shiftTable[][PAT_LEN]);

int BMScanByte_bak(const char* pat, const int plen, unsigned int& b_idx, unsigned char token, int* skip, int* shift);
void readPatten(std::vector<PatternInfo>& patList);

int uncompressTest();//对于普通字符串的测试
#endif	// _UTILS_H_
