#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
using namespace std;
int num1 = 0;
int g_next[PAT_LEN] = { 0 };
extern unsigned long  g_nMatch;
extern unsigned long long g_scanned;
extern  clock_t tmScan;
extern  clock_t tmProcess;
extern  clock_t nTime;
void GetNext(const char* pat, int* next)
{
	int q, k;//k记录所有前缀的对称值
	int m = strlen(pat);//模式字符串的长度
	next[0] = 0;//首字符的对称值肯定为0
	for (q = 1, k = 0; q < m; ++q)//计算每一个位置的对称值
	{
		//k总是用来记录上一个前缀的最大对称值
		while (k > 0 && pat[q] != pat[k])
			// k = next[k - 1];//k将循环递减，值得注意的是next[k]<k总是成立
			--k;
		if (pat[q] == pat[k])
			k++;//增加k的唯一方法
		next[q] = k;//获取最终值
	}

	//for (int k = 0; k < m; k++)
		//printf("%d ", next[k]);
	//printf("\n");
}

int KMPScanByte(const char* pat, const int plen, short& p_idx, char token,int *next)
{
 	g_scanned++;
	int ret = 0;
	while (p_idx > 0 && pat[p_idx] != token)
	{
		p_idx = next[p_idx - 1];
	}

	if (pat[p_idx] == token)
		p_idx++;

	if (p_idx == plen)
	{
		//printf("match\n");
		//printf("match\n");
		//num1++;
 		g_nMatch++;
		ret = p_idx;
		p_idx = next[p_idx - 1];//寻找下一个匹配
	}
	return ret == 0 ? p_idx : ret;
}

//新算法
int MethodKMP(const char* pat, const int plen, short curState,short* stateArray, TokenInfo* tokenList, int dist, int length ,int *next)
{
	// 	short* offState = stateArray - dist;
	// 	*stateArray = curState = KMPScanByte(pat, plen, curState, tokenList->token);
	// 	stateArray++; tokenList++;

	short* offState = stateArray - dist - 1;
	for (int curPos = 0; curPos < length; curPos++, tokenList++, stateArray++, offState++)
	{
		if (curState == *offState)
		{
			//if (dist < length)
			if (1)
			{
				for (int k = 0; k < length - curPos; k++)
				{
					stateArray[k] = offState[k + 1];
					if (stateArray[k] == plen)
 					{
						//num1++;
						g_nMatch++;
 					}
				}
			}
			else
			{
				memcpy(stateArray, offState + 1, sizeof(short) * (length  -  curPos));
			}
			return offState[length - curPos];
		}
		else
		{
			// g_misscan++;
			*stateArray = KMPScanByte(pat, plen, curState, tokenList->token,next);
		}
	}
	return curState;
}

int KmpNaive(const char* pat, const int plen, short curState, short* stateArray, TokenInfo* tokenList, int dist, int length,int *next)
{
	TokenInfo* offsetList = tokenList - dist;
	for (int k = 0; k < length; k++, offsetList++/*, tokenList++*/)
	{
		*stateArray = KMPScanByte(pat, plen, curState, offsetList->token,next);
	}

	return curState;
}


void KMPCompressedMatching(std::vector<PatternInfo>& patList,TokenInfo** infoArray, int* sizeArray, int nextTable[][PAT_LEN], int count, int nLoop)
{

	short* array = new short[P_SIZE];
	int num = patList.size();

	tmScan = GetTickCount64();
	//printf("start\n");
	for (int loop = nLoop; loop > 0; loop--)
	{
		for (int i = 0; i < num; i++) {
			//printf("%d模式:\n", (i + 1));
			num1 = 0;
			char* pat = patList[i].pat;
			int plen = patList[i].len;
			int* next = nextTable[i];
			for (int m = 0; m < count; m++)
			{
				//printf("第%d个文本:\n", (m + 1));
				TokenInfo* tokenList = infoArray[m];
				const TokenInfo* const endPtr = tokenList + sizeArray[m];
				short* stateArray = array ;
				short state = 0;
				while (tokenList < endPtr)
				{
					if (tokenList->length == 0)	// Search Literal
					{
						*stateArray = KMPScanByte(pat, plen, state, tokenList->token,next);
						tokenList++;
						stateArray++;
					}
					else							// Search DistCode
					{
						int length = tokenList->length;
					    state = MethodKMP(pat, plen, state, stateArray, tokenList, (tokenList + 1)->dist, length,next);
					   // state = KmpNaive(pat, plen, state, stateArray, tokenList, (tokenList + 1)->dist, length,next);
						tokenList += length;
						stateArray += length;
					}
				}
			}
		
			if (num1 != 0) {
			
				printf("%s\n",pat);
			
			}

			  
		}

	}

	tmProcess = GetTickCount64();
	//VirtualFree(array, P_SIZE, MEM_RELEASE);
}


void testKMP()
{
		char buf[] = "HERE IS A SIMPLE EXAMPLE, JUST A EXAMPLE WITHOUT ANY OTHERS";
	 	char pat[] = "EXAMPLE";
// 		GetNext(pat, g_next);
// 		PatternInfo f;
// 		f.pat = buf;
// 		f.len =
	//	std::vector<PatternInfo>& patList;
		//patList.push_back();
	//char buf[] = "ccccccccabccccccddccc";
	//char pat[] = "ccc";

	printf("matches=%d\n", g_nMatch);
}