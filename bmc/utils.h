#ifndef _UTILS_H_
#define _UTILS_H_
#include <vector>
#define PAT_LEN 256
#define P_SIZE	3200000
#define PAT_NUM 1
#define Uncompress false
#define Compress true
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
void readPattern(std::vector<PatternInfo>& patList,char *dir);

int uncompressTest(char* dir, char* patFile, int loop);//对于普通字符串的测试

int compressTestByNaive(char *dir,char *patFile,int loop);

int InitializeData(char *dir,bool option);

int processPattern(std::vector<PatternInfo>& patList);//处理模式

int readText(char *dir,bool option, std::vector<MemBuffer>& m_vecBuf);//处理文本 option 1 代表 压缩数据 0 代表普通数据

int processText(std::vector<MemBuffer>& m_vecBuf,char** infoArray,int * sizeArray,int count);
int processText(std::vector<MemBuffer>& m_vecBuf, TokenInfo** infoArray, int* sizeArray, int count);
int clearArray(std::vector<MemBuffer>& m_vecBuf, char** , int* ,int count);
int clearArray(std::vector<MemBuffer>& m_vecBuf, TokenInfo**, int*, int count);
#endif	// _UTILS_H_
