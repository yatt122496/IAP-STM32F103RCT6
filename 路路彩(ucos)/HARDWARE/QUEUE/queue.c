#include <malloc.h>
#include <assert.h>
#include "queue.h"

#define MaxQueueSize 100

struct queueCDT{
	queueElementT elements[MaxQueueSize];											//��Ŷ���Ԫ�ص�����
	int iHead;																								//����ͷ���±�
	int iCount;																								//����Ԫ�صĸ���
};

queueADT NewQueue(void)																			//��������
{
	queueADT queue;
	queue = (queueADT)mymalloc(sizeof(struct queueCDT));
	queue->iHead = queue->iCount = 0;
	return (queue);
}

void FreeQueue(queueADT queue)															//���ٶ���
{
	myfree(queue);
}

void Enqueue(queueADT queue,queueElementT x)								//�����
{
	assert(!QueueIsFull(queue));
	queue->elements[(queue->iHead + queue->iCount)%MaxQueueSize] = x;
	queue->iCount++;
}

queueElementT Dequeue(queueADT queue)												//������
{
	queueElementT result;
	
	assert(!QueueIsEmpty(queue));
	result = queue->elements[queue->iHead];
	queue->iHead = (queue->iHead + 1)%MaxQueueSize;
	queue->iCount--;
	return (result);
}

bool QueueIsEmpty(queueADT queue)														//�ж϶����ǹ�Ϊ��
{
	return (bool)(queue->iCount == 0);
}

bool QueueIsFull(queueADT queue)														//�ж϶����Ƿ���
{
	return (bool)(queue->iCount == MaxQueueSize);
}

int QueueLength(queueADT queue)															//������г���
{
	return (queue->iCount);
}

queueElementT GetQueueElement(queueADT queue,int index)			//ȡ�ö���Ԫ��
{
	assert(index >= 0&&index < QueueLength(queue));
	return (queue->elements[(queue->iHead + index)%MaxQueueSize]);
}
