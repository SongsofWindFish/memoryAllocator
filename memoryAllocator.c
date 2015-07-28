
//进行动态内存分配的代码

#include <stdio.h>
#include "basic.h"
#include "memModel.h"
#include "memoryAllocator.h"

static char *heap_listp;		//堆链表指针		
struct freeListBoundary
{
	char* prev;
	char* next;
}head;						//空闲块链表头部和尾部

//将块从空闲队列中取出
static void fetchFromList(void *bp)
{
	if (GET((char*)bp + WSIZE) != NULL)				//不是最后一个
		PUT(GET((char*)bp + WSIZE), GET(bp));		//下一块的祖先
	if (GET((char*)bp) == (char*)&head)				//第一块
		head.next = NULL;
	else
		PUT(GET(bp) + WSIZE, (char*)bp + WSIZE);	//上一块的后继
}

//对空闲块进行合并
static void *coalesce(void *bp)
{
	size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
	size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
	size_t size = GET_SIZE(HDRP(bp));

	//分四种情况对前后空闲块进行合并
	if (prev_alloc && next_alloc)
	{
		;
	}
	else if (prev_alloc && !next_alloc)
	{
		//将要合并的的块从空闲队列中取出
		fetchFromList(NEXT_BLKP(bp));

		size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
		PUT(HDRP(bp), PACK(size, 0));
		PUT(FTRP(bp), PACK(size, 0));
	}
	else if (!prev_alloc && next_alloc)
	{
		//将要合并的的块从空闲队列中取出
		fetchFromList(PREV_BLKP(bp));

		size += GET_SIZE(HDRP(PREV_BLKP(bp)));
		PUT(FTRP(bp), PACK(size, 0));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		bp = PREV_BLKP(bp);
	}
	else
	{
		//将要合并的的块从空闲队列中取出
		fetchFromList(NEXT_BLKP(bp));
		fetchFromList(PREV_BLKP(bp));

		size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
		bp = PREV_BLKP(bp);
	}

	//将新释放的块放在空闲队列头部
	PUT(bp, &head);
	PUT((char*)bp + (1 * WSIZE), head.next);
	if (head.next != NULL)
		PUT(head.next, bp);
	head.next = bp;


	return bp;
}

//对堆的大小进行扩展
static void *extendHeap(size_t words)
{
	char *bp;
	size_t size;
	size_t temp;
	//在保证分配的地址双字对齐的基础上分配一个内存块
	size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
	if ((long)(bp = (char*)memSbrk(size)) == -1)
		return NULL;

	temp = PACK(size, 0);
	//对这个新分配的块做初始化
	PUT(HDRP(bp), PACK(size, 0));			//0代表空闲块
	PUT(FTRP(bp), PACK(size, 0));
	PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));	//新的结尾块头部

	return coalesce(bp);
}

//在空闲链表中寻找一个合适的内存块来进行分配，这里使用了受次适配的方法
static void *findFit(size_t asize)
{
	PWSIZE pmatch = head.next;

	while (pmatch != NULL)
	{
		size_t temp = GET_SIZE(HDRP(pmatch));
		if (GET_SIZE(HDRP(pmatch)) >= asize)
		{
			//将选中的的块从空闲队列中取出
			fetchFromList(pmatch);

			return (void*)pmatch;
		}


		pmatch = GET((char*)pmatch + WSIZE);
	}

	return NULL;
}

//将请求块放置在空闲块的起始位置，当块的剩余部分大小大于等于最小块的大小时，进行分割
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
		PUT(FTRP(bp), PACK(size, 0));		//新块尾部
		PUT(HDRP(bp), PACK(asize, 1));		//分配块头部
		PUT(FTRP(bp), PACK(asize, 1));		//分配块尾部
		PUT(HDRP(NEXT_BLKP(bp)), PACK(size, 0));	//新块头部

		//将新产生的块放在空闲队列头部
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
	PUT(heap_listp, 0);									//对齐填充
	PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));		//序言块头部
	PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));		//序言块尾
	PUT(heap_listp + (3 * WSIZE), PACK(0,	  1));		//结尾块
	heap_listp += (2 * WSIZE);							//堆指针指向序言块中间

	head.next = NULL;
	head.prev = NULL;
	if ((fbp = (char*)extendHeap(CHUNKSIZE/WSIZE)) == NULL)			//分配第一个空闲块
		return -1;

	head.next = fbp;									//链表头节点指针指向第一个块
	PUT(head.next, &head);
	PUT(head.next + (1 * WSIZE), NULL);					//祖先和后继为链表头部

	return 0;
}

void *memmalloc(size_t size)
{
	size_t asize;
	size_t extendsize;
	char *bp;

	if (size == 0)
		return NULL;

	//块大小要能够包含头部和尾部以及前后向指针
	if (size <= DSIZE)
		asize = 4 * DSIZE;
	else
		asize = DSIZE * ((size + (DSIZE)+(DSIZE - 1)) / DSIZE);


	//寻找合适大小的块
	if ((bp = (char*)findFit(asize)) != NULL)
	{
		place(bp, asize);
		return bp;
	}

	//没有合适大小的块就要重新申请
	extendsize = MAX(asize,CHUNKSIZE);
	if ((bp = (char*)extendHeap(extendsize/WSIZE)) == NULL)
		return NULL;

	//将新分配的块放在空闲队列头部
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
