
//����һ���洢��ģ��

#define MAX_HEAP	10000	//ģ���ڴ�ռ����Ĵ�С

//@����һ���ڴ�ϵͳģ��
void memInit(void);

//@ģ��ϵͳ�е�sbrk��������չ�Ѵ�С������û��ʵ�ֶѵ���������
void *memSbrk(int incr);