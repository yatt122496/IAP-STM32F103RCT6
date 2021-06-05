#include <malloc.h>
#include <assert.h>
#include "queue.h"

#define MaxQueueSize 100

struct queueCDT{
	queueElementT elements[MaxQueueSize];											//存放队列元素的数组
	int iHead;																								//队列头的下标
	int iCount;																								//队列元素的个数
};

queueADT NewQueue(void)																			//创建队列
{
	queueADT queue;
	queue = (queueADT)mymalloc(sizeof(struct queueCDT));
	queue->iHead = queue->iCount = 0;
	return (queue);
}

void FreeQueue(queueADT queue)															//销毁队列
{
	myfree(queue);
}

void Enqueue(queueADT queue,queueElementT x)								//入队列
{
	assert(!QueueIsFull(queue));
	queue->elements[(queue->iHead + queue->iCount)%MaxQueueSize] = x;
	queue->iCount++;
}

queueElementT Dequeue(queueADT queue)												//出队列
{
	queueElementT result;
	
	assert(!QueueIsEmpty(queue));
	result = queue->elements[queue->iHead];
	queue->iHead = (queue->iHead + 1)%MaxQueueSize;
	queue->iCount--;
	return (result);
}

bool QueueIsEmpty(queueADT queue)														//判断队列是够为空
{
	return (bool)(queue->iCount == 0);
}

bool QueueIsFull(queueADT queue)														//判断队列是否满
{
	return (bool)(queue->iCount == MaxQueueSize);
}

int QueueLength(queueADT queue)															//计算队列长度
{
	return (queue->iCount);
}

queueElementT GetQueueElement(queueADT queue,int index)			//取得队列元素
{
	assert(index >= 0&&index < QueueLength(queue));
	return (queue->elements[(queue->iHead + index)%MaxQueueSize]);
}
