
//进行动态内存分配的函数

#include <stdio.h>
#include "basic.h"
#include "memModel.h"

//所有分配前的内存初始化
int meminit(void);

//分配
void *memmalloc(size_t size);

//释放
void memfree(void *bp);