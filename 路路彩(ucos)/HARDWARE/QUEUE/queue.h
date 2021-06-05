#ifndef __queue_h
#define __queue_h

typedef enum boolean{FALSE,TRUE} bool;	
typedef void *queueElementT;																	//与类型无关的任意类型的定义
typedef struct queueCDT *queueADT;														//队列抽象的类型定义

queueADT NewQueue(void);																			//创建队列
void FreeQueue(queueADT queue);																//销毁队列
void Enqueue(queueADT queue,queueElementT x);									//入队列
queueElementT Dequeue(queueADT queue);												//出队列
bool QueueIsEmpty(queueADT queue);														//判断队列是够为空
bool QueueIsFull(queueADT queue);															//判断队列是否满
int QueueLength(queueADT queue);															//计算队列长度
queueElementT GetQueueElement(queueADT queue,int index);			//取得队列元素

#endif
