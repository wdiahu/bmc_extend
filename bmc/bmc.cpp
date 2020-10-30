#include "utils.h"
#include "mstring.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int g_skip[PAT_LEN] = { 0 }; 
int g_shift[PAT_LEN] = { 0 };
extern unsigned long  g_nMatch;
extern unsigned long  long g_Total;
extern unsigned long long g_scanned;
short stateArray[P_SIZE];


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

unsigned int BMScanByte(const char* pat, const int plen, unsigned int &b_idx, unsigned char token, int* skip, int* shift)
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
			//printf("match,b_idx=%d\n", b_idx);
			goto label_exit;
		}
	}
label_exit:
	b_idx += (shift[p_idx] > skip[token]) ? shift[p_idx] : skip[token];
	p_idx = plen;
	//printf("%d\n", b_idx);
	return p_idx;
}

//��������������
inline unsigned int BMNaive(const char* pat, const int plen, unsigned int b_idx, short* stateArray, TokenInfo* tokenList, int dist, int length,int *skip,int *shift)
{
	int blen = b_idx + length;
	//ָ��ָ��Ĳο����� 
	TokenInfo* offsetList = tokenList + b_idx - dist;
	for (b_idx++; b_idx <= blen; )//��ʼ����+1��Ϊ��֮���-1����
	{
		// 		stateArray[b_idx] = BMScanByte(pat, plen, b_idx, offsetList[(--b_idx)- dist].token);
		stateArray[b_idx] = BMScanByte(pat, plen, b_idx, tokenList[--b_idx].token,skip,shift);
	}
	return b_idx;
}



void UncompressedMatching(std::vector<PatternInfo>& patList, char** infoArray, int* sizeArray, int count, int nLoop, int skipTable[][PAT_LEN], int shiftTable[][PAT_LEN])
{
	short* array = new short[P_SIZE];
	int num = patList.size();
	for (int loop = nLoop; loop > 0; loop--)
	{
		// for ȡpatterns, print pattern ��Ҫһ����ά���������patten; ÿ��ȡ��һ��pattern������Ծ��
		//ָ��ȡ��shift skip
		for (int i = 0; i < num; i++) {
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
 					//BMScanByte(pat, plen, b_idx, tokenList[--b_idx],skip,shift);
				    BMScanByte(pat, plen, b_idx, tokenList[--b_idx], skip, shift);
  				}
				//printf("\n");
			}
		
		}
	
	}
	delete[]array;
}

/*Naive����������ַ���������ͨ�ַ� ��ִ����ͨ��ɨ���ѯ
 ���������ָ���ڲ� ͨ��ָ�볤�ȼ����ָ�������λ�� �Դ�λ��Ϊ��
 �������бȶ�*/
void BMCompressedMatching(std::vector<PatternInfo>& patList, TokenInfo** infoArray, int* sizeArray, int count, int nLoop, int skipTable[][PAT_LEN], int shiftTable[][PAT_LEN])
{
	short* array = new short[P_SIZE];
	int num = patList.size();
	for (int loop = nLoop; loop > 0; loop--)
	{
		for (int i = 0; i < num; i++) { //ȡÿһ��ģʽ
			int plen = patList[i].len;
			int* skip = skipTable[i];
			int* shift = shiftTable[i];
			char* pat = patList[i].pat;
			//printf("g_scanned = %lld    g_Total =%lld \n ", g_scanned, g_Total * (i+1));
			for (int m = 0; m < count; m++)	// ȡ��ҳ�ļ�
			{
				//printf("��%d���ļ�\n", m);
				TokenInfo* tokenList = infoArray[m];//�ļ���������
				short* stateArray = array + 1;//��¼״̬
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
	// 					
					    //tokenList+b_idx��ʾ������ָ��Ķ�Ӧλ�� tokenListָ���ı���һ��λ�� һֱ�Ǵ���ָ�� �����������ѭ��
						b_idx = BMNaive(pat, plen, b_idx, stateArray, tokenList, (tokenList + b_idx)->dist, length,skip,shift);
					}	
				}
				//printf("\n");
			}

		}
	}
	delete[]array;
}
//bmc�㷨  
/*
int bmc(std::vector<PatternInfo>& patList, TokenInfo** infoArray, int* sizeArray, int count, int nLoop, int skipTable[][PAT_LEN], int shiftTable[][PAT_LEN])
{

	
	for (int loop = nLoop; loop > 0; loop--)
	{
		for (int i = 0; i < PAT_NUM; i++) {
			int plen = patList[i].len;
			int* skip = skipTable[i];
			int* shift = shiftTable[i];
			char* pat = patList[i].pat;
			for (int m = 0; m < count; m++)	// ȡ��ҳ�ļ�  
			{
				memset(stateArray, -1, P_SIZE * sizeof(short));
				TokenInfo* tokenList = infoArray[m];//�ļ���������
				//short* stateArray = array;//��¼״̬
				int blen = sizeArray[m];//blen ĳ���ļ��ĳ���
				unsigned int b_idx = plen;
				while (b_idx <= blen)
				{
					if (tokenList[--b_idx].length == 0)	// Search Literal
					{
						//�洢״̬ ÿ��ɨ�趼Ӧ�ü�¼currentIndex ��Ϊb_idx�ᷢ���仯
						int currentIndex = b_idx;//��¼��ǰλ�� ��Ϊ�������ܻὫb_idx�ı� 
						stateArray[currentIndex]=BMScanByte_Ex(pat, plen, b_idx, tokenList[b_idx].token, skip, shift);
					}
					else					 		   // Search DistCode
					{     
						TokenInfo * offsetList = tokenList + b_idx - (tokenList+b_idx)->dist;//offsetListָ��ref�еĶ�Ӧλ��
						int refIndex = b_idx - (tokenList + b_idx)->dist;//��Ӧ���±�
						stateArray[b_idx]=BMScanByte_Ex(pat, plen, b_idx, tokenList[b_idx].token, skip, shift);//ɨ���λ�� 
						//�ж���Ȳ���Ҫ��¼λ�� ��Ϊ�����ת�˱ض������������if����
						if (stateArray[b_idx] == stateArray[refIndex]) //״̬һ��
						{
							//ֱ�ӴӲο������� �����������pointer��һ�������Ƿ�С�ڵ���1 �����������ǰɨ��
							int ptrStart=findPtrStart(b_idx, tokenList);

							int move = b_idx - ptrStart;//�ƶ�����
						
							memcpy(stateArray + b_idx - move, stateArray + refIndex - move, move*sizeof(int));//����״̬

							//�����������ָ���һ��λ�õ�index
							if (stateArray[ptrStart] > 1) {
							
								b_idx = ptrStart-1;
								//ͬʱģʽ����λ��ҲӦ�øı�
								
								//����ɨ�� 
								stateArray[ptrStart-1]=BMScanByte_Ex(pat, plen, b_idx, tokenList[b_idx].token, skip, shift);
								
							}
						}
					}
				}
				printf("\n");
			}
		}
	}
	return 0;

}
unsigned int NewMethod(const char* pat, const int plen, unsigned int b_idx, short* stateArray, TokenInfo* tokenList, int dist, int length, int* skip, int* shift)
{
	int blen = b_idx + length;
	//ָ��ָ��Ĳο����� 
	TokenInfo* offsetList = tokenList + b_idx - dist;
	for (b_idx++; b_idx <= blen; )//��ʼ����+1��Ϊ��֮���-1���� 
	{
		// 		stateArray[b_idx] = BMScanByte(pat, plen, b_idx, offsetList[(--b_idx)- dist].token);
		stateArray[b_idx] = BMScanByte_Ex(pat, plen, b_idx, tokenList[--b_idx].token, skip, shift);//��¼״̬
	}
	return b_idx;
}

int findPtrStart(int b_idx, TokenInfo* info) //�����Ȳ�Ϊ0ʱ һֱ��ǰ���� ��ָ��ǰһ��λ��ͣ��
{
	while (info[b_idx].length!=0)
	{
		--b_idx;
	}

	return b_idx + 1;
}

int BMScanByte_Ex(const char* pat, const int plen, unsigned int& b_idx, unsigned char token, int* skip, int* shift) {

	static short p_idx = plen;	// must be static
	g_scanned++;
	if (token != pat[--p_idx]) //����
	{
		b_idx += (shift[p_idx] > skip[token]) ? shift[p_idx] : skip[token];
		p_idx = plen;
		printf("%d\n", b_idx);
		return defaultValue; //���ƥ�䵽�� ���߷�������� ����Ĭ��ֵ-1
	}
	else //���   ��p_idxΪ0ʱ��Ϊ�ҵ�һ��ƥ�� ��Ϊ0ʱֱ�ӷ��� p_idx +1
	{
		
		if(p_idx==0) //p_idxΪ0 ��ʾ�ߵ�ģʽ��һ��λ���Ҵ������ �Ѿ��ҵ�ƥ����
		{    
			g_nMatch++;
			printf("match,b_idx=%d\n", b_idx);
			b_idx += (shift[p_idx] > skip[token]) ? shift[p_idx] : skip[token];
			p_idx = plen;
			printf("%d\n", b_idx);
			
		}

		return p_idx + 1;
	}



}


*/



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