#include "utils.h"
#include <string.h>
#include <stdio.h>

int g_next[PAT_LEN] = { 0 };
extern unsigned long g_scanned, g_nMatch;

void GetNext(const char* pat, int* next)
{
	int q, k;//k��¼����ǰ׺�ĶԳ�ֵ
	int m = strlen(pat);//ģʽ�ַ����ĳ���
	next[0] = 0;//���ַ��ĶԳ�ֵ�϶�Ϊ0
	for (q = 1, k = 0; q < m; ++q)//����ÿһ��λ�õĶԳ�ֵ
	{
		//k����������¼��һ��ǰ׺�����Գ�ֵ
		while (k > 0 && pat[q] != pat[k])
			// k = next[k - 1];//k��ѭ���ݼ���ֵ��ע�����next[k]<k���ǳ���
			--k;
		if (pat[q] == pat[k])
			k++;//����k��Ψһ����
		next[q] = k;//��ȡ����ֵ
	}

	for (int k = 0; k < m; k++)
		printf("%d ", next[k]);
	printf("\n");
}

int KMPScanByte(const char* pat, const int plen, short &p_idx, char token)
{
	g_scanned++;
	int ret = 0;
	while (p_idx > 0 && pat[p_idx] != token)
	{
		p_idx = g_next[p_idx - 1];
	}

	if (pat[p_idx] == token)
		p_idx++;

	if (p_idx == plen)
	{
		g_nMatch++;
		ret = p_idx;
		p_idx = g_next[p_idx - 1];//Ѱ����һ��ƥ��
	}
	return ret == 0 ? p_idx : ret;
}
short MethodKMP(const char* pat, const int plen, short curState, short* stateArray, TokenInfo* tokenList, int dist, int length)
{
// 	short* offState = stateArray - dist;
// 	*stateArray = curState = KMPScanByte(pat, plen, curState, tokenList->token);
// 	stateArray++; tokenList++;

	short* offState = stateArray - dist - 1;
	for (int curPos = 0; curPos < length; curPos++, tokenList++, stateArray++, offState++)
	{
		if (curState == *offState)
		{
// 			if (dist < length)
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
				memcpy(stateArray, offState + 1, sizeof(short) * (length - curPos));
			}
			return offState[length - curPos];
		}
		else
		{
			// g_misscan++;
			*stateArray = KMPScanByte(pat, plen, curState, tokenList->token);
		}
	}
	return curState;
}

short KmpNaive(const char* pat, const int plen, short curState, short* stateArray, TokenInfo* tokenList, int dist, int length)
{
	TokenInfo* offsetList = tokenList - dist;
	for (int k = 0; k < length; k++, offsetList++/*, tokenList++*/)
	{
		*stateArray = KMPScanByte(pat, plen, curState, offsetList->token);
	}

	return curState;
}

void KMPCompressedMatching(const char* pat, const int plen, TokenInfo** infoArray, int* sizeArray, int count, int nLoop)
{
	short* array = new short[P_SIZE];

	for (int loop = nLoop; loop > 0; loop--)
	{
		for (int m = 0; m < count; m++)
		{
			TokenInfo* tokenList = infoArray[m];
			const TokenInfo* const endPtr = tokenList + sizeArray[m];
			short* stateArray = array;

			short state = 0;
			while (tokenList < endPtr)
			{
				if (tokenList->length == 0)	// Search Literal
				{
					*stateArray = KMPScanByte(pat, plen, state, tokenList->token);
					tokenList++;
					stateArray++;
				}
				else							// Search DistCode
				{
					int length = tokenList->length;
// 					state = MethodKMP(pat, plen, state, stateArray, tokenList, (tokenList + 1)->dist, length);
					state = KmpNaive(pat, plen, state, stateArray, tokenList, (tokenList + 1)->dist, length);
					tokenList += length;
					stateArray += length;
				}
			}
		}
	}
	delete[]array;
}

void testKMP()
{
// 	char buf[] = "HERE IS A SIMPLE EXAMPLE, JUST A EXAMPLE WITHOUT ANY OTHERS";
// 	char pat[] = "EXAMPLE";

	char buf[] = "ccccccccabccccccddccc";
	char pat[] = "ccc";

	printf("matches=%d\n", g_nMatch);
}