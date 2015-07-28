
//构建一个存储器模型
//@这里使用C语言的系统函数在系统层之上构建了一个简单的存储器模型
//@用于后面算法仿真

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "memModel.h"

static char *memHeap;		//堆头部指针
static char *memBrk;		//当前堆尾部+1位置指针
static char *memMaxAddr;	//堆上限+1位置指针

void memInit(void)
{
	memHeap = (char*)malloc(MAX_HEAP);
	if (memHeap == NULL)
	{
		fprintf_s(stderr,"内存空间不足，无法得到足够多的虚拟内存！\n");
		exit(1);
	}

	memBrk = (char*)memHeap;
	memMaxAddr = (char*)(memHeap + MAX_HEAP);
}

void *memSbrk(int incr)
{
	char *oldBrk = memBrk;
	if ((incr < 0)||((memBrk + incr) > memMaxAddr))
	{
		errno = ENOMEM;
		fprintf_s(stderr,"错误：memSbrk失败，内存空间不足...\n");
		return (void*)-1;
	}
	memBrk += incr;

	return (void*)oldBrk;
}