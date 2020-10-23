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

//增加了两个参数
inline unsigned int BMNaive(const char* pat, const int plen, unsigned int b_idx, short* stateArray, TokenInfo* tokenList, int dist, int length,int *skip,int *shift)
{
	int blen = b_idx + length;
	//指向指针的参考区域 
	TokenInfo* offsetList = tokenList + b_idx - dist;
	for (b_idx++; b_idx <= blen; )//初始条件+1是为了之后的-1抵消
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
 					//BMScanByte(pat, plen, b_idx, tokenList[--b_idx],skip,shift);
				    BMScanByte(pat, plen, b_idx, tokenList[--b_idx], skip, shift);
  				}
				printf("\n");
			}
		
		}
	
	}
	delete[]array;
}

//bmc算法
int bmc(std::vector<PatternInfo>& patList, TokenInfo** infoArray, int* sizeArray, int count, int nLoop, int skipTable[][PAT_LEN], int shiftTable[][PAT_LEN],int *state)
{
	
	short* array = new short[P_SIZE];
	for (int loop = nLoop; loop > 0; loop--)
	{
		// for 取patterns, print pattern 需要一个二维数组来存放patten; 每次取出一个pattern生成跳跃表
		//指针取出shift skip
		for (int i = 0; i < PAT_NUM; i++) {
			int plen = patList[i].len;
			int* skip = skipTable[i];
			int* shift = shiftTable[i];
			char* pat = patList[i].pat;
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
						//stateArray[b_idx] = BMScanByte(pat, plen, b_idx, tokenList[--b_idx].token, skip, shift,state);
					}
					else							// Search DistCode
					{
						/*int length = tokenList[b_idx].length;	
					    //tokenList+b_idx是当前指针区域
						b_idx = BMNaive(pat, plen, b_idx, stateArray, tokenList, (tokenList + b_idx)->dist, length);*/
						
					}
				}
				printf("\n");
				
				/*
						1、扫描过程记录跳跃点
						2、尾部落入pointr中后，判断比较最后一个字符得到的index值是否和referred中相等，如果不等跳转至3，否则跳转至4.
						3、不相等，比较下一个字符，然后跳转至2，再次判断；
						4、直接从referred向pointer中拷贝index值，
						判断pointer中第一个位置的index是否小于等于1
						if不是
							继续向pointer之前扫描，直至得到不相等的字符，或者匹配到模式
						end if
						从referred中最后一个跳跃点寻找下一个跳跃点，对齐pattern后继续扫描pointer之后的字符串

	*/
			     
			}
			
			
		}

	}
	delete[]array;
	return 0;

}
int BMScanByte(const char* pat, const int plen, unsigned int& b_idx, unsigned char token, int* skip, int* shift, int* state) {

	static short p_idx = plen;	// must be static
	g_scanned++;
	if (token != pat[--p_idx])
		goto label_exit;
	else //相等
	{
		if (p_idx != 0)
			state[b_idx] = plen;
			return p_idx;
		/*else
		{

			g_nMatch++;
			printf("match,b_idx=%d\n", b_idx);
			goto label_exit;
		}*/
	}
label_exit:
	b_idx += (shift[p_idx] > skip[token]) ? shift[p_idx] : skip[token];
	p_idx = plen;
	printf("%d\n", b_idx);
	return p_idx;


}
/*Naive做法：如果字符本身是普通字符 则执行普通的扫描查询
 如果进入了指针内部 通过指针长度计算出指针结束的位置 以此位置为界
 继续进行比对*/
void BMCompressedMatching(std::vector<PatternInfo>& patList, TokenInfo** infoArray, int* sizeArray, int count, int nLoop, int skipTable[][PAT_LEN], int shiftTable[][PAT_LEN])
{
	short* array = new short[P_SIZE];

	for (int loop = nLoop; loop > 0; loop--)
	{
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
	// 					
					    //tokenList+b_idx表示的是在指针的对应位置 tokenList指向文本第一个位置
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