#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include<string.h>
#include <stdlib.h>

void test1() {

	FILE* in, * out;

	char infilename[] = "D:\\bmc\\test\\pat\\123.txt";

	char outfilename[] = "D:\\bmc\\test\\pat\\456.txt";

	if ((in = fopen(infilename, "r")) == NULL)
	{
		printf("无法打开此文件!,请检查路径");
		exit(0);
	}

	if ((out = fopen(outfilename, "w")) == NULL)
	{
		printf("无法打开此文件!,请检查路径");
		exit(0);
	}
	char ch;
	ch = fgetc(in);//读入一个字符
	while (!feof(in))//如果没有读到结束符号
	{

		fputc(ch, out);
		putchar(ch);
		ch = fgetc(in);//继续读取
	}

	putchar(10);
	fclose(in);
	fclose(out);
}

void test2() {
	FILE* in;
	char infilename[] = "D:\\bmc\\pattern\\patten.txt";
	char patten[2252][120];
	if ((in = fopen(infilename, "r")) == NULL)
	{
		printf("无法打开此文件!,请检查路径");
		exit(0);
	}
	int index = 0;
	char* find = NULL;
	for (int i = 0; i < 2252; i++) {

		fgets(patten[i], 120, in);
		find = strchr(patten[i], '\n');
		if (find)
			*find = '\0';
	}

	for (int i = 0; i < 2252; i++) {

		printf("%s\n", patten[i]);
	}
}

int* test3() {

	int a[10] = { 0 };

	return a;
}
int test_x(int*a,int i) {

	printf("%d\n", *(a + i));
	printf("%d\n", a[i]);
	return 0;
}

int test4() {

	int a[] = { 1,3,5,7,9 };
	int b[] = { 2,4,6,8,10 };
	int* q = a;
	int* p = b;
	int** c=new int *[2];
	int d[10][5] = {0};
	c[0] = q;
	c[1] = b;
	/*for (int i = 0; i < 2; i++) {
	   
		for (int j = 0; j < 5; j++) {

			printf("%d ", c[i][j]);
		}
		printf("\n");
	}*/

	int* x = c[0]; //从指针数组里面取出指针 将指针传到函数里调用

	test_x(x, 1);
	
	


	printf("%d ", c[1][4]);
	delete[] c;
	return 0;
	

}

int* test5() {

	int a = 10;
	return &a;
}

int test6(int *array,int **b) {

	for (int i = 0; i < 10; i++) {
		array[i] += 2;
		b[i] = array;
	}
	return 0;

}

int test7(int* a) {

	for (int i = 0; i < 5; i++)
	{
		a[i]++;
	}
	return 0;
}

void getLen(const char **pat) {

	for (int i = 0; i < 5; i++) {
		const char* c = pat[i];
		printf("%d\n", strlen(c));
	}
	
	
	
}
void getLen1(const char **pat, int* plen) {


	for (int i = 0; i < 5; i++) {
	
		const char* c = pat[i];
		plen[i] = strlen(c);//获取长度
	}
}
int main() {

	//test2();
	/*int *p=test3();
	for (int i=0;i<10;i++)
	{
		printf("%d", *(p+i));
	}*/

	/*int* p[2];
	for (int i = 0; i < 2; i++) {
		int* q = test5();
		p[i] = q;
	}

	int* a = p[0];
	printf("%d\n", *a);
	int* b = p[1];
	printf("%d\n", *a);*/

/*	int** b = new int* [2];
	int c[] = {1, 2, 3, 4, 5};
	int d[] = { 6,7,8,9,10 };
	b[0] = c;
	b[1] = d;*/

	/*int a[2][10] = { {1,2,3,4,5},
		             {6,7,8,9,10}};*/

	/*for (int i = 0; i < 2; i++) {
		test7(b[i]);
	}
	
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 5; j++) {
		
			printf("%d ", b[i][j]);
		}
		printf("\n");
	}*/

 
	const char* a = "Hello";
	const char* b = "Hell";
	const char* c = "Her";
	const char* d = "He";
	const char* e = "H";
	const char** x = new const char* [5];
	x[0] = a;
	x[1] = b;
	x[2] = c;
	x[3] = d;
	x[4] = e;

	int f[5] = { 0 };
	getLen1(x,f);
	for (int i = 0; i < 5; i++) {
	
		printf("%d ", f[i]);
	}

	
}




