#ifndef __queue_h
#define __queue_h

typedef enum boolean{FALSE,TRUE} bool;	
typedef void *queueElementT;																	//�������޹ص��������͵Ķ���
typedef struct queueCDT *queueADT;														//���г�������Ͷ���

queueADT NewQueue(void);																			//��������
void FreeQueue(queueADT queue);																//���ٶ���
void Enqueue(queueADT queue,queueElementT x);									//�����
queueElementT Dequeue(queueADT queue);												//������
bool QueueIsEmpty(queueADT queue);														//�ж϶����ǹ�Ϊ��
bool QueueIsFull(queueADT queue);															//�ж϶����Ƿ���
int QueueLength(queueADT queue);															//������г���
queueElementT GetQueueElement(queueADT queue,int index);			//ȡ�ö���Ԫ��

#endif
