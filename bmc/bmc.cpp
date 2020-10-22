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
	if (token == pat[--p_idx] && p_idx != 0)//���һ���ַ���ͬ
		return p_idx;
	else
	{
		if (p_idx == 0)//��һ��ƥ��
		{
			g_nMatch++;
			printf("ƥ�䷢��ʱ��b_idx=%d\n", b_idx);
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
	//ָ��ָ��Ĳο�����
	TokenInfo* offsetList = tokenList + b_idx - dist;
	for (b_idx++; b_idx<= blen; )
	{
// 		stateArray[b_idx] = BMScanByte(pat, plen, b_idx, offsetList[(--b_idx)- dist].token);
		stateArray[b_idx] = BMScanByte(pat, plen, b_idx, tokenList[--b_idx].token);
	}
	return b_idx;
}

//��������������
inline short BMNaive(const char* pat, const int plen, unsigned int b_idx, short* stateArray, TokenInfo* tokenList, int dist, int length,int *skip,int *shift)
{
	int blen = b_idx + length;
	//ָ��ָ��Ĳο�����
	TokenInfo* offsetList = tokenList + b_idx - dist;
	for (b_idx++; b_idx <= blen; )
	{
		// 		stateArray[b_idx] = BMScanByte(pat, plen, b_idx, offsetList[(--b_idx)- dist].token);
		stateArray[b_idx] = BMScanByte(pat, plen, b_idx, tokenList[--b_idx].token,skip,shift);
	}
	return b_idx;
}

/*plen ģʽ������  
* 
infoArray�ı����� 

sizeArray�ı����ݴ�С 

count��¼�ļ��±�

nLoopѭ������*/
void BMCompressedMatching(const char* pat, const int plen, TokenInfo** infoArray, int* sizeArray, int count, int nLoop)
{
	short* array = new short[P_SIZE];

	for (int loop = nLoop; loop > 0; loop--)
	{
		// for ȡpatterns, print pattern ��Ҫһ����ά���������patten; ÿ��ȡ��һ��pattern������Ծ��
		for (int m = 0; m < count; m++)	// ȡ��ҳ�ļ�
		{
			TokenInfo* tokenList = infoArray[m];//�ļ���������
			short* stateArray = array;//��¼״̬
			int blen = sizeArray[m];//blen ĳ���ļ��ĳ���
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
					//tokenList+b_idx�ǵ�ǰָ������
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
		// for ȡpatterns, print pattern ��Ҫһ����ά���������patten; ÿ��ȡ��һ��pattern������Ծ��
		for (int m = 0; m < count; m++)	// ȡ��ҳ�ļ�
		{
			char *tokenList = infoArray[m];//�ļ���������
			short* stateArray = array;//��¼״̬
			int blen = sizeArray[m];//blen ĳ���ļ��ĳ���
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
		// for ȡpatterns, print pattern ��Ҫһ����ά���������patten; ÿ��ȡ��һ��pattern������Ծ��
		//ָ��ȡ��shift skip
		for (int i = 0; i < PAT_NUM; i++) {
			int plen = patList[i].len;
			int* skip = skipTable[i];
			int* shift =shiftTable[i];
			char* pat = patList[i].pat;
			for (int m = 0; m < count; m++)	// ȡ��ҳ�ļ�  
			{
				//printf("matching %d file\n", m);
				char* tokenList = infoArray[m];//�ļ���������
				short* stateArray = array;//��¼״̬
				int blen = sizeArray[m];//blen ĳ���ļ��ĳ���
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
		// for ȡpatterns, print pattern ��Ҫһ����ά���������patten; ÿ��ȡ��һ��pattern������Ծ��
		for (int i = 0; i < PAT_NUM; i++) { //ȡÿһ��ģʽ
			int plen = patList[i].len;
			int* skip = skipTable[i];
			int* shift = shiftTable[i];
			char* pat = patList[i].pat;
			for (int m = 0; m < count; m++)	// ȡ��ҳ�ļ�
			{
				TokenInfo* tokenList = infoArray[m];//�ļ���������
				short* stateArray = array;//��¼״̬
				int blen = sizeArray[m];//blen ĳ���ļ��ĳ���
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
					    //tokenList+b_idx�ǵ�ǰָ������
						b_idx = BMNaive(pat, plen, b_idx, stateArray, tokenList, (tokenList + b_idx)->dist, length,skip,shift);
					}
				}
				printf("\n");
			}

		}
	}
	delete[]array;
}




/*buf��Դ�� blen��Դ������ pat��ģʽ�� plen��ģʽ������ */
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
	make_skip(pat, strlen(pat), g_skip);//���ַ���
	make_shift(pat, strlen(pat), g_shift);//�ú�׺��
	mSearch(buf, strlen(buf), pat, strlen(pat));
// 	mSearch_original(buf, strlen(buf), pat, strlen(pat), g_skip, g_shift);
	// 	mSearch_original(buf, sizeof(buf) - 1, pat, sizeof(pat) - 1, skip, shift);
	// 	mContainsSubstr(buf, sizeof(buf) - 1, pat, sizeof(pat) - 1);
	return 0;
}*/