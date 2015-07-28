
//基本的数据结构和常用的宏

#define WSIZE		(sizeof(int))
#define DSIZE		(sizeof(int)*2)
#define CHUNKSIZE	(1 << 12)		//一次最小分配的内存大小 4096 个字节

#define MINBLOCK	(1 << 6)		//最小块的大小为64B

#define PWSIZE		unsigned int*

#define MAX(x,y)	((x) > (y)?(x):(y))
#define MIN(x,y)	((x) < (y)?(x):(y))

//基于双链表形式的空闲块的结构
	//int blockHead;	//块头部
	//memBlock *pred;	//前向指针，指向上一个未分配的块（算在有效载荷部分）
	//memBlock *succ;	//后向指针，指向下一个未分配的块
	//有效载荷
	//<填充部分>
	//int bolckTail;	//块尾部

//已分配块的结构
	//int blockHead;	//块头部
	//有效载荷
	//<填充部分>
	//int bolckTail;	//块尾部

//用于将块的大小与后三位已分配的位结合起来，用于块的头部和尾部
#define PACK(size,alloc)	((size)|(alloc))

//在地址p上对一个字进行读写
#define GET(p)				(*(PWSIZE)(p))
#define PUT(p,val)			(*(PWSIZE)(p) = (val))

//返回块的大小与已分配位 必须对块的头部或尾部进行操作
#define GET_SIZE(p)			(GET(p) & ~0x7)
#define GET_ALLOC(p)		(GET(p) &  0x1)

//通过块指针计算它的头部或尾部的地址
#define HDRP(bp)			((char*)(bp) - WSIZE)
#define FTRP(bp)			((char*)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

//计算前一个或后一个块的地址
#define NEXT_BLKP(bp)			((char*)(bp) + GET_SIZE(((char*)(bp) - WSIZE)))
#define PREV_BLKP(bp)			((char*)(bp) - GET_SIZE(((char*)(bp) - DSIZE)))