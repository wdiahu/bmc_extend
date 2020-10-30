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



void UncompressedMatching(std::vector<PatternInfo>& patList, char** infoArray, int* sizeArray, int count, int nLoop, int skipTable[][PAT_LEN], int shiftTable[][PAT_LEN])
{
	short* array = new short[P_SIZE];
	int num = patList.size();
	for (int loop = nLoop; loop > 0; loop--)
	{
		// for 取patterns, print pattern 需要一个二维数组来存放patten; 每次取出一个pattern生成跳跃表
		//指针取出shift skip
		for (int i = 0; i < num; i++) {
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
				//printf("\n");
			}
		
		}
	
	}
	delete[]array;
}

/*Naive做法：如果字符本身是普通字符 则执行普通的扫描查询
 如果进入了指针内部 通过指针长度计算出指针结束的位置 以此位置为界
 继续进行比对*/
void BMCompressedMatching(std::vector<PatternInfo>& patList, TokenInfo** infoArray, int* sizeArray, int count, int nLoop, int skipTable[][PAT_LEN], int shiftTable[][PAT_LEN])
{
	short* array = new short[P_SIZE];
	int num = patList.size();
	for (int loop = nLoop; loop > 0; loop--)
	{
		for (int i = 0; i < num; i++) { //取每一个模式
			int plen = patList[i].len;
			int* skip = skipTable[i];
			int* shift = shiftTable[i];
			char* pat = patList[i].pat;
			//printf("g_scanned = %lld    g_Total =%lld \n ", g_scanned, g_Total * (i+1));
			for (int m = 0; m < count; m++)	// 取网页文件
			{
				//printf("第%d个文件\n", m);
				TokenInfo* tokenList = infoArray[m];//文件本身内容
				short* stateArray = array + 1;//记录状态
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
					    //tokenList+b_idx表示的是在指针的对应位置 tokenList指向文本第一个位置 一直是处理指针 处理结束跳出循环
						b_idx = BMNaive(pat, plen, b_idx, stateArray, tokenList, (tokenList + b_idx)->dist, length,skip,shift);
					}	
				}
				//printf("\n");
			}

		}
	}
	delete[]array;
}
//bmc算法  
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
			for (int m = 0; m < count; m++)	// 取网页文件  
			{
				memset(stateArray, -1, P_SIZE * sizeof(short));
				TokenInfo* tokenList = infoArray[m];//文件本身内容
				//short* stateArray = array;//记录状态
				int blen = sizeArray[m];//blen 某个文件的长度
				unsigned int b_idx = plen;
				while (b_idx <= blen)
				{
					if (tokenList[--b_idx].length == 0)	// Search Literal
					{
						//存储状态 每次扫描都应该记录currentIndex 因为b_idx会发生变化
						int currentIndex = b_idx;//记录当前位置 因为函数可能会将b_idx改变 
						stateArray[currentIndex]=BMScanByte_Ex(pat, plen, b_idx, tokenList[b_idx].token, skip, shift);
					}
					else					 		   // Search DistCode
					{     
						TokenInfo * offsetList = tokenList + b_idx - (tokenList+b_idx)->dist;//offsetList指向ref中的对应位置
						int refIndex = b_idx - (tokenList + b_idx)->dist;//对应的下标
						stateArray[b_idx]=BMScanByte_Ex(pat, plen, b_idx, tokenList[b_idx].token, skip, shift);//扫描该位置 
						//判断相等不必要记录位置 因为如果跳转了必定不会产生下面if条件
						if (stateArray[b_idx] == stateArray[refIndex]) //状态一致
						{
							//直接从参考串拷贝 拷贝结束检查pointer第一个区域是否小于等于1 不是则继续往前扫描
							int ptrStart=findPtrStart(b_idx, tokenList);

							int move = b_idx - ptrStart;//移动距离
						
							memcpy(stateArray + b_idx - move, stateArray + refIndex - move, move*sizeof(int));//拷贝状态

							//拷贝结束检查指针第一个位置的index
							if (stateArray[ptrStart] > 1) {
							
								b_idx = ptrStart-1;
								//同时模式串的位置也应该改变
								
								//继续扫描 
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
	//指向指针的参考区域 
	TokenInfo* offsetList = tokenList + b_idx - dist;
	for (b_idx++; b_idx <= blen; )//初始条件+1是为了之后的-1抵消 
	{
		// 		stateArray[b_idx] = BMScanByte(pat, plen, b_idx, offsetList[(--b_idx)- dist].token);
		stateArray[b_idx] = BMScanByte_Ex(pat, plen, b_idx, tokenList[--b_idx].token, skip, shift);//记录状态
	}
	return b_idx;
}

int findPtrStart(int b_idx, TokenInfo* info) //当长度不为0时 一直往前遍历 在指针前一个位置停下
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
	if (token != pat[--p_idx]) //不等
	{
		b_idx += (shift[p_idx] > skip[token]) ? shift[p_idx] : skip[token];
		p_idx = plen;
		printf("%d\n", b_idx);
		return defaultValue; //如果匹配到了 或者发生不相等 返回默认值-1
	}
	else //相等   且p_idx为0时即为找到一个匹配 不为0时直接返回 p_idx +1
	{
		
		if(p_idx==0) //p_idx为0 表示走到模式第一个位置且处处相等 已经找到匹配了
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