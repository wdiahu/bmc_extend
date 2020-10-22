#include "utils.h"
#include "mstring.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int g_skip[PAT_LEN] = { 0 }; 
int g_shift[PAT_LEN] = { 0 };
extern unsigned long g_scanned, g_nMatch;


inline int BMScanByte(const char* pat, const int plen, short& b_idx, char token)
{
	static short p_idx = plen;	// must be static
	printf("%d ", p_idx);
	g_scanned++;
	if (token == pat[--p_idx] && p_idx != 0)//最后一个字符相同
		return p_idx;
	else
	{
		if (p_idx == 0)//有一个匹配
		{
			g_nMatch++;
			printf("匹配发生时：b_idx=%d\n", b_idx);
		}

		b_idx += (g_shift[p_idx] > g_skip[token]) ? g_shift[p_idx] : g_skip[token];
		p_idx = plen;
		printf("b_idx=%d\n", b_idx);
		return p_idx;
	}
}

inline int BMScanByte_bak(const char *pat, const int plen, unsigned int &b_idx, unsigned char token, int *skip, int *shift)
{
	static short p_idx = plen;	// must be static
	g_scanned++;
	if (token != pat[--p_idx])
	{
		b_idx += (shift[p_idx] > skip[token]) ? shift[p_idx] : skip[token];
		p_idx = plen;
		return p_idx;
	}
	else
	{
		if (p_idx != 0)
			return p_idx;
		else
		{
			g_nMatch++;
			b_idx += (shift[p_idx] > skip[token]) ? shift[p_idx] : skip[token];
			p_idx = plen;
			return p_idx;
		}
	}
}

int BMScanByte(const char* pat, const int plen, unsigned int &b_idx, unsigned char token, int* skip, int* shift)
{
	static short p_idx = plen;	// must be static
	g_scanned++;
	if (token != pat[--p_idx])
		goto label_exit;
	else
	{
		if (p_idx != 0)
			return p_idx;
		else
		{
			g_nMatch++;
			printf("match,b_idx=%d\n", b_idx);
			goto label_exit;
		}
	}
label_exit:
	b_idx += (shift[p_idx] > skip[token]) ? shift[p_idx] : skip[token];
	p_idx = plen;
	printf("%d\n", b_idx);
	return p_idx;
}

inline short BMNaive(const char* pat, const int plen, short b_idx, short* stateArray, TokenInfo* tokenList, int dist, int length)
{
	int blen = b_idx + length;
	//指向指针的参考区域
	TokenInfo* offsetList = tokenList + b_idx - dist;
	for (b_idx++; b_idx<= blen; )
	{
// 		stateArray[b_idx] = BMScanByte(pat, plen, b_idx, offsetList[(--b_idx)- dist].token);
		stateArray[b_idx] = BMScanByte(pat, plen, b_idx, tokenList[--b_idx].token);
	}
	return b_idx;
}

//增加了两个参数
inline short BMNaive(const char* pat, const int plen, unsigned int b_idx, short* stateArray, TokenInfo* tokenList, int dist, int length,int *skip,int *shift)
{
	int blen = b_idx + length;
	//指向指针的参考区域
	TokenInfo* offsetList = tokenList + b_idx - dist;
	for (b_idx++; b_idx <= blen; )
	{
		// 		stateArray[b_idx] = BMScanByte(pat, plen, b_idx, offsetList[(--b_idx)- dist].token);
		stateArray[b_idx] = BMScanByte(pat, plen, b_idx, tokenList[--b_idx].token,skip,shift);
	}
	return b_idx;
}

/*plen 模式串长度  
* 
infoArray文本内容 

sizeArray文本内容大小 

count记录文件下标

nLoop循环次数*/
void BMCompressedMatching(const char* pat, const int plen, TokenInfo** infoArray, int* sizeArray, int count, int nLoop)
{
	short* array = new short[P_SIZE];

	for (int loop = nLoop; loop > 0; loop--)
	{
		// for 取patterns, print pattern 需要一个二维数组来存放patten; 每次取出一个pattern生成跳跃表
		for (int m = 0; m < count; m++)	// 取网页文件
		{
			TokenInfo* tokenList = infoArray[m];//文件本身内容
			short* stateArray = array;//记录状态
			int blen = sizeArray[m];//blen 某个文件的长度
			short b_idx = plen;
			while (b_idx <= blen)
			{
				if (tokenList[--b_idx].length == 0)	// Search Literal
				{
					stateArray[b_idx] = BMScanByte(pat, plen, b_idx, tokenList[b_idx].token);
				}
				else							// Search DistCode
				{
					int length = tokenList[b_idx].length;
// 					state = MethodKMP(pat, plen, state, stateArray, tokenList, (tokenList + 1)->dist, length);
					//tokenList+b_idx是当前指针区域
					b_idx = BMNaive(pat, plen, b_idx, stateArray, tokenList, (tokenList + b_idx)->dist, length);
				}
			}
			printf("\n");
		}
	}
	delete[]array;
}

void BMCompressedMatching(char* pat, const int plen, char** infoArray, int* sizeArray, int count, int nLoop)
{
	short* array = new short[P_SIZE];

	for (int loop = nLoop; loop > 0; loop--)
	{
		// for 取patterns, print pattern 需要一个二维数组来存放patten; 每次取出一个pattern生成跳跃表
		for (int m = 0; m < count; m++)	// 取网页文件
		{
			char *tokenList = infoArray[m];//文件本身内容
			short* stateArray = array;//记录状态
			int blen = sizeArray[m];//blen 某个文件的长度
			short b_idx = plen;
			while (b_idx <= blen)
			{
			     BMScanByte(pat, plen, b_idx, tokenList[--b_idx]);
			}
			printf("\n");
			
		}
	}
	delete[]array;
}



void UncompressedMatching(std::vector<PatternInfo>& patList, char** infoArray, int* sizeArray, int count, int nLoop, int skipTable[][PAT_LEN], int shiftTable[][PAT_LEN])
{
	short* array = new short[P_SIZE];

	for (int loop = nLoop; loop > 0; loop--)
	{
		// for 取patterns, print pattern 需要一个二维数组来存放patten; 每次取出一个pattern生成跳跃表
		//指针取出shift skip
		for (int i = 0; i < PAT_NUM; i++) {
			int plen = patList[i].len;
			int* skip = skipTable[i];
			int* shift =shiftTable[i];
			char* pat = patList[i].pat;
			for (int m = 0; m < count; m++)	// 取网页文件  
			{
				//printf("matching %d file\n", m);
				char* tokenList = infoArray[m];//文件本身内容
				short* stateArray = array;//记录状态
				int blen = sizeArray[m];//blen 某个文件的长度
				unsigned int b_idx = plen;
				//mSearch_original(tokenList, blen, pat, plen, skip, shift);
 		        while (b_idx <= blen)
 				{
 					BMScanByte(pat, plen, b_idx, tokenList[--b_idx],skip,shift);
				    //BMScanByte_bak(pat, plen, b_idx, tokenList[--b_idx], skip, shift);
  				}
				printf("\n");
			}
		
		}
	
	}
	delete[]array;
}

void BMCompressedMatching(std::vector<PatternInfo>& patList, TokenInfo** infoArray, int* sizeArray, int count, int nLoop, int skipTable[][PAT_LEN], int shiftTable[][PAT_LEN])
{
	short* array = new short[P_SIZE];

	for (int loop = nLoop; loop > 0; loop--)
	{
		// for 取patterns, print pattern 需要一个二维数组来存放patten; 每次取出一个pattern生成跳跃表
		for (int i = 0; i < PAT_NUM; i++) { //取每一个模式
			int plen = patList[i].len;
			int* skip = skipTable[i];
			int* shift = shiftTable[i];
			char* pat = patList[i].pat;
			for (int m = 0; m < count; m++)	// 取网页文件
			{
				TokenInfo* tokenList = infoArray[m];//文件本身内容
				short* stateArray = array;//记录状态
				int blen = sizeArray[m];//blen 某个文件的长度
				unsigned int b_idx = plen;
				while (b_idx <= blen)
				{
					if (tokenList[--b_idx].length == 0)	// Search Literal
					{
						stateArray[b_idx] = BMScanByte(pat, plen, b_idx, tokenList[b_idx].token,skip,shift);
					}
					else							// Search DistCode
					{
						int length = tokenList[b_idx].length;
	// 					state = MethodKMP(pat, plen, state, stateArray, tokenList, (tokenList + 1)->dist, length);
					    //tokenList+b_idx是当前指针区域
						b_idx = BMNaive(pat, plen, b_idx, stateArray, tokenList, (tokenList + b_idx)->dist, length,skip,shift);
					}
				}
				printf("\n");
			}

		}
	}
	delete[]array;
}




/*buf是源串 blen是源串长度 pat是模式串 plen是模式串长度 */
int mSearch(const char* buf, int blen, const char* pat, int plen)
{
	short b_idx = plen;
	while (b_idx <= blen)
	{
		
		BMScanByte(pat, plen, b_idx, buf[--b_idx]);
	}

	printf("Scanning, matches:%d, compares:%d\n", g_nMatch, g_scanned);
	return 0;
}

/*int testBM()
{
	char buf[] = "HERE IS A SIMPLE EXAMPLE, JUST A EXAMPLE WITHOUT ANY OTHERS";
	char pat[] = "MPLE";
	make_skip(pat, strlen(pat), g_skip);//坏字符表
	make_shift(pat, strlen(pat), g_shift);//好后缀表
	mSearch(buf, strlen(buf), pat, strlen(pat));
// 	mSearch_original(buf, strlen(buf), pat, strlen(pat), g_skip, g_shift);
	// 	mSearch_original(buf, sizeof(buf) - 1, pat, sizeof(pat) - 1, skip, shift);
	// 	mContainsSubstr(buf, sizeof(buf) - 1, pat, sizeof(pat) - 1);
	return 0;
}*/