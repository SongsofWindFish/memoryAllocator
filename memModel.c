
//����һ���洢��ģ��
//@����ʹ��C���Ե�ϵͳ������ϵͳ��֮�Ϲ�����һ���򵥵Ĵ洢��ģ��
//@���ں����㷨����

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "memModel.h"

static char *memHeap;		//��ͷ��ָ��
static char *memBrk;		//��ǰ��β��+1λ��ָ��
static char *memMaxAddr;	//������+1λ��ָ��

void memInit(void)
{
	memHeap = (char*)malloc(MAX_HEAP);
	if (memHeap == NULL)
	{
		fprintf_s(stderr,"�ڴ�ռ䲻�㣬�޷��õ��㹻��������ڴ棡\n");
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
		fprintf_s(stderr,"����memSbrkʧ�ܣ��ڴ�ռ䲻��...\n");
		return (void*)-1;
	}
	memBrk += incr;

	return (void*)oldBrk;
}