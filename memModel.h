
//构建一个存储器模型

#define MAX_HEAP	10000	//模拟内存空间最大的大小

//@构建一个内存系统模型
void memInit(void);

//@模拟系统中的sbrk函数，扩展堆大小，但是没有实现堆的收缩功能
void *memSbrk(int incr);