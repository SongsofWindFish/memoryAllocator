
//���������ݽṹ�ͳ��õĺ�

#define WSIZE		(sizeof(int))
#define DSIZE		(sizeof(int)*2)
#define CHUNKSIZE	(1 << 12)		//һ����С������ڴ��С 4096 ���ֽ�

#define MINBLOCK	(1 << 6)		//��С��Ĵ�СΪ64B

#define PWSIZE		unsigned int*

#define MAX(x,y)	((x) > (y)?(x):(y))
#define MIN(x,y)	((x) < (y)?(x):(y))

//����˫������ʽ�Ŀ��п�Ľṹ
	//int blockHead;	//��ͷ��
	//memBlock *pred;	//ǰ��ָ�룬ָ����һ��δ����Ŀ飨������Ч�غɲ��֣�
	//memBlock *succ;	//����ָ�룬ָ����һ��δ����Ŀ�
	//��Ч�غ�
	//<��䲿��>
	//int bolckTail;	//��β��

//�ѷ����Ľṹ
	//int blockHead;	//��ͷ��
	//��Ч�غ�
	//<��䲿��>
	//int bolckTail;	//��β��

//���ڽ���Ĵ�С�����λ�ѷ����λ������������ڿ��ͷ����β��
#define PACK(size,alloc)	((size)|(alloc))

//�ڵ�ַp�϶�һ���ֽ��ж�д
#define GET(p)				(*(PWSIZE)(p))
#define PUT(p,val)			(*(PWSIZE)(p) = (val))

//���ؿ�Ĵ�С���ѷ���λ ����Կ��ͷ����β�����в���
#define GET_SIZE(p)			(GET(p) & ~0x7)
#define GET_ALLOC(p)		(GET(p) &  0x1)

//ͨ����ָ���������ͷ����β���ĵ�ַ
#define HDRP(bp)			((char*)(bp) - WSIZE)
#define FTRP(bp)			((char*)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

//����ǰһ�����һ����ĵ�ַ
#define NEXT_BLKP(bp)			((char*)(bp) + GET_SIZE(((char*)(bp) - WSIZE)))
#define PREV_BLKP(bp)			((char*)(bp) - GET_SIZE(((char*)(bp) - DSIZE)))