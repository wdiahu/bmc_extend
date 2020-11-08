#include "utils.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
using namespace std;
int g_next[PAT_LEN] = { 0 };
extern unsigned long  g_nMatch;
extern unsigned long long g_scanned;
extern  clock_t tmScan;
extern  clock_t tmProcess;
extern  clock_t nTime;



void GetNext(const char* pat, int* next)
{
	int q, k;//The k record the symmetric value of all prefixes
	int m = strlen(pat);//the length of pattern
	next[0] = 0;//the symmetric value of the first character must be 0
	for (q = 1, k = 0; q < m; ++q)//calculate the symmetry value of each position
	{
		//The k always record the maximum symmetric value of the previous prefix
		while (k > 0 && pat[q] != pat[k])
			// k = next[k - 1];//k cyclically decreasing
			--k;
		if (pat[q] == pat[k])
			k++;//add K
		next[q] = k;//get final value
	}
}

int KMPScanByte(const char* pat, const int plen, short& p_idx, char token,int *next)
{

	int ret = 0;
	while (p_idx > 0 && pat[p_idx] != token)
	{
		p_idx = next[p_idx - 1];
	}

	if (pat[p_idx] == token)
		p_idx++;

	if (p_idx == plen)
	{
	
 		g_nMatch++;
		ret = p_idx;
		p_idx = next[p_idx - 1];//find next match
	}
	return ret == 0 ? p_idx : ret;
}

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
		     //g_misscan++;
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
	for (int loop = nLoop; loop > 0; loop--)
	{
		for (int i = 0; i < num; i++) {
			num1 = 0;
			char* pat = patList[i].pat;
			int plen = patList[i].len;
			int* next = nextTable[i];
			for (int m = 0; m < count; m++)
			{
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
						//If you want to test NewMethod and Naive.Just choose one of them 
					    state = MethodKMP(pat, plen, state, stateArray, tokenList, (tokenList + 1)->dist, length,next);
					    //state = KmpNaive(pat, plen, state, stateArray, tokenList, (tokenList + 1)->dist, length,next);
						tokenList += length;
						stateArray += length;
					}
				}
			}
		


			  
		}

	}

	tmProcess = GetTickCount64();
}


//a demo to test Kmp
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