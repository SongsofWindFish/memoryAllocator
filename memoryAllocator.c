
//���ж�̬�ڴ����Ĵ���

#include <stdio.h>
#include "basic.h"
#include "memModel.h"
#include "memoryAllocator.h"

static char *heap_listp;		//������ָ��		
struct freeListBoundary
{
	char* prev;
	char* next;
}head;						//���п�����ͷ����β��

//����ӿ��ж�����ȡ��
static void fetchFromList(void *bp)
{
	if (GET((char*)bp + WSIZE) != NULL)				//�������һ��
		PUT(GET((char*)bp + WSIZE), GET(bp));		//��һ�������
	if (GET((char*)bp) == (char*)&head)				//��һ��
		head.next = NULL;
	else
		PUT(GET(bp) + WSIZE, (char*)bp + WSIZE);	//��һ��ĺ��
}

//�Կ��п���кϲ�
static void *coalesce(void *bp)
{
	size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
	size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
	size_t size = GET_SIZE(HDRP(bp));

	//�����������ǰ����п���кϲ�
	if (prev_alloc && next_alloc)
	{
		;
	}
	else if (prev_alloc && !next_alloc)
	{
		//��Ҫ�ϲ��ĵĿ�ӿ��ж�����ȡ��
		fetchFromList(NEXT_BLKP(bp));

		size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
		PUT(HDRP(bp), PACK(size, 0));
		PUT(FTRP(bp), PACK(size, 0));
	}
	else if (!prev_alloc && next_alloc)
	{
		//��Ҫ�ϲ��ĵĿ�ӿ��ж�����ȡ��
		fetchFromList(PREV_BLKP(bp));

		size += GET_SIZE(HDRP(PREV_BLKP(bp)));
		PUT(FTRP(bp), PACK(size, 0));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		bp = PREV_BLKP(bp);
	}
	else
	{
		//��Ҫ�ϲ��ĵĿ�ӿ��ж�����ȡ��
		fetchFromList(NEXT_BLKP(bp));
		fetchFromList(PREV_BLKP(bp));

		size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
		bp = PREV_BLKP(bp);
	}

	//�����ͷŵĿ���ڿ��ж���ͷ��
	PUT(bp, &head);
	PUT((char*)bp + (1 * WSIZE), head.next);
	if (head.next != NULL)
		PUT(head.next, bp);
	head.next = bp;


	return bp;
}

//�ԶѵĴ�С������չ
static void *extendHeap(size_t words)
{
	char *bp;
	size_t size;
	size_t temp;
	//�ڱ�֤����ĵ�ַ˫�ֶ���Ļ����Ϸ���һ���ڴ��
	size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
	if ((long)(bp = (char*)memSbrk(size)) == -1)
		return NULL;

	temp = PACK(size, 0);
	//������·���Ŀ�����ʼ��
	PUT(HDRP(bp), PACK(size, 0));			//0������п�
	PUT(FTRP(bp), PACK(size, 0));
	PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));	//�µĽ�β��ͷ��

	return coalesce(bp);
}

//�ڿ���������Ѱ��һ�����ʵ��ڴ�������з��䣬����ʹ�����ܴ�����ķ���
static void *findFit(size_t asize)
{
	PWSIZE pmatch = head.next;

	while (pmatch != NULL)
	{
		size_t temp = GET_SIZE(HDRP(pmatch));
		if (GET_SIZE(HDRP(pmatch)) >= asize)
		{
			//��ѡ�еĵĿ�ӿ��ж�����ȡ��
			fetchFromList(pmatch);

			return (void*)pmatch;
		}


		pmatch = GET((char*)pmatch + WSIZE);
	}

	return NULL;
}

//�����������ڿ��п����ʼλ�ã������ʣ�ಿ�ִ�С���ڵ�����С��Ĵ�Сʱ�����зָ�
static void place(void *bp, size_t asize)
{
	if ((GET_SIZE(HDRP(bp)) - asize) < MINBLOCK)
	{
		PUT(HDRP(bp), PACK(GET_SIZE(HDRP(bp)), 1));
		PUT(FTRP(bp), PACK(GET_SIZE(FTRP(bp)), 1));
	}
	else
	{
		size_t size = GET_SIZE(HDRP(bp)) - asize;
		PUT(FTRP(bp), PACK(size, 0));		//�¿�β��
		PUT(HDRP(bp), PACK(asize, 1));		//�����ͷ��
		PUT(FTRP(bp), PACK(asize, 1));		//�����β��
		PUT(HDRP(NEXT_BLKP(bp)), PACK(size, 0));	//�¿�ͷ��

		//���²����Ŀ���ڿ��ж���ͷ��
		PUT(NEXT_BLKP(bp), (void*)&head);
		PUT(NEXT_BLKP(bp) + WSIZE, (void*)head.next);
		if (head.next != NULL)
			PUT((void*)head.next, NEXT_BLKP(bp));
		head.next = (char*)NEXT_BLKP(bp);
	}
}

int meminit(void)
{
	char *fbp;

	if ((heap_listp = (char*)memSbrk(4 * WSIZE)) == (void *)-1)
		return -1;
	PUT(heap_listp, 0);									//�������
	PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));		//���Կ�ͷ��
	PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));		//���Կ�β
	PUT(heap_listp + (3 * WSIZE), PACK(0,	  1));		//��β��
	heap_listp += (2 * WSIZE);							//��ָ��ָ�����Կ��м�

	head.next = NULL;
	head.prev = NULL;
	if ((fbp = (char*)extendHeap(CHUNKSIZE/WSIZE)) == NULL)			//�����һ�����п�
		return -1;

	head.next = fbp;									//����ͷ�ڵ�ָ��ָ���һ����
	PUT(head.next, &head);
	PUT(head.next + (1 * WSIZE), NULL);					//���Ⱥͺ��Ϊ����ͷ��

	return 0;
}

void *memmalloc(size_t size)
{
	size_t asize;
	size_t extendsize;
	char *bp;

	if (size == 0)
		return NULL;

	//���СҪ�ܹ�����ͷ����β���Լ�ǰ����ָ��
	if (size <= DSIZE)
		asize = 4 * DSIZE;
	else
		asize = DSIZE * ((size + (DSIZE)+(DSIZE - 1)) / DSIZE);


	//Ѱ�Һ��ʴ�С�Ŀ�
	if ((bp = (char*)findFit(asize)) != NULL)
	{
		place(bp, asize);
		return bp;
	}

	//û�к��ʴ�С�Ŀ��Ҫ��������
	extendsize = MAX(asize,CHUNKSIZE);
	if ((bp = (char*)extendHeap(extendsize/WSIZE)) == NULL)
		return NULL;

	//���·���Ŀ���ڿ��ж���ͷ��
	PUT(bp, &head);
	PUT(bp + (1 * WSIZE), (void*)head.next);
	if (head.next != NULL)
		PUT((void*)head.next, FTRP(bp) + DSIZE);
	head.next = bp;

	place(bp, asize);

	return bp;
}

void memfree(void *bp)
{
	size_t size = GET_SIZE(HDRP(bp));

	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0));
	coalesce(bp);
}
